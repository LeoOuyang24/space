#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include "objects.h"
#include "shape.h"
#include "render.h"
#include "player.h"
#include "collideTriggers.h"
#include "item.h"

struct Portal : public Object<CircleCollider,ShapeRenderer<CIRCLE>>
{
    Orient dest;
    Shader portalShader;
    RenderTexture2D texture;
    Portal(int x, int y,int z, int radius, const Vector3& dest_);
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
    std::shared_ptr<Portal> ptr;
    Key::KeyVal lockVal = Key::unlocked;

    static InteractAction createInteractFunc(TriggerPortalSpawn& self, const Vector2& disp);

    TriggerPortalSpawn(bool transition_, const Vector3& start,
                       const Vector3& end, int radius, Key::KeyVal keyVal_ = Key::unlocked) :
                                                                transition(transition_),
                                                                lockVal(keyVal_),
                                                                ptr(std::make_shared<Portal>(start.x,start.y,start.z,radius,end)),
                                                                InteractComponent(createInteractFunc(*this,{0,0}))
    {

    }

    TriggerPortalSpawn(bool transition_, const Vector2& disp, float z,
                       const Vector3& end, int radius, Key::KeyVal keyVal_ = Key::unlocked) :
                                                        transition(transition_),
                                                        lockVal(keyVal_),
                                                        ptr(std::make_shared<Portal>(0,0,z,radius,end)),
                                                        InteractComponent(createInteractFunc(*this,disp))
    {

    }

};

using PortalSpawner = Object<CircleCollider,TextureRenderer,TriggerPortalSpawn>;


#endif // PORTAL_H_INCLUDED
