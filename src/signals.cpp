#include "../headers/signals.h"
#include "../headers/objects.h"
#include "../headers/portal.h"

//spawn an object relative to "caller"
template<typename T>
T* spawn(void* caller, const Vector2& disp)
{
    PhysicsBody* spawner = static_cast<PhysicsBody*>(caller); //the object that sent the signal
    T* t = new T();
    t->setOrient(spawner->getOrient());
    t->setPos(t->getPos() + disp);
    Globals::Game.addObject(*t,spawner->getOrient().layer);
    return t;
}

struct CounterWrapper //wrap a signal callback such that it'll only trigger after being called a number of times
{
    int limit = 0;
    SignalCallback callback;
    int x = 0;
    void operator()(void* thing)
    {
        x ++;
        if (x == limit) //== so it can't trigger multiple times
        {
            callback(thing);
        }
    }
};

SignalSet world1()
{
    SignalSet sigs;

    sigs["spawn_basic_portal"] = [](void* thing){

        Portal* p = spawn<Portal>(thing,{0,-100});
            p->cond.reset(nullptr);
            p->destPos = {1833,3015};
            p->layerDisp = 1;
    };

    sigs["spawn_bonus_portal"] = [](void* thing){

        Portal* p = spawn<Portal>(thing,{0,100});
            p->destPos = {3000,4599};            

    };

    sigs["trigger1"] = CounterWrapper{2,[](void* thing){
        Portal* p = new Portal();
            p->setPos({3704,4733});
            p->cond.reset();
            p->destPos = {2433,1020};
        Globals::Game.addObject(*p,static_cast<PhysicsBody*>(thing)->getOrient().layer);
    }};

    sigs["finish_golf_course"] = [](void* thing){



    };


    return sigs;
}

SignalSet getWorldsSet(size_t i)
{
    switch (i)
    {
        case 1:
            return world1();
        default:
            std::cerr << "ERROR getWorldsSet: hmm, failed to get a signal set for " << i << "\n";
    }
    return {};
}