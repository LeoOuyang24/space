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
    virtual void interact(PhysicsBody& self, PhysicsBody& other)
    {
        func(self,other);
    }
    void collideWith(PhysicsBody& self, PhysicsBody& other)
    {
        if (IsKeyPressed(KEY_E))
        {
            interact(self,other);
        }
    }
};



#endif // COLLIDETRIGGERS_H_INCLUDED
