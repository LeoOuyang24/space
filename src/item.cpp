#include "../headers/item.h"
#include "../headers/collideTriggers.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"



Item::Item(const Vector3& pos, const Vector2& dimen, Texture2D& sprite) : Object(
                                                                                 {{pos.x,pos.y},pos.z},
                                                                                 std::make_tuple(dimen.x,dimen.y),
                                                                                 std::make_tuple(std::ref(sprite)),
                                                                                 std::make_tuple(HoldThis()))
{

}


bool Key::unlocks(Key::KeyVal keyVal, Key::KeyVal lockVal)
{
    return lockVal == Key::unlocked || lockVal == keyVal;
}

void KeyCollider::collideWith(PhysicsBody& self, PhysicsBody& other)
{
    if (&other == Globals::Game.player.get())
    {
        Key* key = static_cast<Key*>(&self);
        Player* player = static_cast<Player*>(&other);
        player->addKey(key->getKey());

        self.setDead(true);
    }



   /* Sequences::add({[k=key](int times) mutable {

                   k->renderer.render({RECT,{k->orient.pos - Vector2(0,10*times),k->orient.layer},&k->collider},WHITE);

                   return times >= 10;

                   }},false);*/

}


bool operator==(const Key::KeyVal& left, const Key::KeyVal& right)
{
    return left.r == right.r &&
            left.g == right.g &&
            left.b == right.b &&
            left.a == right.a ;
}
