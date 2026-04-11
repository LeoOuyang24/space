#include "../headers/colliders.h"
#include "../headers/game.h"
#include "../headers/raylib_helper.h"
#include "../headers/objects.h"

bool CircleCollider::isOnGround(const PhysicsBody& body, Terrain& terrain)
{

    return terrain.blockExists(body.getShape());
}

float CircleCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
    int count = 10;
    for (int i = 0; i < count; i ++)
    {
        float radians = 360.0f/count*DEG2RAD*i;
        Vector2 point = o.pos + Vector2(cos(radians),sin(radians))*radius;

        if (terrain.blockExists(point))
        {
            return radians + M_PI*3/2;
        }
    }
    return o.rotation;
}


bool RectCollider::isOnGround(PhysicsBody& body, Terrain& t)
{
    Orient o = body.getOrient();

    //if gravity is down and we are not jumping, we are considered onGround if a bottom side (nonrotated) is in the ground
    //however, if gravity is down and we are jumping, use regular onGround checking; this prevents an issue where we are still "on the ground"
    //one frame after jumping and don't end up going anywhere.
    if (Globals::Game.terrain.get_gravityMode() == GlobalTerrain::DOWN &&
        Vector2LengthSqr(body.getForces().getForce(Forces::JUMP)) <= Vector2LengthSqr(body.getForces().getForce(Forces::GRAVITY)))
    {
        Vector2 botLeft = o.pos + Vector2(-width/2*0.5,height/2);
        Vector2 botRight = o.pos + Vector2(width/2*0.5,height/2);
        Vector2 botCenter = o.pos + Vector2(0,height/2);

        Color c = t.isBlockType(botLeft,SOLID) ? RED : BLUE;
        Color c2 = t.isBlockType(botRight,SOLID) ? RED : BLUE;

        Debug::addDeferRender([botLeft,botRight,pos=o.pos,c,c2](){
                              DrawSphere(toVector3(botLeft),2,c);
                              DrawSphere(toVector3(botRight),2,c2);
                              //DrawSphere(toVector3(pos),2,BLACK);
                             // DrawSphere(toVector3(intersect),2,PURPLE);

                              });

        return (t.isBlockType(botLeft,SOLID) || t.isBlockType(botRight,SOLID) || t.isBlockType(botCenter,SOLID));
    } 
    else     //calculate collision based on if any of our 4 sides intersects with terrain
    {
        return t.blockExists(body.getShape());
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
    return atan2(ground.y,ground.x) - M_PI/2;
}

