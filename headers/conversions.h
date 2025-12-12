#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED

//a bunch of helper functions that convert a string into some type or convert a type into a string. Used heavily when loading levels

#include <vector>
#include <iostream>

#include "game.h"

//class that represents the indicies of a split string.
//uses a string_view, so has to not get outlived by some string-like object.
//DOES NOT SUPPORT MULTIPLE CONSECUTIVE DELIMITTERS NOR STARTING WITH A DELIMITTER
//a single dangling delimiter at the very end is fine.
struct SplitString
{
    //each index is the first character of a valid string; the string size + 1 is also tacked on as the last element.
    //so nums[index + 1] - nums[index] - 1 is always valid substring.
    std::vector<size_t> nums;
    std::string_view str; //the underlying string view, make sure this never gets outlived

    //construct our split string
    SplitString(std::string_view str, char delimit = ',');

    //only indicies 0 - nums.size() - 1 are valid. All other indicies return "". Valid indicies return the index-th valid substring
    std::string_view operator[](size_t index) const;
    size_t size() const;
};

std::vector<size_t> split(std::string_view str, char delimit = ',');

template<typename T>
T fromString(std::string_view);

template<typename T>
std::string toString(T);


template<typename T>
concept Arith = std::is_arithmetic_v<T>;


template<Arith A>
std::string toString(A f)
{
    return std::to_string(f);
}

#endif // CONVERSIONS_H_INCLUDED
