#include "../headers/colliders.h"
#include "../headers/game.h"
#include "../headers/raylib_helper.h"

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
    //calculate collision based on if any of our 4 sides intersects with terrain

     if (Globals::Game.terrain.get_gravityMode() == GlobalTerrain::DOWN)
    {
       /* Vector2 botLeft = o.pos + Vector2Normalize(o.getNormal() - o.getFacingVector())*sqrt(width*width+height*height)/2;
        Vector2 botRight = o.pos + Vector2Normalize(o.getNormal() + o.getFacingVector())*sqrt(width*width+height*height)/2;
        Vector2 intersect = t.lineBlockIntersect(botLeft,botRight,true);
        Debug::addDeferRender([intersect,botLeft,botRight,pos=o.pos](){
                              DrawSphere(toVector3(botLeft),2,RED);
                              DrawSphere(toVector3(botRight),2,BLUE);
                              DrawSphere(toVector3(pos),2,BLACK);
                              DrawSphere(toVector3(intersect),2,PURPLE);

                              });
        return  !Vector2Equals(botRight,intersect);*/

        Debug::addDeferRender([pos=o.pos + Vector2(0,height/2)](){

                            DrawSphere(toVector3(pos),1,PURPLE);

                              });

        return t.isBlockType(o.pos + Vector2(0,height/2),SOLID);
    }
    else
    {
        Vector2 last = o.pos;
        Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left

        for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left
        {
            int index =(i + 1)%4;
            Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(((index%3) != 0)*2 - 1,index/2*2 - 1),o.rotation);

            //PossiblePoint intersect = t.lineIntersectWithTerrain(prevCorner,corner);
            Vector2 intersect = t.lineBlockIntersect(prevCorner,corner,true);

            //if (intersect.exists)
            if (!Vector2Equals(intersect,corner)) //something is in the way!
            {
                return true;
            }

            prevCorner = corner;
        }
        return false;
    }
}

float RectCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left


    Vector2 ground = {}; //vector towrads the ground, calculated by adding all intersections
    for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left corners
    {
        int index =(i + 1)%4;
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(((index%3) != 0)*2 - 1,index/2*2 - 1),o.rotation);
        Vector2 intersect = terrain.lineBlockIntersect(corner,prevCorner,true);

       if (terrain.isBlockType(intersect,SOLID,true))
        //if (terrain.blockExists(intersect,true,true))
        {
            //ground += o.pos - intersect2.pos;
            ground += intersect - o.pos;
        }

        prevCorner = corner;
    }
    if (Vector2Equals(ground,{}))
        {
            return o.rotation;
        }
    return atan2(ground.y,ground.x) + M_PI*3/2;
}

