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

void PhysicsBody::applyForces(Terrain& terrain)
{
    int searchRad = freeFall ? 220 : 120;

    if (!onGround && followGravity)
    {
        int divide = 100;
        const int landingDivide = 11;
        int upTo =  divide;// freeFall ? divide : landingDivide;
        Vector2 grav = {0,0};
        int count = 0;
        for (int i = 0; i < upTo; i ++)
        {
            float angle =  2*M_PI/divide*i + (M_PI/2)-M_PI/(divide)*(landingDivide-1) + orient.rotation;
            auto pos = terrain.lineBlockIntersect(orient.pos, orient.pos + Vector2(cos(angle),sin(angle))*searchRad,false);
            Debug::addDeferRender([pos,&terrain](){

                                  DrawCircle3D(Vector3(pos.x,pos.y,Globals::Game.getCurrentZ()),10,{0,1,0},0,terrain.isBlockType(pos,ANTI,true) ? BLUE : RED);

                                  });
            if (terrain.blockExists(pos,true) && !Vector2Equals(pos,orient.pos))
            {
                Vector2 force = Vector2Normalize(pos - orient.pos)/pow(Vector2Length(pos - orient.pos),1);
                if (terrain.isBlockType(pos,ANTI,true))
                {
                    force *= -1;
                }
                if (terrain.isBlockType(pos,LAVA,true))
                {
                    force *= 0.1;
                }
                grav +=  force;

                count ++;
            }
        }
        if (count > 0)
        {
            Vector2 norm = Vector2Normalize(grav);
            //std::cout << Vector2Length(grav) << "\n";
            forces.addForce(norm*0.25,Forces::GRAVITY);
           // forces.addForce(grav*23/count,Forces::GRAVITY);
        }
        else
        {
            //freeFall = true;
        }
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
    forces.addFriction(onGround ? 0.5 : .99);

    //forces.addForce({0,.5},Forces::GRAVITY);

    wasOnGround = onGround;
    onGround = isOnGround(terrain);
    freeFall = freeFall && !onGround;

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
