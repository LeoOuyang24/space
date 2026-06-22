#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "colliders.h"
#include "objects.h"
#include "render.h"
#include "factory.h"
#include "collideTriggers.h"

class HoldThis;

struct KeyCollider
{
    //only works if "self" is a Key
    static void collideWith(PhysicsBody& self, PhysicsBody& other);
};

//keys are objects with a specific key value
//this key value is an integer that represents in
struct Key : public Object<RectCollider,TextureRenderer,Key,KeyCollider>
{
    typedef Color KeyVal;

    static constexpr Vector2 KEY_DIMEN = {20,20};
    static constexpr std::string KEY_SPRITE_PATH = "key.png";


    static constexpr KeyVal UNLOCKED = WHITE; //value for unlocked things

    KeyVal key = UNLOCKED;


    static bool unlocks(KeyVal keyVal, KeyVal lockVal);
    template<typename T>
    static bool unlocks(const T& container,KeyVal lockVal)
    {
        return container.find(lockVal) != container.end();
    }

    Key(KeyVal key_, const Vector3& pos) : Object({Vector2(pos.x,pos.y),pos.z},std::make_tuple(KEY_DIMEN.x,KEY_DIMEN.y),
                                                             std::make_tuple(Globals::Game.Sprites.getSprite(KEY_SPRITE_PATH))),
                                                             key(key_) 
    {
        tint = key;
    }
    Key() : Object()
    {
        renderer.setSprite(Globals::Game.Sprites.getSprite("key.png"));
        collider.width = KEY_DIMEN.x;
        collider.height = KEY_DIMEN.y;
    }
    KeyVal getKey()
    {
        return key;
    }
};

struct CollectibleCollider
{
    static void collideWith(PhysicsBody& self, PhysicsBody& other);
};

struct Collectible : public Object<CircleCollider,TextureRenderer,Collectible,CollectibleCollider>
{
    Collectible() : Object({},std::make_tuple(20),std::make_tuple(Globals::Game.Sprites.getSprite("gear.png")))
    {
        followGravity = false;
    }
    void onRestore();
};


struct Booster : public Object<CircleCollider,TextureRenderer,Booster>
{
    Booster()
    {
        followGravity = false;
        collider.radius = 10;
        renderer.sprite = Globals::Game.Sprites.getSprite("grapple.png");
    }
    void onCollide(PhysicsBody& other);
};

struct PickupComponent
{
    double lastHeld = 0;
    bool held = false; //true if currently held
    void collideWith(PhysicsBody& owner, PhysicsBody& other);
    bool isThrown(PhysicsBody& owner); //true if we have been thrown at least once
    void setPickupable(bool val);
protected:
    //called when picked up
    virtual void onPickup(){};
private:
    bool pickupable = true; //set this to false if you want an object to no longer be pickup-able
};

struct Barrel : public Object<RectCollider,TextureRenderer,Barrel,PickupComponent>
{
    double held = 0;
    Barrel()
    {
        keyVal = 1;
        followGravity = true;
        collider.width = 30;
        collider.height = 60;
        renderer.sprite = Globals::Game.Sprites.getSprite("barrel.png");
    }
    void update(Terrain& terrain);
    //void onCollide(PhysicsBody& other);
};


template<>
struct Factory<Barrel>
{
    static constexpr std::string_view ObjectName = "barrel";
    static constexpr EMPTY_TYPE DontSerialize = {};
    using Base = FactoryBase<Barrel,
                            access<Barrel,&Barrel::orient,&Orient::pos>>;
};

template<typename PhysicsBodyType>
struct GenericSpawner : public Object<RectCollider,TextureRenderer,GenericSpawner<PhysicsBodyType>>
{
    GenericSpawner()
    {
        this->collider.width = 100;
        this->collider.height = 150;
        this->set_followGravity(false);
        this->renderer.sprite = Globals::Game.Sprites.getSprite("barrel_spawner.png");
    }
    void update(Terrain& terrain)
    {
        if (!baby.lock().get() && activated)
        {
            PhysicsBodyType* obj = new PhysicsBodyType();
            obj->setPos(this->orient.pos + Vector2(10,0));
            Globals::Game.addObject(*obj,this->orient.layer);
            this->baby = std::static_pointer_cast<PhysicsBodyType>(Globals::Game.objects.getObject(obj));
        }
        Object<RectCollider,TextureRenderer,GenericSpawner<PhysicsBodyType>>::update(terrain);
    }
    void interactWith(PhysicsBody& other)
    {
        activated = true;
        if (PhysicsBodyType* brah = baby.lock().get())
        {
            brah->setDead(true);
        }
    }

private:
    std::weak_ptr<PhysicsBodyType> baby; //if our baby is dead, respawn!
    bool activated = false; //don't activate until first interaction
};


//generalist class that does something upon receiving another object
template<typename T>
requires IsObject<T>
struct ObjReceiver : public Object<RectCollider,TextureRenderer,ObjReceiver<T>>
{
    T renderSlave; //keep a copy of the thing we are receiving around, just to render it
    SignalName signal;

