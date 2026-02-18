#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "objects.h"
#include "factory.h"
#include "render.h"

struct GrapplePoint : public Object<CircleCollider,TextureRenderer,GrapplePoint>
{
    GrapplePoint();
    void update(Terrain& terrain);
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

    RenderTexture laserBeam;
    LaserBeamEnemy()
    {
        followGravity = false;

        laserBeam = LoadRenderTexture(1000,1000);
        BeginTextureMode(laserBeam);
            ClearBackground(RED);
        EndTextureMode();

        collider.width = 100;
        collider.height = 100;
        renderer.sprite = Globals::Game.Sprites.getSprite("laser_beamer.png");
    }

    void render();
    void update(Terrain& t);
    Shape getShape(); //for collision detection purposes, only the laser beam's shape matters
    void collideWith(PhysicsBody& other);
};

//moving terrain
//the entity itself doesn't matter, rather it exists purely so it works with the rest of the entity frameworks
//construct, Debug mode, etc.
struct MovingTerrain : public Object<CircleCollider,ShapeRenderer<CIRCLE>,MovingTerrain>
{
    struct MoveFunc //represents a function that determines how the terrain moves
    {
        std::function<Vector2(const Orient& o,const Vector2& starting,uint16_t,float speed)> moveFunc;
        std::function<std::string()>toString;
        uint16_t frame = 0; //used to calculate position, increments each time calcNewPos is called

        Vector2 operator()(const Orient& o,const Vector2& starting,float speed)
        {
            frame++;
            return moveFunc(o,starting,frame,speed);
        }
    };
    MoveFunc calcNewPos;
    float speed = 0;
    Vector2 starting = {3000,3000};

    MovingTerrain()
    {
        collider.radius = 0;
        tangible = false;
    }
    void onAdd();
    void update(Terrain&);
    Planet getPlanet();
private:
    Planet planet; //unused currently
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
                                access<LaserBeamEnemy,&LaserBeamEnemy::func>
                                >;
};

template<>
struct Factory<GrapplePoint>
{
    static constexpr std::string ObjectName = "grapple_point";
    using Base = FactoryBase<GrapplePoint,
                                access<GrapplePoint,&GrapplePoint::orient,&Orient::pos>
                                >;
};

template<>
struct Factory<MovingTerrain>
{
    static constexpr std::string ObjectName = "moving_terrain";

    using Base = FactoryBase<MovingTerrain,
                    access<MovingTerrain,&MovingTerrain::starting>,
                    access<MovingTerrain,&MovingTerrain::collider,&CircleCollider::radius>,
                    access<MovingTerrain,&MovingTerrain::speed>,
                    access<MovingTerrain,&MovingTerrain::calcNewPos>>;
};

#endif // ENEMY_H_INCLUDED
