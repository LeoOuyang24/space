#include "../headers/../headers/colliders.h"


bool CircleCollider::isOnGround(const Orient& orient, Terrain& terrain)
{

    return  terrain.blockExists(orient.pos + Vector2Rotate(orient.getNormal(),+ M_PI/4)*radius) ||
            terrain.blockExists(orient.pos + orient.getNormal()*radius) ||
            terrain.blockExists(orient.pos + Vector2Rotate(orient.getNormal(),- M_PI/4)*radius);
}

float CircleCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
    return o.rotation;
}

bool RectCollider::isOnGround(const Orient& orient, Terrain& terrain)
{
    return  terrain.blockExists(rotatePoint(orient.pos + Vector2(width/2,  height/2),orient.pos,orient.rotation)) ||
            terrain.blockExists(rotatePoint(orient.pos + Vector2(-width/2,  -height/2),orient.pos,orient.rotation)) ||
            terrain.blockExists(rotatePoint(orient.pos + Vector2(width/2,  -height/2),orient.pos,orient.rotation)) ||
            terrain.blockExists(rotatePoint(orient.pos + Vector2(-width/2,  height/2),orient.pos,orient.rotation));
}

float RectCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left


    Vector2 ground = {}; //vector towrads the ground, calculated by adding all intersections
    for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left
    {
        int index =(i + 1)%4;
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(index/2*2 - 1,((index%3) != 0)*2 - 1),o.rotation);

        PossiblePoint intersect = terrain.lineIntersectWithTerrain(prevCorner,corner);

        if (intersect.exists)
        {
            ground += o.pos - intersect.pos;
        }

        prevCorner = corner;
    }

    if (Vector2Equals(ground,{0,0}))
        {
            return o.rotation;
        }

    return atan2(ground.y,ground.x) + M_PI/2;
}


Rectangle RectCollider::getRect(const Orient& orient)
{
    return Rectangle(orient.pos.x - width/2, orient.pos.y - height/2, width,height);
}
