#ifndef COLLIDETRIGGERS_H_INCLUDED
#define COLLIDETRIGGERS_H_INCLUDED

#include <functional>
#include "raylib.h"

//things to do on collide

#include <iostream>

#include "conversions.h"
#include "factory.h"

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

struct OnTrigger
{
    virtual void operator()(PhysicsBody& owner) = 0;
    virtual std::string to_string() = 0;
};

//on trigger, spawn planets at each of the given points
struct TriggerSpawnPlanets : public OnTrigger
{
    double duration = 0; //how long the terrain should persist, negative if you want forever.
    struct Circle
    {
        Vector2 center;
        int radius;
        Color color;
        bool absolute = false; //true if position is absolute, false if relative to object that calls this
        using Factory = FactoryBase<Circle,
                            access<Circle,&Circle::center>,
                            access<Circle,&Circle::radius>,
                            access<Circle,&Circle::color>,
                            access<Circle,&Circle::absolute>>;
    };
    std::vector<Circle> points;

    TriggerSpawnPlanets(const SplitString& split);
    std::string to_string();
    void operator()(PhysicsBody&);
};

#endif // COLLIDETRIGGERS_H_INCLUDED
