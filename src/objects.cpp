#include <iostream>

#include "../headers/objects.h"
#include "../headers/raylib_helper.h"

void PhysicsBody::setPos(const Vector2& pos)
{
    orient.pos = pos;
}

void PhysicsBody::setPos(const Vector3& pos)
{
    orient.pos = {pos.x,pos.y};
    orient.setZ(pos.z);
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

void PhysicsBody::setLayer(LayerType layer)
{
    this->orient.layer = layer;
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
    Vector2 grav = {};
    switch (Globals::Game.terrain.get_gravityMode())
    {
    case GlobalTerrain::GravityMode::PLANET:
        grav = planetGravity(terrain);
        break;
    case GlobalTerrain::GravityMode::DOWN:
        grav = downGravity(terrain);
        break;
    case GlobalTerrain::GravityMode::POINT:
        grav = pointGravity(terrain);
        break;
    default:
        break;
    }

    forces.addForce(grav,Forces::GRAVITY);

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

    set_wasOnGround(onGround);
    set_onGround(isOnGround(terrain));

    forces.addFriction(onGround ? 0.5 : .99);

    //last time we were contacted by gravity, rn if we have gravity or if on ground
    if (!(Vector2Equals(grav,{})) || (get_onGround()) )
    {
        lastGravityContact = Globals::getCurrentFrame();
    }

}

Vector2 PhysicsBody::downGravity(Terrain& t)
{
    return Vector2(0,GlobalTerrain::GRAVITY_CONSTANT);
}

Vector2 PhysicsBody::planetGravity(Terrain& terrain)
{
    int searchRad = gravRadius;

    if (!onGround && followGravity)
    {
        int divide = 20;
        const int landingDivide = 3;
        int upTo = divide;//freeFall ? divide : landingDivide;
        Vector2 grav = {0,0};
        int count = 0;
       // Vector2 down = {};
       
       Shape shape = getShape();
       for (int i = 0; i < getShapePoints(shape.type); i ++)
       {
            Vector2 point = getIthShapePoint(shape,i);

            grav += Vector2Normalize(terrain.field.getFieldAtPos(point));
       }

        //if (count > 0)
        {
            terrainAngle += Vector2Normalize(grav);

            Vector2 norm = Vector2LengthSqr(grav) > GlobalTerrain::GRAVITY_CONSTANT*GlobalTerrain::GRAVITY_CONSTANT ? 
                                Vector2Normalize(grav): 
                                grav;


            Vector2 moveVec = forces.getForce(Forces::MOVE);
            if (!Vector2Equals(moveVec,{}))
            {
                //norm -= moveVec*.01f*Vector2DotProduct(norm,moveVec)/Vector2DotProduct(moveVec,moveVec);
            }

            return norm*GlobalTerrain::GRAVITY_CONSTANT;
            //forces.addForce(grav*20,Forces::GRAVITY);
        }
    }
    return {};
}

Vector2 PhysicsBody::pointGravity(Terrain&)
{
    return Vector2Normalize(Vector2(Terrain::MAX_TERRAIN_SIZE,
                                             Terrain::MAX_TERRAIN_SIZE)*0.5f - getPos())*GlobalTerrain::GRAVITY_CONSTANT;
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
