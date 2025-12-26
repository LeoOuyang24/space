#include "../headers/item.h"
#include "../headers/collideTriggers.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"


void Collectible::onRestore()
{
    Globals::Game.addCollects(-1);
}

void CollectibleCollider::collideWith(PhysicsBody& self, PhysicsBody& other)
{
    Player* player = static_cast<Player*>(Globals::Game.getPlayer());
    if (&other == player)
    {
        Globals::Game.addCollects();
        player->addResetObject(self);
        self.setDead(true);
    }
}

bool Key::unlocks(Key::KeyVal keyVal, Key::KeyVal lockVal)
{
    return lockVal == Key::UNLOCKED || lockVal == keyVal;
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
