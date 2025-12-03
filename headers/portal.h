#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include "objects.h"
#include "shape.h"
#include "render.h"
#include "player.h"
#include "collideTriggers.h"
#include "item.h"

struct PortalCondition
{
    virtual bool unlocked() = 0;
    virtual void render(Shape shape) = 0;
};

struct TokenLocked : public PortalCondition
{
    size_t requirement = 0;
    bool unlocked()
    {
        return Globals::Game.getCollects() >= requirement;
    }
    void render(Shape shape)
    {
        DrawText(std::to_string(requirement),shape.orient.pos.x,shape.orient.pos.y,20,BLACK);
    }
};

struct UnlockPortal : public InteractComponent
{
    std::unique_ptr<PortalCondition> condition;
    UnlockPortal(PortalCondition* cond, Portal& owner);
};

struct Portal : public Object<CircleCollider,ShapeRenderer<CIRCLE>,UnlockCondition,Portal>
{

    Orient dest;
    static Shader PortalShader;
    RenderTexture2D texture;
    Portal(const Vector3& start, int radius, const Vector3& dest_);
    Portal();

    void update()
    {

    }
    void collideWith(PhysicsBody& other);
    void render();
};

//spawn a portal when interacted with
struct TriggerPortalSpawn : public InteractComponent
{
    bool active = true;
    bool transition = false;
    bool absolute = true; //true if "start" is the absolute position to spawn the portal; false if instead start is the displacement from current position

    Vector3 start = {}; //portal start point
    Vector3 end = {}; //portal end point

    Key::KeyVal lockVal = Key::UNLOCKED;

    static InteractAction createInteractFunc();

    TriggerPortalSpawn(bool transition_, const Vector3& start_,
                       const Vector3& end_, int radius, Key::KeyVal keyVal_ = Key::UNLOCKED) :
                                                                transition(transition_),
                                                                lockVal(keyVal_),
                                                                InteractComponent({}),
                                                                start(start_),
                                                                end(end_)
    {

    }

    TriggerPortalSpawn(bool transition_, const Vector2& disp,
                       const Vector3& end_, int radius, Key::KeyVal keyVal_ = Key::UNLOCKED) :
                                                        transition(transition_),
                                                        lockVal(keyVal_),
                                                        InteractComponent({}),
                                                        start(disp.x,disp.y,0),
                                                        end(end_),
                                                        absolute(false)

    {

    }

    TriggerPortalSpawn() : InteractComponent({})
    {

    }

    void interact(PhysicsBody&, PhysicsBody& other);

};

using PortalSpawner = Object<CircleCollider,TextureRenderer,TriggerPortalSpawn>;

template<>
struct Factory<Portal>
{
    static constexpr std::string ObjectName = "portal";
    using Base = FactoryBase<Portal,
                                access<Portal,&Portal::dest,&Orient::pos>,
                                access<Portal,&Portal::dest,&Orient::layer>,
                                access<Portal,&Portal::orient,&Orient::pos>,
                                access<Portal,&Portal::orient,&Orient::layer>,
                                access<Portal,&Portal::collider,&CircleCollider::radius>>;
};

template<>
struct Factory<PortalSpawner>
{
    static constexpr std::string ObjectName = "portal_spawner";
    using Base = FactoryBase<PortalSpawner,
                                access<PortalSpawner,&PortalSpawner::orient,&Orient::pos>,
                                access<PortalSpawner,&PortalSpawner::orient,&Orient::layer>,
                                access<PortalSpawner,&PortalSpawner::collider,&CircleCollider::radius>,
                                access<PortalSpawner,&PortalSpawner::renderer,&TextureRenderer::sprite>,
                                access<PortalSpawner,&PortalSpawner::collideTrigger,&TriggerPortalSpawn::start>,
                                access<PortalSpawner,&PortalSpawner::collideTrigger,&TriggerPortalSpawn::end>,
                                access<PortalSpawner,&PortalSpawner::collideTrigger,&TriggerPortalSpawn::absolute>>;

};


#endif // PORTAL_H_INCLUDED
