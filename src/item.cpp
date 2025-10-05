#include "../headers/item.h"
#include "../headers/collideTriggers.h"


bool Key::unlocks(size_t keyVal, size_t lockVal)
{
    return lockVal == Key::unlocked || lockVal == keyVal;
}

Item::Item(const Vector3& pos, const Vector2& dimen, Texture2D& sprite) : Object(
                                                                                 {{pos.x,pos.y},pos.z},
                                                                                 std::make_tuple(dimen.x,dimen.y),
                                                                                 std::make_tuple(std::ref(sprite)),
                                                                                 std::make_tuple(HoldThis()))
{

}

