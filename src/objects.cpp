#include <iostream>

#include "../headers/objects.h"

void PhysicsBody::setPos(const Vector2& pos)
{
    orient.pos = pos;
}

void PhysicsBody::setDead(bool val)
{
    dead = val;
}

bool PhysicsBody::getDead()
{
    return dead;
}

bool PhysicsBody::isDead()
{
    return dead;
}

void Forces::setForce(const Vector2& force, Forces::ForceSource source)
{
    Vector2 old = getForce(source);
    totalForce -= old;
    totalForce += force;
    forces[source] = force;
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

void Forces::addFriction(const Vector2& friction)
{
    for (auto& source : forces)
    {
        source.second *= friction;
    }
    totalForce *= friction;
}

void Forces::addFriction(float friction)
{
    for (auto& source : forces)
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
    return Vector2LengthSqr(totalForce) > 100 ? Vector2Normalize(totalForce)*10 : totalForce;
}
