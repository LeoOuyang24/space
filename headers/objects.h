#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <tuple>

#include <raylib.h>

#include "blocks.h"
#include "terrain.h"
#include "factory.h"
#include "checkFields.h"
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
    void setPos(const Vector2& pos);
    virtual void onCollide(PhysicsBody& other)
    {

    }
   virtual std::string serialize()
    {
        return EMPTY_SERIAL;
    }
    void setDead(bool val);
    bool getDead();
    virtual bool isDead();
};


struct Forces
{

    enum ForceSource
    {
        GRAVITY = 0,
        JUMP,
        MOVE,
        BOUNCE //forces from when going out of bounds
    };

    std::unordered_map<ForceSource,Vector2> forces; //mapping force source to a force
    Vector2 totalForce = {0,0}; //total forces

    void setForce(const Vector2& force, Forces::ForceSource source);
    void addForce( Vector2 force, ForceSource source);
    void addFriction(const Vector2& friction);
    void addFriction(float friction);
    void addFriction(float friction, ForceSource source);
    Vector2 getTotalForce();

    Vector2 getForce(ForceSource source) //read-only, get force from a source
    {
        return (forces.find(source) == forces.end()) ? Vector2{0,0} : forces[source];
    }
};
template<typename Collider, typename Renderer, typename Descendant, typename... More>
struct Object : public PhysicsBody
{
    Collider collider;
    Renderer renderer;

    Color tint = WHITE;
    bool onGround = false;
    bool wasOnGround = false;
    bool freeFall = false; //freefall is true if we have not yet experienced gravity and stays true until we land

    bool followGravity = true;
    Forces forces;

    Vector2 nearest = {-1,-1};

    template<typename... CollArgs, typename... RenderArgs>
    Object(const Orient& pos, std::tuple<CollArgs...> colliderArgs, std::tuple<RenderArgs...> renderArgs) : tint(WHITE),
                                                                        collider(std::make_from_tuple<Collider>(colliderArgs)),
                                                                        renderer(std::make_from_tuple<Renderer>(renderArgs))
    {
        orient = pos;
    }

    template<typename... CollArgs, typename... RenderArgs, typename... Args>
    Object(const Orient& pos, std::tuple<CollArgs...> colliderArgs, std::tuple<RenderArgs...> renderArgs, Args... conArgs) : tint(WHITE),
                                                                        collider(std::make_from_tuple<Collider>(colliderArgs)),
                                                                        renderer(std::make_from_tuple<Renderer>(renderArgs))
    {
        orient = pos;
    }

    Object()
    {

    }

    void onCollide(PhysicsBody& other)
    {
        //if there is a collide with function,
        if constexpr (has_interactWith<Descendant>)
        {
            if (IsKeyPressed(KEY_E) && &other == Globals::Game.getPlayer())
            {
                static_cast<Descendant*>(this)->interactWith(other);
            }
        }
        else if constexpr (has_collideWith<Descendant>)
        {
           static_cast<Descendant*>(this)->collideWith(other);
        }
        else
        {
            using collideTrigger = GET_TYPE_WITH_collideWith<More...>::type;
            if constexpr(!std::is_same<collideTrigger,EMPTY_TYPE>::value)
            {
                collideTrigger::collideWith(*this,other);
            }
        }
    }

    //calls the corresponding Factory<>::Base::serialize
    //we do this by figuring out which of our parameters is a descendant of PhysicsBody (because it has a "dead" field)
    std::string serialize()
    {
        if constexpr(!std::is_same<Descendant,EMPTY_TYPE>::value)
        {
            return Factory<Descendant>::Base::serialize(*static_cast<Descendant*>(this));
        }
        else
        {
            return "";
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
        Vector2 dimen = GetDimen(getShape());
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
        Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + orient.getNormal()*GetDimen(getShape()).y).pos; //- normal*(collider.height)/2;
        Vector2 newPos = bruh - orient.getNormal()*(GetDimen(getShape()).y/2  - 1);

        orient.pos = newPos;
    }

    void adjustAngle(Terrain& terrain)
    {
        //if on ground, adjust our angle based on the angle of the terrain
        if (onGround || !freeFall)
        {
            if (!wasOnGround && onGround) //just landed
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

                botLeft = terrain.lineTerrainIntersect(botLeft,botLeft + normal ).pos;
                botRight = terrain.lineTerrainIntersect(botRight,botRight + normal).pos;

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
        }
    }
    void applyForces(Terrain& terrain)
    {
        int searchRad = freeFall ? 400 : 200;


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
            int divide = 32;
            const int landingDivide = 5;
            int upTo = freeFall ? divide : landingDivide;
            Vector2 grav = {0,0};
            int count = 0;
            for (int i = 0; i < upTo; i ++)
            {
                float angle =  2*M_PI/divide*i + M_PI/2-M_PI/(divide)*(landingDivide-1) + orient.rotation;
                auto pos = terrain.lineBlockIntersect(orient.pos, orient.pos + Vector2(cos(angle),sin(angle))*searchRad,false);
                /*Debug::addDeferRender([pos](){

                                      DrawCircle3D(Vector3(pos.pos.x,pos.pos.y,Globals::Game.getCurrentZ()),10,{0,1,0},0,pos.type == ANTI ? BLUE : RED);

                                      });*/
                if (pos.type != AIR)
                {
                    Vector2 force = Vector2Normalize(pos.pos - orient.pos)/pow(Vector2Length(pos.pos - orient.pos),1);
                    if (pos.type == ANTI)
                    {
                        force *= -1;
                    }
                    else if (pos.type == LAVA)
                    {
                        force *= .5;
                    }
                    grav +=  force;

                    count ++;
                }
            }
            if (count > 0)
            {
                forces.addForce(grav*20/count,Forces::GRAVITY);
            }
            else
            {
                freeFall = true;
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

        wasOnGround = onGround;
        onGround = collider.isOnGround(orient,terrain);

    }

};


#endif // OBJECTS_H_INCLUDED
