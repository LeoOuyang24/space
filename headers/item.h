#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "colliders.h"
#include "objects.h"
#include "render.h"
#include "factory.h"

class HoldThis;

struct Item : public Object<RectCollider,TextureRenderer,HoldThis>
{
public:
    Item(const Vector3& pos, const Vector2& dimen, Texture2D& sprite);
};

struct KeyCollider
{
    //only works if "self" is a Key
    void collideWith(PhysicsBody& self, PhysicsBody& other);
};

//keys are objects with a specific key value
//this key value is an integer that represents in
struct Key : public Object<RectCollider,TextureRenderer,KeyCollider,Key>
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

    Key(KeyVal key_, const Vector3& pos) : key(key_), Object({Vector2(pos.x,pos.y),pos.z},std::make_tuple(KEY_DIMEN.x,KEY_DIMEN.y),std::make_tuple(*Globals::Game.Sprites.getSprite(KEY_SPRITE_PATH)))
    {
        tint = key;
    }
    Key() : Object()
    {
        renderer.sprite = Globals::Game.Sprites.getSprite("key.png");
        collider.width = KEY_DIMEN.x;
        collider.height = KEY_DIMEN.y;
    }
    KeyVal getKey()
    {
        return key;
    }
};

template<>
struct Factory<Key>
{
  static constexpr std::string ObjectName = "key";
  using Base = FactoryBase<Key,
                        access<Key,&Key::key>,
                        access<Key,&Key::orient,&Orient::pos>,
                        access<Key,&Key::orient,&Orient::layer>>;
};

bool operator==(const Key::KeyVal& left, const Key::KeyVal& right);


#endif // ITEM_H_INCLUDED
