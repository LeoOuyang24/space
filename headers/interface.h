#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <iostream>
#include <memory>

class Sign;
class Interface
{
    //is set to true when signMessage is first set, which basically just means that the sign UI can't go down the same frame its activated
    bool justSet = false;
    std::weak_ptr<Sign> sign;
    size_t messageIndex = 0;
    void processSign();//handle rendering sign message
public:
    void setMessage(Sign* message);
    void render();
};

#endif // INTERFACE_H_INCLUDED