    ObjReceiver()
    {
        this->keyVal = 1;
        this->followGravity = true;
        this->collider.width = 150;
        this->collider.height = 200;
        this->renderer.sprite = Globals::Game.Sprites.getSprite("barrel_receiver.png");
    }
    void onCollide(PhysicsBody& other)
    {
        if (other.getName() == Factory<T>::ObjectName && !activated)
        {
            other.setDead(true);
            activated = true;
            Globals::Game.terrain.emitSignal(signal,this);
            onReceive();
        }
    }
    virtual void onReceive(){};
    void render()
    {
        float rotation = this->getOrient().rotation - M_PI/2;
        renderSlave.setPos(this->getPos() + Vector2(cos(rotation),sin(rotation))*.25*this->collider.height);
        renderSlave.setLayer(this->orient.layer);
        Object<RectCollider,TextureRenderer,ObjReceiver<T>>::render();
        renderSlave.render();
    }
protected:
    make_setter(activated,bool);
private:
    //true if we want this to no longer receive
    bool activated = false;
};

//wrapper class that allows us to use strings in template parameters
template<size_t N>
struct CompileString
{
    char data[N];
    constexpr CompileString(const char(& str)[N])
    {
        for (size_t i = 0; i < N; i ++)
        {
            data[i] = str[i];
        }
    }
};

/**
 * @brief generalist factory class for any specialization or descendant of ObjReceiver
 * 
 * @tparam T full type of the descendant or specialization
 * @tparam ObjName compile-time string for the object name
 */
template<typename T,CompileString ObjName>
struct ReceiverFactory
{
    static constexpr std::string_view ObjectName = ObjName.data;
    using Base = FactoryBase<T,
                            access<T,&T::orient,&Orient::pos>,
                            access<T,&T::keyVal>,
                            access<T,&T::signal>>;
};

using BarrelReceiver = ObjReceiver<Barrel>;
template<>
struct Factory<BarrelReceiver> : ReceiverFactory<BarrelReceiver,"barrel_receiver">{};

struct BigGear : public Object<CircleCollider,TextureRenderer,BigGear,PickupComponent>
{
    BigGear()
    {
        this->followGravity = false; //follows physics once picked up
        this->collider.radius = 50;
        this->renderer.sprite = Globals::Game.Sprites.getSprite("gear.png");
    }

    void onPickup();
    void update(Terrain& t);

};

template<>
struct Factory<BigGear>
{
    static constexpr char ObjectName[] = "big_gear";
    using Base = FactoryBase<BigGear,
                        access<BigGear,&BigGear::orient,&Orient::pos>>;
};

struct BigGearReceiver : public ObjReceiver<BigGear>
{
    using SerializerType = Factory<BigGearReceiver>;

    BigGearReceiver() : ObjReceiver<BigGear>(){};
    void onReceive();
};

template<> struct Factory<BigGearReceiver> : ReceiverFactory<BigGearReceiver,"big_gear_receiver">{};

struct TerrainPod :  public Object<CircleCollider,TextureRenderer,TerrainPod,PickupComponent>
{
    TerrainPod()
    {
        this->followGravity = true;
        this->collider.radius = 50;
        this->renderer.sprite = Globals::Game.Sprites.getSprite("grapple.png");
    }
    void update(Terrain&);
private:
    bool activated = false;
    BlockType type = SOLID;
};

template<>
struct Factory<Collectible>
{
  static constexpr std::string ObjectName = "gear";
  using Base = FactoryBase<Collectible,
                        access<Collectible,&Collectible::orient,&Orient::pos>>;
};

template<>
struct Factory<Key>
{
  static constexpr std::string ObjectName = "key";
  using Base = FactoryBase<Key,
                        access<Key,&Key::key>,
                        access<Key,&Key::orient,&Orient::pos>>;
};

template<>
struct Factory<Booster>
{
    static constexpr std::string ObjectName = "booster";
    using Base = FactoryBase<Booster,
                            access<Booster,&Booster::orient,&Orient::pos>>;
};



typedef GenericSpawner<Barrel> BarrelSpawner;

template<>
struct Factory<GenericSpawner<Barrel>>
{
    static constexpr std::string ObjectName = "barrel_spawner";
    using Base = FactoryBase<GenericSpawner<Barrel>,
                            access<GenericSpawner<Barrel>,&GenericSpawner<Barrel>::orient,&Orient::pos>>;
};

template<>
struct Factory<GenericSpawner<TerrainPod>>
{
    static constexpr std::string ObjectName = "terrain_spawner";
    using Base = FactoryBase<GenericSpawner<TerrainPod>,
                            access<GenericSpawner<TerrainPod>,&GenericSpawner<TerrainPod>::orient,&Orient::pos>>;   
};

struct Battery : public Object<RectCollider,TextureRenderer,Battery,PickupComponent>
{
    Battery()
    {
        keyVal = 2;
        followGravity = true;
        collider.width = 30;
        collider.height = 60;
        renderer.sprite = Globals::Game.Sprites.getSprite("battery.png");
    }
};

template<>
struct Factory<Battery>
{
    static constexpr std::string ObjectName = "battery";
    using Base = FactoryBase<Battery,
                            access<Battery,&Battery::orient,&Orient::pos>>;         
};


bool operator==(const Key::KeyVal& left, const Key::KeyVal& right);


#endif // ITEM_H_INCLUDED
