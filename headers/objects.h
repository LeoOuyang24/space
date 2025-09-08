#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <tuple>

#include <raylib.h>

#include "blocks.h"
#include "shape.h"

struct Orient
{
    Vector2 pos = {0,0};
    float rotation = 0; // IN RADIANS

    inline Vector2 getFacing() const
    {
        return Vector2Rotate(Vector2(1,0),rotation);
    }
    inline Vector2 getNormal() const
    {
        return Vector2Rotate(Vector2(0,1),rotation);
    }
};

//life is too short to have to write the getShapeType function for each collider
#define GET_SHAPE_TYPE(type) ShapeType getShapeType() {return type;}

struct CircleCollider
{
    int radius = 0;


    bool isOnGround(const Orient& orient, GlobalTerrain& t);

    GET_SHAPE_TYPE(ShapeType::CIRCLE);
};

struct RectCollider
{
    //orient.pos is considered to be the center of the rectangle
    float width = 0, height = 0;

    bool isOnGround(const Orient& orient, GlobalTerrain& t);
    Rectangle getRect(const Orient& orient);

    GET_SHAPE_TYPE(ShapeType::RECT);
};

struct PhysicsBody
{
    virtual Shape getShape() = 0;
    virtual void render() = 0;
    virtual void update(GlobalTerrain&) = 0;
    virtual void addForce( const Vector2& force) = 0;
    virtual Vector2 getPos() = 0;
};


struct Forces
{

    enum ForceSource
    {
        GRAVITY = 0,
        JUMP,
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


template<typename Collider, typename Renderer>
struct Object : public PhysicsBody
{
    Collider collider;
    Renderer renderer;
    Color tint;
    Orient orient;
    bool onGround = false;

    Vector2 force = {0,0};

    Forces forces;


    template<typename... CollArgs, typename... RenderArgs>
    Object(const Vector2& pos, std::tuple<CollArgs...> colliderArgs, std::tuple<RenderArgs...> renderArgs) : orient({pos}),tint(WHITE),
                                                                        collider(std::make_from_tuple<Collider>(colliderArgs)),
                                                                        renderer(std::make_from_tuple<Renderer>(renderArgs))
    {

    }

    template<typename... Args>
    Object(const Orient& o, const Color& color,  Args... args) : orient(o), tint(color), collider(args...)
    {

    }

    Vector2 getPos()
    {
        return orient.pos;
    }
    Shape getShape()
    {
        return {collider.getShapeType(),orient,&collider};
    }

    void addForce(const Vector2& force_)
    {
        force += force_;
    }

    void render()
    {
        //collider.render(orient);
        renderer.render(getShape(),tint);
    }
    void update(GlobalTerrain& terrain)
    {
        int searchRad = 100;


        if (!onGround)
        {

                terrain.forEachPos([this,&terrain](const Vector2& pos){
                    if (terrain.blockExists(pos))
                    {
                        float mag = 1.0/pow(std::max(1.0f,Vector2Length(pos - orient.pos)),3);
                        this->forces.addForce((pos - orient.pos)*mag,Forces::GRAVITY);
                    }
                           },orient.pos,searchRad);

           // orient.rotation = (atan2(forces.getTotalForce().y,forces.getTotalForce().x));

        }
        /*Vector2 normal = Vector2Normalize(force);
        float mag = Vector2Length(force);
        if (mag >= 1000.0f)
        {
            force = normal*1000.0f;
        }*/

       //

        orient.pos += forces.getTotalForce();
        //force = force*(onGround ? 0.5 : .99);
        forces.addFriction(onGround ? 0.5 : .99);
        onGround = collider.isOnGround(orient,terrain);

    }
};


#endif // OBJECTS_H_INCLUDED
