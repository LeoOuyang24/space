#include "../headers/collideTriggers.h"
#include "../headers/item.h"
#include "../headers/objects.h"

void HoldThis::collideWith(PhysicsBody& self, PhysicsBody& other)
{
    if (Globals::Game.player.get() == &other )
    {
        static_cast<Player*>(&other)->setHolding(*static_cast<Item*>(&self));
    }
}
