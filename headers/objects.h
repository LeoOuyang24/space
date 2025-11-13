#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <tuple>

#include <raylib.h>

#include "blocks.h"
#include "terrain.h"
#include "checkFunctions.h"
#include "collideTriggers.h"
#include "shape.h"
#include "debug.h"
#include "colliders.h"
#include "game.h"

struct PhysicsBody
{
    bool dead = false;
    Orient orient;

    virtual Shape getShape() = 0;
    virtual void render() = 0;
    virtual void update(Terrain&) = 0;
    virtual Vector2 getPos() = 0;
    virtual void collideWith(PhysicsBody& other)
    {

    }
    void setDead(bool val);
    virtual bool isDead();
};


struct Forces
{

    enum ForceSource
    {
        GRAVITY = 0,
        JUMP,
        GRAPPLE,
        MOVE
    };

    std::unordered_map<ForceSource,Vector2> forces; //mapping force source to a force
    Vector2 totalForce = {0,0}; //total forces

    void addForce( Vector2 force, ForceSource source);
    void addFriction(float friction);
    void addFriction(float friction, ForceSource source);
    Vector2 getTotalForce();

    Vector2 operator[](ForceSource source) //read-only, get force from a source
    {
        return forces[source];
    }
};
template<typename Collider, typename Renderer, typename... More>
struct Object : public PhysicsBody
{

    Collider collider;
    Renderer renderer;

    GET_TYPE_WITH_collideWith<More...>::type collideTrigger;

    Color tint;
    bool onGround = false;
    bool wasOnGround = false;
    bool freeFall = false; //freefall is true if we have not yet experienced gravity and stays true until we land

    bool followGravity = true;
    Forces forces;

    Vector2 nearest = {-1,-1};
    bool balls = true;

    template<typename... CollArgs, typename... RenderArgs>
    Object(const Orient& pos, std::tuple<CollArgs...> colliderArgs, std::tuple<RenderArgs...> renderArgs, std::tuple<More...> tup) : tint(WHITE),
                                                                        collider(std::make_from_tuple<Collider>(colliderArgs)),
                                                                        renderer(std::make_from_tuple<Renderer>(renderArgs)),
                                                                        collideTrigger(std::move(std::get<decltype(collideTrigger)>(tup)))
    {
        orient = pos;
    }
    template<typename... Args>
    Object(const Orient& o, const Color& color,  Args... args) : tint(color), collider(args...)
    {
        orient = o;
    }

    template<typename... CollArgs, typename... RenderArgs, typename... Args>
    Object(const Orient& pos, std::tuple<CollArgs...> colliderArgs, std::tuple<RenderArgs...> renderArgs, Args... conArgs) : tint(WHITE),
                                                                        collider(std::make_from_tuple<Collider>(colliderArgs)),
                                                                        renderer(std::make_from_tuple<Renderer>(renderArgs)),
                                                                        collideTrigger(createFromArgs<decltype(collideTrigger)>(conArgs...))
    {
        orient = pos;
    }

    void collideWith(PhysicsBody& other)
    {
        //if there is a collide with function,
        if constexpr (has_collideWith<decltype(collideTrigger)>)
        {
            collideTrigger.collideWith(*this,other);
        }
    }

    Vector2 getPos()
    {
        return orient.pos;
    }
    Shape getShape()
    {
        return {collider.getShapeType(),orient,collider.getCollider()};
    }
    virtual void render()
    {
        renderer.render(getShape(),tint);
    }
    virtual void update(Terrain& t)
    {
        applyForces(t);
        if (onGround)
        {
            adjustAngle(t);
            stayOnGround(t);
        }
    }
protected:
    void stayOnGround(Terrain& terrain)
    {
        Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + orient.getNormal()*GetDimen(getShape()).y/2).pos; //- normal*(collider.height)/2;
        Vector2 newPos = bruh - orient.getNormal()*(GetDimen(getShape()).y/2  - 1);

        orient.pos = newPos;
    }

    void adjustAngle(Terrain& terrain)
    {
        //if on ground, adjust our angle based on the angle of the terrain
        if (onGround)
        {
            if (!wasOnGround) //just landed
            {
                orient.rotation = collider.getLandingAngle(orient,terrain);
                freeFall = false;
            }
            else //otherwise adjust angle based on terrain angle
            {
                Vector2 dimen = GetDimen(getShape());
                Vector2 botLeft = orient.pos +Vector2Rotate(Vector2(-dimen.x/2,dimen.y/2),orient.rotation);
                Vector2 botRight = orient.pos + Vector2Rotate(Vector2(dimen.x/2,dimen.y/2),orient.rotation);

                Vector2 normal = orient.getNormal();

                botLeft = terrain.lineTerrainIntersect(botLeft - normal, botLeft).pos;
                botRight = terrain.lineTerrainIntersect(botRight - normal,botRight).pos;

                float newAngle = trunc(atan2(botRight.y - botLeft.y, botRight.x - botLeft.x),3);

                if (trunc(abs(newAngle - orient.rotation),2) > .001)
                {
                    orient.rotation = newAngle;
                }
            }
        }
    }
    void applyForces(Terrain& terrain)
    {
        int searchRad = 100;


        if (!onGround && followGravity)
        {


                /*terrain.forEachPosSample([this,&terrain](const Vector2& pos, int size){
                    if (terrain.blockExists(pos))
                    {
                        float mag = 0.001/pow(std::max(1.0f,Vector2Length(pos -orient.pos)),2);
                        this->forces.addForce((pos - orient.pos)*mag,Forces::GRAVITY);
                    }
                    },orient.pos,searchRad);*/
             /*Debug::addDeferRender([this](){

                                   DrawLine3D({orient.pos.x,orient.pos.y,Globals::Game.getCurrentZ()},{nearest.x,nearest.y,Globals::Game.getCurrentZ()},PURPLE);

                                   });*/
            //forces.addForce(orient.getNormal()*0.1f,Forces::GRAVITY);
            int divide = 16;
            int upTo = freeFall ? divide : 3;
            Vector2 grav = {0,0};
            int count = 0;
            for (int i = 0; i < upTo; i ++)
            {
                float angle =  2*M_PI/divide*i + M_PI/2-2*M_PI/divide + orient.rotation;
                auto pos = terrain.lineBlockIntersect(orient.pos, orient.pos + Vector2(cos(angle),sin(angle))*150);
               /* Debug::addDeferRender([pos](){

                                      DrawCircle3D(Vector3(pos.pos.x,pos.pos.y,Globals::Game.getCurrentZ()),10,{0,1,0},0,BLUE);

                                      });*/
                if (pos.exists)
                {
                    grav +=  Vector2Normalize(pos.pos - orient.pos)*(1.0/pow(std::max(1.0f,Vector2Length(pos.pos -orient.pos)),2));
                    count ++;
                }
            }
            if (count >0)
            {
                forces.addForce(grav*150/count,Forces::GRAVITY);
            }
            else
            {
                freeFall = true;
            }
        }

        orient.pos += forces.getTotalForce();
        //force = force*(onGround ? 0.5 : .99);
        forces.addFriction(onGround ? 0.5 : .99);
        wasOnGround = onGround;
        onGround = collider.isOnGround(orient,terrain);

    }

};


#endif // OBJECTS_H_INCLUDED
