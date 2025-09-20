#include <iostream>

#include "../headers/objects.h"

bool CircleCollider::isOnGround(const Orient& orient, Terrain& terrain)
{

    return  terrain.blockExists(orient.pos + Vector2Rotate(orient.getFacing(),+ M_PI/4)*radius) ||
            terrain.blockExists(orient.pos + orient.getFacing()*radius) ||
            terrain.blockExists(orient.pos + Vector2Rotate(orient.getFacing(),- M_PI/4)*radius);
}


bool RectCollider::isOnGround(const Orient& orient, Terrain& terrain)
{
    return  terrain.blockExists(rotatePoint(orient.pos + Vector2(width/2,  height/2),orient.pos,orient.rotation)) ||
            terrain.blockExists(rotatePoint(orient.pos + Vector2(-width/2,  -height/2),orient.pos,orient.rotation)) ||
            terrain.blockExists(rotatePoint(orient.pos + Vector2(width/2,  -height/2),orient.pos,orient.rotation)) ||
            terrain.blockExists(rotatePoint(orient.pos + Vector2(-width/2,  height/2),orient.pos,orient.rotation));
}


Rectangle RectCollider::getRect(const Orient& orient)
{
    return Rectangle(orient.pos.x - width/2, orient.pos.y - height/2, width,height);
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
