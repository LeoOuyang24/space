#include "../headers/collideTriggers.h"
#include "../headers/item.h"
#include "../headers/objects.h"
#include "../headers/player.h"

void HoldThis::collideWith(PhysicsBody& self, PhysicsBody& other)
{
    if (&other  == Globals::Game.player.get())
    {
        static_cast<Player*>(&other)->setHolding(*static_cast<Item*>(&self));
    }
}
