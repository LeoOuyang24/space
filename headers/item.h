#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "objects.h"
#include "render.h"
#include "player.h"

class HoldThis;

struct Item : public Object<RectCollider,TextureRenderer,HoldThis>
{
public:
    Item(const Vector3& pos, const Vector2& dimen, Texture2D& sprite);
    virtual size_t getKey() //return key value
    {
        return 0;
    }
};

struct Key : public Item
{
    const size_t key;
    static const size_t unlocked = 0; //value for unlocked things
    static bool unlocks(size_t keyVal, size_t lockVal);

    Key(size_t key_, const Vector3& pos, const Vector2& dimen, Texture2D& sprite) : key(key_), Item(pos,dimen,sprite)
    {

    }
    size_t getKey()
    {
        return key;
    }
};

#endif // ITEM_H_INCLUDED
