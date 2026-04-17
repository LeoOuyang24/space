#include <iostream>

#include "../headers/objects.h"
#include "../headers/raylib_helper.h"

void PhysicsBody::setPos(const Vector2& pos)
{
    orient.pos = pos;
}

Forces& PhysicsBody::getForces()
{
    return forces;
}

Orient PhysicsBody::getOrient() const
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

bool PhysicsBody::isUnderwater(Terrain& t)
{
    return t.isBlockType(getPos(),WATER,true);
}

bool PhysicsBody::isTangible()
{
    return tangible;
}

void PhysicsBody::setTangible(bool val)
{
    tangible = val;
}

size_t PhysicsBody::getKeyVal()
{
    return keyVal;
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
    forces[source] += force;

    totalForce += force;
}

void Forces::addFriction(const Vector2& friction)
{
    for (auto& source : forces)
    {
        source *= friction;
    }
    totalForce *= friction;
}

void Forces::addFriction(float friction)
{
    for (auto& source : forces)
    {
        source *= friction;
    }
    totalForce *= friction;
}

void Forces::addFriction(float friction, Forces::ForceSource source)
{
    totalForce -= forces[source];
    forces[source] *= friction;
    totalForce += forces[source];
}

Vector2 Forces::getTotalForce()
{
    return Vector2LengthSqr(totalForce) > 400 ? Vector2Normalize(totalForce)*20 : totalForce;
}

void PhysicsBody::applyForces(Terrain& terrain)
{
    switch (Globals::Game.terrain.get_gravityMode())
    {
    case GlobalTerrain::GravityMode::PLANET:
        planetGravity(terrain);
        break;
    case GlobalTerrain::GravityMode::DOWN:
        downGravity(terrain);
        break;
    case GlobalTerrain::GravityMode::POINT:
        pointGravity(terrain);
        break;
    default:
        break;
    }

    if (orient.pos.x >= Terrain::MAX_TERRAIN_SIZE || orient.pos.x <= 0)
    {
        forces.addFriction({-1,1});
        forces.addForce( Vector2{(orient.pos.x <= 0 ) * 2 - 1,0},Forces::BOUNCE);
    }
    else if (orient.pos.y >= Terrain::MAX_TERRAIN_SIZE || orient.pos.y <= 0)
    {
        forces.addFriction({1,-1});
        forces.addForce( Vector2{0,(orient.pos.y <= 0 ) * 2 - 1},Forces::BOUNCE);
    }
    Vector2 total = forces.getTotalForce();
    setPos(getPos() + total);

    if (Globals::Game.terrain.get_gravityMode() == GlobalTerrain::DOWN)
    {
        Vector2 dimen = GetDimen(getShape());
        Vector2 left = orient.pos + Vector2(-dimen.x/2 - 1,2);
        Vector2 right = orient.pos + Vector2(dimen.x/2 + 1,2);
        Vector2 up = orient.pos + Vector2(0,-dimen.y/2 - Block::BLOCK_DIMEN);

        Color lColor = WHITE;
        Color rColor = WHITE;

        Vector2 lpos = terrain.lineBlockIntersect(orient.pos,left);
        Vector2 rpos = terrain.lineBlockIntersect(orient.pos,right);
        Vector2 upos = terrain.lineBlockIntersect(orient.pos,up);

        if (!Vector2Equals(left,lpos))
        {
            orient.pos.x = lpos.x + dimen.x/2 + Block::BLOCK_DIMEN;
            lColor = BLUE;
        }
        if (!Vector2Equals(right,rpos))
        {
            orient.pos.x = rpos.x - dimen.x/2 - Block::BLOCK_DIMEN;
            rColor = BLUE;
        }
        if (!Vector2Equals(up,upos))
        {
            orient.pos.y = upos.y + dimen.y/2 + Block::BLOCK_DIMEN;
        }
        Debug::addDeferRender([left,right,lColor,rColor,lpos,rpos](){

            DrawCircle3D(toVector3(left),2,{},0,PURPLE);
            DrawCircle3D(toVector3(right),2,{},0,PURPLE);

            DrawSphere(toVector3(lpos),2,lColor);
            DrawSphere(toVector3(rpos),2,rColor);

        });
    }

    /*Vector2 horiz = {forces.getTotalForce().x,0};
    Vector2 vert = {0,forces.getTotalForce().y};

    Shape horizShape = getShape();
    horizShape.orient.pos += horiz;
    Shape vertShape = getShape();
    vertShape.orient.pos += vert;

    if (terrain.blockExists(getPos() + horiz  + Vector2(GetDimen(getShape()).x*(horiz.x < 0 ? -1 : 1),0)))
    {
        horiz = {};
    }
    if (terrain.blockExists(vertShape))
    {
     //   vert = {};
    }*/

    forces.addFriction(onGround ? 0.5 : .99);
    //forces.setForce(grav,Forces::GRAVITY);

    /*if (Globals::Game.terrain.get_gravityMode() == GlobalTerrain::DOWN && onGround)
    {
        forces.setForce(grav*0.9,Forces::GRAVITY);
    }*/

    wasOnGround = onGround;
    onGround = isOnGround(terrain);
    freeFall = freeFall && !onGround;

}

