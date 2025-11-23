#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED

//a bunch of helper functions that convert a string into some type or convert a type into a string. Used heavily when loading levels

#include <vector>
#include <iostream>

#include "game.h"

std::vector<std::string> split(std::string str, char delimit = ',');

template<typename T>
T fromString(std::string);

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
