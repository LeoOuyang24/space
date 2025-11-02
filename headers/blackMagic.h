#ifndef BLACKMAGIC_H_INCLUDED
#define BLACKMAGIC_H_INCLUDED

//metaprogramming? whens the last time you meta WOMAN


//this macro defines a concept that checks if T has a member function called FUNC.
//will be false if T has multiple functions named FUNC
#define CHECK_FUNCTION(FUNC)                                 \
template <typename T>                                              \
concept has_##FUNC = requires { &T::FUNC; };

//an empty class to represent a lack of a class
struct EMPTY_TYPE
{

};

//CHECK_FUNCTION_LOOP macro defines a struct that uses recursive variadic struct to determine if a template list of typenames have a member function called FUNC
//or is a ConArgs of a type with that function
//the end result is a GET_TYPE_WITH_FUNC object that can check if a list of typenames have the corresponding member fucntion
//to utilize this macro, the corresponding has_FUNC has to also be declared.
#define CHECK_FUNCTION_LOOP(FUNC)\
\
template<typename...>\
struct GET_TYPE_WITH_##FUNC\
{\
    using type = EMPTY_TYPE;\
};\
\
template<typename A, typename... Args>\
struct GET_TYPE_WITH_##FUNC<A,Args...>\
{\
    using type = std::conditional<has_##FUNC<A>,\
                        A,\
                        typename std::conditional<has_##FUNC<typename A::type>,\
                            typename A::type,\
                            typename GET_TYPE_WITH_##FUNC<Args...>::type>\
                        ::type>\
                    ::type;\
};\
\
template<typename T>\
struct GET_TYPE_WITH_##FUNC<T>\
{\
    using type = std::conditional<has_##FUNC<T>,\
                    T,\
                    EMPTY_TYPE\
    >::type;\
};\




//finally, this macro sets up checking for the existence of FUNC.
//  CHECK_FUNCTION creates the concept that checks for FUNC.
//  CHECK_FUNCTION_LOOP creates the struct that can recursively check for FUNC
//example: GET_TYPE_WITH_foo<int, double, class_with_foo, class_without_foo> x; will set x to a type of "class_with_foo", or "EMPTY_TYPE" if none of these are valid
#define CHECK_FOR(FUNC)\
CHECK_FUNCTION(FUNC)\
CHECK_FUNCTION_LOOP(FUNC)

template<typename Class, typename... Args>
struct ConArgs //represents the arguments of a classes' constructor
{
    using type = Class;
    std::tuple<Args...> args;
};

template<typename Class, typename... Args>
ConArgs<Class,Args...> createArgs(Args... args) //create a ConArgs instance, apparently class constructors can't infer template parameters if you provide Class but not Args...
{
    return ConArgs<Class,Args...>{std::make_tuple<Args...>(std::move(args)...)};
}

template<typename Class>
Class createFromArgs()
{
   return Class();
}

template<typename Class, typename Head, typename... Tail>
Class createFromArgs(Head head, Tail... t) //given a class and a list of potential ConArgs, fetch the args that are used for Class and construct a Class
{
    if constexpr (std::is_same<Class,typename Head::type>::value)
    {
        return std::make_from_tuple<Class>(head.args);
    }
    else
    {
        return createFromArgs<Class,Tail...>(t...);
    }
}



#endif // BLACKMAGIC_H_INCLUDED
