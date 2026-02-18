#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <tuple>

#include <raylib.h>

#include "blocks.h"
#include "factory.h"
#include "checkFields.h"
#include "collideTriggers.h"
#include "shape.h"
#include "debug.h"
#include "colliders.h"
#include "game.h"
#include "sequencer.h"


struct Forces
{

    enum ForceSource
    {
        GRAVITY = 0,
        JUMP,
        MOVE,
        BOUNCE, //forces from when going out of bounds
        SWINGING,
        BOOSTING //from boosting, player specific
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


struct PhysicsBody
{
    size_t keyVal = 0; //a value that is sometimes used for object-object interactions
    bool dead = false;
    Orient orient;
    bool onGround = false;
    bool wasOnGround = false;
    bool tangible = true;
    bool freeFall = false; //freefall is true if we have not yet experienced gravity and stays true until we land

    bool followGravity = true; //true if object follows gravity and can not be inside terrain
    Forces forces;
    virtual Shape getShape() = 0;
    virtual void render() = 0;
    virtual void update(Terrain&) = 0;
    virtual Vector2 getPos() = 0;
    Orient getOrient();
    void setOrient(const Orient& orient);
    void setPos(const Vector2& pos);
    virtual Forces& getForces() = 0;
    virtual void onCollide(PhysicsBody& other)
    {

    }
    virtual void onRestore() //called when an object is restored
    {

    }
   virtual std::string serialize()
    {
        return EMPTY_SERIAL;
    }
    size_t getKeyVal();
    void setDead(bool val);
    bool getDead();

    void setTangible(bool val);
    virtual bool isTangible(); //can be collided with. If false, will not call "onCollide"
    virtual bool isDead();
    virtual bool isOnGround(Terrain& t) = 0;
    virtual void onAdd() //called upon being added to terrain, when all fields have been set and object is ready to go
    {

    }

    void applyForces(Terrain& t);
};



//renders a suggested button press over an object
void suggestButtonPress(const Shape& shape,std::string_view str);

template<typename Collider, typename Renderer, typename Descendant, typename... More>
struct Object : public PhysicsBody
{
    Collider collider;
    Renderer renderer;
    typedef GET_TYPE_WITH_collideWith<More...>::type CollideTrigger;
    [[no_unique_address]] std::conditional<
                            std::is_same<CollideTrigger,EMPTY_TYPE>::value,
                                EMPTY_TYPE,
                                CollideTrigger>::type collideTrigger;

    Color tint = WHITE;


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

    bool isOnGround(Terrain& t)
    {
        return collider.isOnGround(orient,t);
    }

    void onCollide(PhysicsBody& other)
    {
        //if there is a collide with function,
        if constexpr (has_interactWith<Descendant>)
        {
            if (&other == Globals::Game.getPlayer())
            {
                if (IsKeyPressed(KEY_E))
                {
                    static_cast<Descendant*>(this)->interactWith(other);
                }
                else
                {
                    Sequences::add({[this](int){suggestButtonPress(getShape(),"E");return true;}},false);
                }
            }
        }
        else if constexpr (has_collideWith<Descendant>)
        {
           static_cast<Descendant*>(this)->collideWith(other);
        }
        else
        {
            if constexpr(!std::is_same<CollideTrigger,EMPTY_TYPE>::value)
            {
                collideTrigger.collideWith(*this,other);
            }
        }
    }

    //calls the corresponding Factory<>::Base::serialize
    std::string serialize()
    {
        if constexpr(!std::is_same<Descendant,EMPTY_TYPE>::value)
        {
            return Factory<Descendant>::Base::serialize(*static_cast<Descendant*>(this));
        }
        else
        {
            return EMPTY_SERIAL;
        }
    }

    Vector2 getPos()
    {
        return orient.pos;
    }
    virtual Shape getShape()
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
        if (onGround && followGravity)
        {
            adjustAngle(t);
            stayOnGround(t);
        }
    }
    Forces& getForces()
    {
        return forces;
    }
protected:

    void stayOnGround(Terrain& terrain)
    {
        Vector2 norm = orient.getNormal();

        Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + norm*GetDimen(getShape()).y); //- normal*(collider.height)/2;
        Vector2 newPos = bruh - norm*(GetDimen(getShape()).y/2  - 1);
        setPos(newPos);
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

                botLeft = terrain.lineTerrainIntersect(botLeft,botLeft + normal );//.pos;
                botRight = terrain.lineTerrainIntersect(botRight,botRight + normal);//.pos;

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


};


#endif // OBJECTS_H_INCLUDED
