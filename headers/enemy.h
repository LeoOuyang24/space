#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "objects.h"
#include "factory.h"
#include "render.h"
#include "conversions.h"

struct GrapplePoint : public Object<CircleCollider,TextureRenderer,GrapplePoint>
{
    GrapplePoint();
    void update(Terrain& terrain);
};

template<>
struct Factory<GrapplePoint>
{
    static constexpr std::string ObjectName = "grapple_point";
    using Base = FactoryBase<GrapplePoint,
                                access<GrapplePoint,&GrapplePoint::orient,&Orient::pos>
                                >;
};

struct MoveFunc //represents a function that determines how the terrain moves
{
    std::function<Vector2(const Orient& o,const Vector2& starting,float)> moveFunc;
    std::function<std::string()> toString;
    uint16_t duration = 1; //how many frames it takes to complete a full cycle
    uint16_t frame = 0; //used to calculate position, increments each time calcNewPos is called
    Vector2 operator()(const Orient& o,const Vector2& starting)
    {
        frame = (frame+1)%duration;
        if (moveFunc)
        {
            /*Debug::addDeferRender([this,starting,o](){
            
                DrawText3D(GetFontDefault(),
                            std::to_string(frame).c_str(),
                            toVector3(moveFunc(o,starting,static_cast<float>(frame)/duration)),
                        10,10,0,false,WHITE);

            });*/

            return moveFunc(o,starting,static_cast<float>(frame)/duration);
        }
        else
        {
            return starting;
        }
    }
};

struct LaserBeamEnemy : public Object<RectCollider,TextureRenderer,LaserBeamEnemy>
{
    enum RotateFunc : int
    {
        CONSTANT = 0,
        SINE
    };
    float startingRot = 0; //starting rotation in degrees
    float rotSpeed = 0;
    float arc = 0; //arc in degrees
    float beamLength = 100;
    RotateFunc func = SINE;
    MoveFunc movement;

    LaserBeamEnemy()
    {
        followGravity = false;

        collider.width = 100;
        collider.height = 100;
        renderer.sprite = Globals::Game.Sprites.getSprite("laser_beamer.png");
    }

    void render();
    void update(Terrain& t);
    Shape getShape() const; //for collision detection purposes, only the laser beam's shape matters
    void collideWith(PhysicsBody& other);
};

template<>
struct Factory<LaserBeamEnemy>
{
    static constexpr std::string ObjectName = "laser_beamer";
    using Base = FactoryBase<LaserBeamEnemy,
                                access<LaserBeamEnemy,&LaserBeamEnemy::orient,&Orient::pos>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::arc>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::beamLength>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::startingRot>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::func>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::movement>
                                >;
};
//moving terrain
//the entity itself doesn't matter, rather it exists purely so it works with the rest of the entity frameworks
//construct, Debug mode, etc.
struct MovingTerrain : public Object<CircleCollider,ShapeRenderer<CIRCLE>,MovingTerrain>
{
    MoveFunc calcNewPos;
    Vector2 starting = {3000,3000};
    BlockType type = SOLID;
    MovingTerrain()
    {
        collider.radius = 0;
        followGravity = false;
        //tangible = false;
    }
    void onAdd();
    void update(Terrain&);
    void collideWith(PhysicsBody&);
protected:
    Vector2 moved = {};

};

template<>
struct Factory<MovingTerrain>
{
    static constexpr std::string ObjectName = "moving_terrain";

    using Base = FactoryBase<MovingTerrain,
                    access<MovingTerrain,&MovingTerrain::starting>,
                    access<MovingTerrain,&MovingTerrain::collider,&CircleCollider::radius>,
                    access<MovingTerrain,&MovingTerrain::type>,
                    access<MovingTerrain,&MovingTerrain::calcNewPos>>;
};

struct PushBot : public Object<RectCollider,TextureRenderer,PushBot>
{
    PushBot()
    {
        collider.width = 50;
        collider.height = 50;
        renderer.sprite = Globals::Game.Sprites.getSprite("bot.png");
    }
    virtual void update(Terrain&);
    void onCollide(PhysicsBody& other);
};

template<>
struct Factory<PushBot>
{
    static constexpr std::string ObjectName = "push_bot";
    using Base = FactoryBase<PushBot,
                                access<PushBot,&PushBot::orient,&Orient::pos>,
                                access<PushBot,&PushBot::orient,&Orient::facing>>;
};

struct LargePushBot : public PushBot
{
    LargePushBot()
    {
        collider.width = 200;
        collider.height = 200;
    }
    void activate(int pushAmount = 5); //set push to 5
    virtual void update(Terrain& t);
private:
    int push = 10; // if greater than 0, move to the right and decrement 1
};

template<>
struct Factory<LargePushBot>
{
    static constexpr std::string ObjectName = "large_push_bot";
    using Base = FactoryBase<LargePushBot,
                                access<LargePushBot,&LargePushBot::orient,&Orient::pos>,
                                access<LargePushBot,&LargePushBot::orient,&Orient::facing>>;
};

struct GlowStone : public Object<CircleCollider,TextureRenderer,GlowStone>
{
    GlowStone()
    {
        collider.radius = 200;
        renderer.sprite = Globals::Game.Sprites.getSprite("glowstone.png");
    }
    void onCollide(PhysicsBody& other);
};

struct CameraMoveRegion : public Object<RectCollider,NoRenderer,CameraMoveRegion>
{
    Vector2 cameraTarget = {};
    CameraMoveRegion()
    {
        collider.width = 100;
        collider.height = 100;
        followGravity = false;
    }
    void collideWith(PhysicsBody& other);
    void update(Terrain& t);
private:
    bool activated = false;
    bool wasActivated = false;
};

template<>
struct Factory<CameraMoveRegion>
{
    static constexpr char ObjectName[] = "camera_move_region";

    using Base = FactoryBase<CameraMoveRegion,
                    access<CameraMoveRegion,&CameraMoveRegion::orient,&Orient::pos>,
                    access<CameraMoveRegion,&CameraMoveRegion::collider,&RectCollider::width>,
                    access<CameraMoveRegion,&CameraMoveRegion::collider,&RectCollider::height>,
                    access<CameraMoveRegion,&CameraMoveRegion::cameraTarget>>;

};


#endif // ENEMY_H_INCLUDED
