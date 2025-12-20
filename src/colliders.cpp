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
           /* std::cout << "INTERSECT: " << intersect << " " << corner << " " << prevCorner << "\n";
            std::cout << t.blockExists(corner) << " " << t.blockExists(prevCorner) << "\n";
                        Debug::addDeferRender([intersect](){

                                  //DrawCircle3D(toVector3(intersect.pos),1,{},0,RED);
                                  DrawSphere(toVector3(intersect),1,RED);

                                  });*/
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
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(((index%3) != 0)*2 - 1,index/2*2 - 1),o.rotation);


       // PossiblePoint intersect = terrain.lineIntersectWithTerrain(prevCorner,corner);
        //PossibleBlock intersect2 = terrain.lineBlockIntersect(prevCorner,corner,true);

       /* Debug::addDeferRender([intersect2,intersect,corner,index,&terrain](){
                              DrawCircle3D(toVector3(intersect2.pos),2,{},0,RED);
                              DrawCircle3D(toVector3(intersect.pos),2,{},0,BLUE);

                              DrawLine3D(toVector3(corner),toVector3(intersect.pos),RED);

                              //  DrawSphere(toVector3(corner),2,WHITE);

                              });*/
        //if (intersect.exists)
       // if (intersect2.type != AIR)
       if (terrain.blockExists(corner,true))
        {
            //ground += o.pos - intersect2.pos;
            ground += o.pos - corner;

        }
      /*  bool existed = terrain.blockExists(corner,true);
                std::cout << "CORNER: " << corner << " " << existed << "\n";
                Debug::addDeferRender([corner,existed](){
                                    DrawCircle3D(toVector3(corner),2,{},0,existed ? RED: BLUE);

                                  });*/

        prevCorner = corner;
    }
   /* Debug::addDeferRender([o,ground](){
                            DrawSphere(toVector3(o.pos),2,PURPLE);
                              DrawSphere(toVector3(o.pos + ground),2,WHITE);
                          });*/

    if (Vector2Equals(ground,{0,0}))
        {
            return o.rotation;
        }
    return atan2(ground.y,ground.x) + M_PI/2;
}