void PhysicsBody::downGravity(Terrain& t)
{
    //float mult = (onGround && abs(orient.rotation) < M_PI/4) ? abs(sin(orient.rotation)) : 1;
    forces.addForce(Vector2(0,GlobalTerrain::GRAVITY_CONSTANT),Forces::GRAVITY);
   //forces.setForce(Vector2(0,mult*5),Forces::GRAVITY);
}

void PhysicsBody::planetGravity(Terrain& terrain)
{
    int searchRad = freeFall ? 220*2 : 220;

    if (!onGround && followGravity)
    {
        int divide = 25;
        const int landingDivide = 3;
        int upTo = divide;//freeFall ? divide : landingDivide;
        Vector2 grav = {0,0};
        int count = 0;
       // Vector2 down = {};
        for (int i = 0; i < upTo; i ++)
        {
            float angle =  2*M_PI/divide*i + (M_PI/2)-M_PI/(divide)*(landingDivide-1) + orient.rotation;
            Vector2 endpoint = orient.pos + Vector2(cos(angle),sin(angle))*searchRad;
            Vector2 pos = terrain.lineBlockIntersect(orient.pos, endpoint,false);
                   Debug::addDeferRender([pos,&terrain,landingDivide,i](){

                                  DrawCircle3D(Vector3(pos.x,pos.y,Globals::Game.getCurrentZ()),10,{0,1,0},0,i < landingDivide ? BLUE : RED);

                                  });
            if (!Vector2Equals(pos,endpoint) && !Vector2Equals(pos,orient.pos))
            {

                Vector2 force = Vector2Normalize(pos - orient.pos)/pow(Vector2Length(pos - orient.pos),2);
                if (terrain.isBlockType(pos,ANTI,true))
                {
                    force *= -1;
                }
                else if (terrain.isBlockType(pos,LAVA,true))
                {
                    force *= 0.1;
                }
                else if (terrain.isBlockType(pos,WATER,true))
                {
                    force *= 0.1;
                }
                if (i < landingDivide)
                {
                   // force *= 1.1;
                }
                grav +=  force;

                count ++;
            }
        }
        if (count > 0)
        {
            Vector2 moveVec = forces.getForce(Forces::MOVE);
            terrainAngle += Vector2Normalize(grav);
            if (!Vector2Equals(moveVec,{}) && !freeFall) //if there moving, subtract that component from gravity (prevents gravity from pulling player against intended motion)
                {
                 //   grav -= moveVec*Vector2DotProduct(grav,moveVec)/Vector2DotProduct(moveVec,moveVec)*.89f;
                }

            Vector2 norm = Vector2Normalize(grav);

            //std::cout << Vector2Length(grav) << "\n";
            forces.addForce(norm*GlobalTerrain::GRAVITY_CONSTANT,Forces::GRAVITY);
            //forces.addForce(grav*20,Forces::GRAVITY);
        }
        else if (count == 0)
        {
            freeFall = true;
        }
    }
}

void PhysicsBody::pointGravity(Terrain&)
{
    forces.addForce(
                    Vector2Normalize(Vector2(Terrain::MAX_TERRAIN_SIZE,
                                             Terrain::MAX_TERRAIN_SIZE)*0.5f - getPos())*GlobalTerrain::GRAVITY_CONSTANT,
                    Forces::GRAVITY);
}

void PhysicsBody::adjustAngle(Terrain& terrain)
{
    Vector2 dimen = GetDimen(getShape());
    Vector2 botLeft = orient.pos +Vector2Rotate(Vector2(-dimen.x/2,dimen.y/2),orient.rotation);
    Vector2 botRight = orient.pos + Vector2Rotate(Vector2(dimen.x/2,dimen.y/2),orient.rotation);

    Vector2 normal = orient.getNormal();

    botLeft = terrain.lineTerrainIntersect(botLeft,botLeft + normal,false);//.pos;
    botRight = terrain.lineTerrainIntersect(botRight,botRight + normal,false);//.pos;

   /* Debug::addDeferRender([botLeft,botRight](){

                          DrawCircle3D({botLeft.x,botLeft.y,Globals::Game.getCurrentZ()},2,{},0,BLUE);
                          DrawCircle3D({botRight.x,botRight.y,Globals::Game.getCurrentZ()},2,{},0,BLUE);
                          DrawLine3D(toVector3(botLeft),toVector3(botRight),BLUE,1);
                          }); */

    float newAngle = trunc(atan2(botRight.y - botLeft.y, botRight.x - botLeft.x),3);

    if (trunc(abs(newAngle - orient.rotation),2) > .001)
    {

        orient.rotation = newAngle;
        //setPos((botLeft + botRight)*0.5 - Vector2Rotate(Vector2(0,dimen.y/2),orient.rotation));
    }
}

void PhysicsBody::stayOnGround(Terrain& terrain)
{

   Vector2 norm = orient.getNormal();

    Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + norm*GetDimen(getShape()).y,false); //- normal*(collider.height)/2;
    
    Vector2 newPos = bruh - Vector2Normalize(norm)*(GetDimen(getShape()).y/2  - 1);


    setPos(newPos);

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
