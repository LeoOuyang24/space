#include "../headers/colliders.h"
#include "../headers/game.h"

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

bool RectCollider::isOnGround(const Orient& o, Terrain& t)
{
    Vector2 last = o.pos;
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left

    for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left
    {
        int index =(i + 1)%4;
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(index/2*2 - 1,((index%3) != 0)*2 - 1),o.rotation);

        PossiblePoint intersect = t.lineIntersectWithTerrain(prevCorner,corner);

      /*  Debug::addDeferRender([prevCorner,corner,intersect](){
                                      DrawLineEx(prevCorner, corner,2, intersect.exists ? PURPLE : WHITE);

                              });*/

        if (intersect.exists)
        {
            return true;
        }

        prevCorner = corner;
    }
    return false;
}

float RectCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left


    Vector2 ground = {}; //vector towrads the ground, calculated by adding all intersections
    for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left corners
    {
        int index =(i + 1)%4;
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(index/2*2 - 1,((index%3) != 0)*2 - 1),o.rotation);

        PossiblePoint intersect = terrain.lineIntersectWithTerrain(prevCorner,corner);

        Debug::addDeferRender([intersect](){

                              DrawCircle3D({intersect.pos.x,intersect.pos.y,Globals::Game.getCurrentZ()},10,{},0,intersect.exists ? RED : BLUE);

                              });

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

