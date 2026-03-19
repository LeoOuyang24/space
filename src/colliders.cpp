#include "../headers/colliders.h"
#include "../headers/game.h"
#include "../headers/raylib_helper.h"
#include "../headers/objects.h"

bool CircleCollider::isOnGround(const PhysicsBody& body, Terrain& terrain)
{

    return  terrain.blockExists(body.orient.pos + Vector2Rotate(body.orient.getNormal(),+ M_PI/4)*radius) ||
            terrain.blockExists(body.orient.pos + body.orient.getNormal()*radius) ||
            terrain.blockExists(body.orient.pos + Vector2Rotate(body.orient.getNormal(),- M_PI/4)*radius);
}

float CircleCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
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
        Vector2 botLeft = o.pos + Vector2(-width/2,height/2);
        Vector2 botRight = o.pos + Vector2(width/2,height/2);
        Vector2 botCenter = o.pos + Vector2(0,height/2);

        Debug::addDeferRender([botLeft,botRight,pos=o.pos](){
                              DrawSphere(toVector3(botLeft),2,RED);
                              DrawSphere(toVector3(botRight),2,BLUE);
                              //DrawSphere(toVector3(pos),2,BLACK);
                             // DrawSphere(toVector3(intersect),2,PURPLE);

                              });

        return t.isBlockType(botLeft,SOLID) || t.isBlockType(botRight,SOLID) || t.isBlockType(botCenter,SOLID);
    }
    else     //calculate collision based on if any of our 4 sides intersects with terrain
    {
        Vector2 last = o.pos;
       // bool down = Globals::Game.terrain.get_gravityMode() == GlobalTerrain::DOWN;
        //int start = down ? int((abs(o.rotation*RAD2DEG)/45) + 1 ) % 4: 0;
        //int upTo = down ? (start + 2) : 4;
        for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left
        {
            int prev = (i);
            int index =(i + 1)%4;

            Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(((prev%3) != 0)*2 - 1,prev/2*2 - 1),o.rotation); //top left
            Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(((index%3) != 0)*2 - 1,index/2*2 - 1),o.rotation);

            Debug::addDeferRender([corner,prevCorner](){

                                  DrawSphere(toVector3(corner),4,RED);
                                  DrawSphere(toVector3(prevCorner),4,RED);

                                  });


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

