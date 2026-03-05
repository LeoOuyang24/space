#ifndef FACTORY_H_INCLUDED
#define FACTORY_H_INCLUDED

//TLDR: stuff here handles serializing/deserializing objects. This is used to load/save levels

/***

- serializing: Object to string
- deserializing: string to Object

The premise here is that each object that can be loaded into a level has a corresponding Factory class:

ex: Factory<Key>

This class handles serializing/deserializing an Object. So Factory<Key>::Base::serialize would take a Key object and return its string representation. Conversely
Factory<Key>::Base::deserialize would take a string and create a Key object. You can see how this may be used to load levels, or save a level.

The workload is primarily done by FactoryBase, a template class that takes an object and pointers to object members. These are the fields that need to be
set/read when deserializing/serializing. From there, Factory<Key> is just a wrapper for a specific specialization of FactoryBase<Key,....>

***/

#include <iostream>
#include <array>

#include "conversions.h"

constexpr std::string EMPTY_SERIAL = ""; //empty serialization for objects that shouldn't be serialized



/***

It would be really nice if we could access a member variable of a member variable but that syntax doesn't exist by default. So to access field C of class B
which is a field of class A, we have to use this function, which just returns a reference to A::B::C given an instance of A.

This is used in FactoryBase. Every FactoryBase contains a typename (Object we want to create) and a list of access() calls to access fields in the type.
These access calls can be used to set/read each field we want of the given typename.

***/
template<typename Obj>
auto& access(Obj& t)
{
    return t;
}
template<typename Obj,auto Member, auto... Members>
auto& access(Obj& thing)
{
    return access<decltype(thing.*Member),Members...>(thing.*Member);
}

//sets a field. For most fields, this is pretty simple; simply set it to the serialized version
template<typename Accessed>
void setValue(const SplitString& params, Accessed& accessed, size_t index)
{
    if (index< params.size())
    {
        accessed = fromString<Accessed>(params[index]);
    }
}

//Look for specializations below class declarations. Each object we want to be able to serialize/deserialize has a specialization
template<typename Obj>
struct Factory;

//specialization for unique_ptr. We instead reset it to the raw pointer of the deserialized object
template<typename T >
void setValue(const SplitString& params, std::unique_ptr<T>& ptr, size_t index)
{
    if (index < params.size())
    {
        ptr.reset(fromString<T*>(params[index]));
    }
}

//default definition for objects that really should never be serialized
template<typename Obj>
struct Factory
{
    struct Base
    {
        static Obj deserialize(const SplitString& params)
        {
            return Obj();
        }
        static std::string serialize(Obj& obj)
        {
            return "";
        }
    };
};

/***
Workhorse class. "Obj" should be a type (probably a child of PhysicsBody) and Accessors should be a parameter pack of access<>() calls..
Each Accessor represents a field in "Obj" that we want to access, either to set a value or to convert into a string.
For this to work, "Obj" must be able to have a blank constructor
***/
template<typename Obj, auto... Accessors>
struct FactoryBase
{
    //number of accessors
    static constexpr size_t size = sizeof...(Accessors);

    //string* because we are usually taking in the contiguous strings of a std::vector<string> in "construct"
    //we can start at any index, however, we usually start at 1 because index 0 is usually the name of the object
    static Obj deserialize(const SplitString& params,size_t start = 1)
    {
        Obj obj;
        [&obj,&params,start]<size_t... Index>(std::index_sequence<Index...>)
        {
            //set each field to a value that is converted from a string
            //"Accessors(obj)" returns a reference to the corresponding field
            //decltype lets us decide what type to convert into, based on the type of the field in "Obj"
            //if fewer than required fields are provided, we go up as high as we can, based on length
            //we start at index + 1 because the first string is always the name of the object
            (setValue(params,Accessors(obj),start + Index),...);

        }(std::make_index_sequence<sizeof...(Accessors)>{}); //<-- allows us to iterate through each accessor. "Index" is 0 - however many Accessors
        return obj;
    }

    //if an Object is defined with itself as part of the Object template parameters, its "serialize" function will automatically call this.
    //so for example if Key = public Object<Collider,Renderer,Key>, Key::serialize will be Factory<Key>::serialize
    static std::string serialize(Obj& object,std::string objName = Factory<Obj>::ObjectName)
    {
        std::string cereal = "";
        //concatenate each serialized version of each field
        ((cereal += (toString(Accessors(object)) + "\t")),...);

        return objName + "\t" + cereal;
    }
};

//used to define a custom setter function for a particular field
//use this the same way you'd use access, but replace access with Setter<obj,setFunc,accessors...>::CustomSetter
template<typename Obj,auto SetFunc, auto... Accessors>
struct Setter
{
    using FieldType = decltype(access<Accessors...>(std::declval<Obj>()));
    FieldType& value;
    void operator=(FieldType&& other)
    {
        SetFunc(value,other);
    }
    static Setter CustomSetter(Obj& obj)
    {
        return Setter{access<Accessors...>(obj)};
    }
};

template<size_t N>
struct StringWrapper
{
  char str[N];
  constexpr StringWrapper(const char (&s)[N])
  {
      for (int i = 0; i < N; i ++)
      {
          str[i] = s[i];
      }
  }
};

//handles how objects are deserialized
class ClassDeserializer
{
    //stores every object and its deserialization function
   static std::unordered_map<std::string,std::function<PhysicsBody*(const SplitString& params)>> funcs;
public:
    //add an object and its name to our list
    template<typename Obj>
    static void registerName()
    {
        funcs[Factory<Obj>::ObjectName] = [](const SplitString& params){
            return new Obj(Factory<Obj>::Base::deserialize(params));
        };
    }
    //sometimes I'm too lazy to make a Factory. This registers an object at a name and just returns it with the default constructor
    template<typename Obj>
    static void registerName(std::string_view name)
    {
        funcs[name.data()] = [](const SplitString& params){
            return new Obj();
        };
    }
    //add all objects we need
    static void init();

    //construct from string
    static std::shared_ptr<PhysicsBody> construct(std::string_view cereal);
};

class PhysicsBody;
//given a string, create the corresponding Object based on the name in the first part of the string.
std::shared_ptr<PhysicsBody> construct(std::string cereal);
#endif // FACTORY_H_INCLUDED
