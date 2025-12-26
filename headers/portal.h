#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include "raylib_helper.h"

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
    virtual std::string toString() = 0;
};

struct TokenLocked : public PortalCondition
{

    size_t requirement = 0;
    TokenLocked(size_t req) : requirement(req)
    {

    }
    bool unlocked();
    void render(Shape shape);
    std::string toString();


};


struct Portal : public Object<CircleCollider,ShapeRenderer<CIRCLE>,Portal>
{
    std::unique_ptr<PortalCondition> cond;
    Vector2 destPos = {};
    int layerDisp = 0;
    static Shader PortalShader;
    RenderTexture2D texture;
    Portal();
    bool unlocked();
    void update()
    {

    }
    void interactWith(PhysicsBody& other);
    void render();
};

template<>
struct Factory<Portal>
{
    static constexpr std::string ObjectName = "portal";
    using Base = FactoryBase<Portal,
                                access<Portal,&Portal::destPos>,
                                access<Portal,&Portal::layerDisp>,
                                access<Portal,&Portal::orient,&Orient::pos>,
                                access<Portal,&Portal::cond>,
                                access<Portal,&Portal::collider,&CircleCollider::radius>>;
};

#endif // PORTAL_H_INCLUDED
