#ifndef TRIGGERS_H_INCLUDED
#define TRIGGERS_H_INCLUDED

#include <functional>
#include <iostream>

typedef std::function<void(void*)> SignalCallback;
typedef std::string SignalName;
typedef std::unordered_map<SignalName,SignalCallback> SignalSet; //mapping of a signal name to a signal callback     

SignalSet getWorldsSet(size_t worldID); //given the ith world, return a set of signals


#endif // TRIGGERS_H_INCLUDED
