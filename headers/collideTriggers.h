#ifndef COLLIDETRIGGERS_H_INCLUDED
#define COLLIDETRIGGERS_H_INCLUDED

#include <functional>
#include "raylib.h"

//#include "item.h"

//things to do on collide
#include <iostream>
class PhysicsBody;
struct HoldThis
{
    void collideWith(PhysicsBody& self, PhysicsBody& other);

    HoldThis()
    {

    }
    HoldThis(HoldThis&& other)
    {

    }
    HoldThis(const HoldThis& other)
    {

    }
};

struct KeyComponent
{
    size_t keyID = 0;
    size_t getKey()
    {
        return keyID;
    }
};

struct InteractComponent
{
    typedef std::function<void(PhysicsBody&, PhysicsBody&)> InteractAction;

    InteractAction func;
    InteractComponent(InteractAction func_) : func(func_)
    {

    }
    void collideWith(PhysicsBody& self, PhysicsBody& other)
    {
        if (IsKeyPressed(KEY_E))
        {
            func(self,other);
        }
    }
};

#endif // COLLIDETRIGGERS_H_INCLUDED
