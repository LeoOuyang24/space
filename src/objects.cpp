#include <iostream>

#include "../headers/objects.h"

void PhysicsBody::setDead(bool val)
{
    dead = val;
}

bool PhysicsBody::isDead()
{
    return dead;
}

void Forces::addForce(Vector2 force, Forces::ForceSource source)
{
    if (forces.find(source) != forces.end())
    {
        forces[source] += force;
    }
    else
    {
        forces[source] = force;
    }

    totalForce += force;
}

void Forces::addFriction(float friction)
{
    for (auto source : forces)
    {
        source.second *= friction;
    }
    totalForce *= friction;
}

void Forces::addFriction(float friction, Forces::ForceSource source)
{
    if (forces.find(source) != forces.end())
    {
        totalForce -= forces[source];
        forces[source] *= friction;
        totalForce += forces[source];
    }
}

Vector2 Forces::getTotalForce()
{
    return totalForce;
}
