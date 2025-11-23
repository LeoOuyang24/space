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


template<typename Obj>
auto& access(Obj& t)
{
    return t;
}
/***

It would be really nice if we could access a member variable of a member variable but that syntax doesn't exist by default. So to access field C of class B
which is a field of class A, we have to use this function, which just returns a reference to A::B::C given an instance of A.

This is used in FactoryBase. Every FactoryBase contains a typename (Object we want to create) and a list of access() calls to access fields in the type.
These access calls can be used to set/read each field we want of the given typename.

***/
template<typename Obj,auto Member, auto... Members>
auto& access(Obj& thing)
{
    return access<decltype(thing.*Member),Members...>(thing.*Member);
}

class PhysicsBody;
//given a string, create the corresponding Object based on the name in the first part of the string.
std::shared_ptr<PhysicsBody> construct(std::string cereal);

//not actually defined here. look in factories.h for specializations. Each object we want to be able to serialize/deserialize has a specialization
template<typename Obj>
struct Factory;

/***
Workhorse class. "Obj" should be a type (probably a child of PhysicsBody) and Accessors should be a parameter pack of access<>() calls..
Each Accessor represents a field in "Obj" that we want to access, either to set a value or to convert into a string.
For this to work, "Obj" must be able to have a blank constructor
***/
template<typename Obj, auto... Accessors>
struct FactoryBase
{
    //string* because we are usually taking in the contiguous strings of a std::vector<string> in "construct"
    static Obj deserialize(std::string* params)
    {
        Obj obj;
        [&obj,&params]<size_t... Index>(std::index_sequence<Index...>)
        {
            //set each field to a value that is converted from a string
            //"Accessors(obj)" returns a reference to the corresponding field
            //decltype lets us decide what type to convert into, based on the type of the field in "Obj"
            ((Accessors(obj) = fromString<typename std::remove_reference<decltype(Accessors(obj))>::type>(params[Index])),...);

        }(std::make_index_sequence<sizeof...(Accessors)>{}); //<-- allows us to iterate through each accessor. "Index" is 0 - however many Accessors
        return obj;
    }

    //if an Object is defined with itself as part of the Object template parameters, its "serialize" function will automatically call this.
    //so for example if Key = public Object<Collider,Renderer,Key>, Key::serialize will be Factory<Key>::serialize
    static std::string serialize(Obj& object)
    {
        std::string cereal = "";
        //concatenate each serialized version of each field
        ((cereal += (toString<typename std::remove_reference<decltype(Accessors(object))>::type>(Accessors(object)) + " ")),...);

        return cereal;
    }
};



#endif // FACTORY_H_INCLUDED
