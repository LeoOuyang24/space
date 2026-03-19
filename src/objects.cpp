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

    setPos(getPos() + forces.getTotalForce());

    Vector2 grav = forces.getForce(Forces::GRAVITY);
    forces.addFriction(onGround ? 0.5 : .99);

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
    float mult = 1;//(onGround && orient.rotation < -M_PI/4) ? abs(sin(orient.rotation)) : 1;
    forces.addForce(Vector2(0,GlobalTerrain::GRAVITY_CONSTANT)*(mult),Forces::GRAVITY);
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
            if (!Vector2Equals(moveVec,{}) && !freeFall) //if there moving, subtract that component from gravity (prevents gravity from pulling player against intended motion)
                {
                    grav -= moveVec*Vector2DotProduct(grav,moveVec)/Vector2DotProduct(moveVec,moveVec)*.9f;
                }

            Vector2 norm = Vector2Normalize(grav);
            //std::cout << Vector2Length(grav) << "\n";
            forces.addForce(norm*GlobalTerrain::GRAVITY_CONSTANT,Forces::GRAVITY);
            //forces.addForce(grav,Forces::GRAVITY);
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

    /*Debug::addDeferRender([botLeft,botRight](){

                          DrawCircle3D({botLeft.x,botLeft.y,Globals::Game.getCurrentZ()},10,{},0,BLUE);
                          DrawCircle3D({botRight.x,botRight.y,Globals::Game.getCurrentZ()},10,{},0,BLUE);

                          });*/

    float newAngle = trunc(atan2(botRight.y - botLeft.y, botRight.x - botLeft.x),3);

    if (trunc(abs(newAngle - orient.rotation),2) > .001)
    {
        orient.rotation = newAngle;
    }
}

void PhysicsBody::stayOnGround(Terrain& terrain)
{

   Vector2 norm = orient.getNormal();

    Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + norm*GetDimen(getShape()).y,true); //- normal*(collider.height)/2;
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
