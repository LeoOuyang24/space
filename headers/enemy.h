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

struct LaserBeamEnemy : Object<RectCollider,TextureRenderer,LaserBeamEnemy>
{
    enum RotateFunc : int
    {
        CONSTANT = 0,
        SINE
    };

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

template<>
struct Factory<LaserBeamEnemy>
{
    static constexpr std::string ObjectName = "laser_beamer";
    using Base = FactoryBase<LaserBeamEnemy,
                                access<LaserBeamEnemy,&LaserBeamEnemy::orient,&Orient::pos>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::arc>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::beamLength>,
                                access<LaserBeamEnemy,&LaserBeamEnemy::orient,&Orient::rotation>,
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

#endif // ENEMY_H_INCLUDED
