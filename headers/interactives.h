#ifndef INTERACTIVES_H_INCLUDED
#define INTERACTIVES_H_INCLUDED

#include "objects.h"
#include "render.h"
#include "factory.h"

struct Sign : public Object<RectCollider,TextureRenderer,Sign>
{
    static constexpr Vector2 SIGN_DIMEN = {40,40};
    static constexpr std::string SIGN_SPRITE_PATH = "sign.png";
    std::vector<std::string> message;
    Sign();

    void interactWith(PhysicsBody& other);
    std::string getMessage(size_t index) const;
    size_t getMessagesSize();
};
template<>
struct Factory<Sign>
{
    static constexpr std::string ObjectName = "sign";
    using Base = FactoryBase<Sign,
                                access<Sign,&Sign::message>,
                                access<Sign,&Sign::orient,&Orient::pos>>;
};


struct Rover : public Object<RectCollider,AnimeRenderer,Rover>
{
    static constexpr Vector2 ROVER_DIMEN = {110,110};
    static constexpr std::string ROVER_SPRITE_PATH = "rover2.png";
    bool on = true;
    Rover();
    void interactWith(PhysicsBody& other);
};

template<>
struct Factory<Rover>
{
    static constexpr std::string ObjectName = "rover";
    using Base = FactoryBase<Rover,
                                access<Rover,&Rover::orient,&Orient::pos>>;
};

struct BigSign : public Object<RectCollider,TextureRenderer,BigSign>
{
    BigSign();
};

template<>
struct Factory<BigSign>
{
    static constexpr std::string ObjectName = "big_sign";
    using Base = FactoryBase<BigSign,
                                access<BigSign,&BigSign::orient,&Orient::pos>,
                                access<BigSign,&BigSign::collider,&RectCollider::width>,
                                access<BigSign,&BigSign::collider,&RectCollider::height>,
                                access<BigSign,&BigSign::renderer,&TextureRenderer::sprite>>;
};

struct GravitySwitch : public Object<RectCollider,TextureRenderer,GravitySwitch>
{
    Vector2 gravityDir = {}; //NORMALIZED vector 
    bool active = true;

    GravitySwitch();
    void interactWith(PhysicsBody& other);
    void render();
};

template<>
struct Factory<GravitySwitch>
{
    static constexpr std::string ObjectName = "gravity_switch";
    using Base = FactoryBase<GravitySwitch,
                                access<GravitySwitch,&GravitySwitch::gravityDir>,
                                access<GravitySwitch,&GravitySwitch::orient,&Orient::pos>>;
};

struct DestroyLaser : public Object<RectCollider,TextureRenderer,DestroyLaser>
{
    DestroyLaser();
    void interactWith(PhysicsBody& other);
    void render();
private:
    bool activated = false;
};

template<>
struct Factory<DestroyLaser>
{
    static constexpr std::string ObjectName = "destroy_laser";
    using Base = FactoryBase<DestroyLaser,
                                access<DestroyLaser,&DestroyLaser::orient,&Orient::pos>,
                                //access<DestroyLaser,&DestroyLaser::orient,&Orient::rotation>,
                                accessSetter<DestroyLaser,[](DestroyLaser&,float deg){return DEG2RAD*deg;},&DestroyLaser::orient,&Orient::rotation>,
                                access<DestroyLaser,&DestroyLaser::orient,&Orient::facing>>;
};

//an object that moves the camera to a specific spot
struct Telescope : public Object<RectCollider,TextureRenderer,Telescope>
{
    Vector2 focusPoint = {};
    float zDisp = 0; //z displacement, z is always relative
    bool absolute = false; //whether "focusPoint" is relative or not
    Telescope()
    {
        renderer.sprite = Globals::Game.Sprites.getSprite("telescope.png");

        collider.width = 100;
        collider.height = 100;

        followGravity = true;
    }
    void interactWith(PhysicsBody& other);
    void update(Terrain& t);
    void setActivated(bool b);
protected:
    bool activated = false;
    bool justSet = false;
};

template<>
struct Factory<Telescope>
{
    static constexpr std::string ObjectName = "telescope";
    using Base = FactoryBase<Telescope,
                                access<Telescope,&Telescope::focusPoint>,
                                access<Telescope,&Telescope::zDisp>,
                                access<Telescope,&Telescope::absolute>,
                                access<Telescope,&Telescope::orient,&Orient::pos>>;
};

struct LifePod : public Object<RectCollider,TextureRenderer,LifePod>
{
    LifePod()
    {
        renderer.sprite = Globals::Game.Sprites.getSprite("lifepod.png");

        collider.width = 100;
        collider.height = 133;
        keyVal = 2;
        followGravity = true;
    }   

    void onCollide(PhysicsBody& other);
    void interactWith(PhysicsBody& other);
};

template<>
struct Factory<LifePod>
{
    static constexpr std::string ObjectName = "lifepod";
    using Base = FactoryBase<LifePod,
                                access<LifePod,&LifePod::orient,&Orient::pos>>;    
};

#endif // INTERACTIVES_H_INCLUDED
