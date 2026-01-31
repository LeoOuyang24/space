#include <iostream>

#include "../headers/objects.h"
#include "../headers/raylib_helper.h"

void PhysicsBody::setPos(const Vector2& pos)
{
    orient.pos = pos;
}

Orient PhysicsBody::getOrient()
{
    return orient;
}

void PhysicsBody::setOrient(const Orient& orient)
{
    this->orient = orient;
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

bool PhysicsBody::isTangible()
{
    return true;
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
    return Vector2LengthSqr(totalForce) > 400 ? Vector2Normalize(totalForce)*20 : totalForce;
}

void suggestButtonPress(const Shape& shape, std::string_view str)
{
    Vector3 pos = {0,0,Globals::Game.terrain.getZOfLayer(shape.orient.layer)};
    assignVector(pos,shape.orient.pos - shape.orient.getNormal()*(50 + GetDimen(shape).y/2.0));
    Vector2 dimen = MeasureTextEx(GetFontDefault(),str.data(),30,10);
    DrawCube(pos,dimen.x*1.5,dimen.y*1.1,0,static_cast<int>(GetTime()*1000)%1000 < 500 ? WHITE : GRAY);
    DrawCubeWires(pos,dimen.x*1.5,dimen.y*1.1,0,BLACK);
    DrawText3D(GetFontDefault(),str.data(),pos,30,10,0,false,BLACK,TextAlign::CENTER);
}
