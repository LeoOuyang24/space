#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <tuple>
#include <array>

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

    enum ForceSource : uint8_t
    {
        GRAVITY = 0,
        JUMP,
        MOVE,
        ENEMY, //misc for any forces applied by enemies
        BOUNCE, //forces from when going out of bounds
        SWINGING,
        BOOSTING, //from boosting, player specific
        FORCE_SOURCE_SIZE //number of force sources, should always be the last member
    };

    std::array<Vector2,FORCE_SOURCE_SIZE> forces; //mapping force source to a force
    Vector2 totalForce = {0,0}; //total forces

    void setForce(const Vector2& force, Forces::ForceSource source);
    void addForce( Vector2 force, ForceSource source);
    void addFriction(const Vector2& friction);
    void addFriction(float friction);
    void addFriction(float friction, ForceSource source);
    Vector2 getTotalForce();

    Vector2 getForce(ForceSource source) const //read-only, get force from a source
    {
        return (source >= forces.size()) ? Vector2{0,0} : forces[source];
    }
};


struct PhysicsBody
{
    Orient orient;
    size_t keyVal = 0; //a value that is sometimes used for object-object interactions

    Forces forces;
    virtual Shape getShape() = 0;
    virtual void render() = 0;
    virtual void update(Terrain&) = 0;
    virtual Vector2 getPos() = 0;
    Orient getOrient() const;
    void setOrient(const Orient& orient);
    void setPos(const Vector2& pos);
    Forces& getForces();
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
    virtual bool isUnderwater(Terrain& t);
    virtual void onAdd() //called upon being added to terrain, when all fields have been set and object is ready to go
    {

    }

    void applyForces(Terrain& t);

    make_getter(followGravity,bool);
    bool followGravity = true; //true if object follows gravity and can not be inside terrain

protected:
    void downGravity(Terrain&);
    void planetGravity(Terrain&);
    void pointGravity(Terrain&);

    void adjustAngle(Terrain& terrain);
    void stayOnGround(Terrain& terrain);

    bool dead = false;
    bool onGround = false;
    bool wasOnGround = false;
    bool tangible = true;
    bool freeFall = false; //freefall is true if we have not yet experienced gravity and stays true until we land
    float gravRadius = 130;
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
                                CollideTrigger>::type collideTrigger; //if no collideTrigger, then try not to take up any space with this field

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
        return collider.isOnGround(*this,t);
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
        if (followGravity)
        {
            applyForces(t);
            if (onGround)
            {
                adjustAngle(t);
            }
            stayOnGround(t);
        }
    }
    Forces& getForces()
    {
        return forces;
    }
protected:

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
                PhysicsBody::adjustAngle(terrain);
            }
        }
    }

};


#endif // OBJECTS_H_INCLUDED
