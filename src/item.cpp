#include "../headers/item.h"
#include "../headers/collideTriggers.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"
#include "../headers/enemy.h"


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

void Booster::onCollide(PhysicsBody& other)
{
    if (&other == Globals::Game.getPlayer())
    {
        Player* player = static_cast<Player*>(Globals::Game.getPlayer());
        player->boosted  = false;
        this->dead = true;
    }
}

void PickupComponent::collideWith(PhysicsBody& owner, PhysicsBody& other)
{
    if (pickupable)
    {
        Player* player = static_cast<Player*>(Globals::Game.getPlayer());
        if (player->getHolding() == &owner)
        {
            lastHeld = GetTime();
        }
        else if (&other == player && GetTime() - lastHeld >= 1)
        {
            player->setHolding(owner);
        }
    }
}

bool PickupComponent::isThrown(PhysicsBody& owner)
{
    //if we were held at one point but no longer, we must've been thrown at some point
    return lastHeld > 0 && static_cast<Player*>(Globals::Game.getPlayer())->getHolding() != &owner;
}

void PickupComponent::setPickupable(bool val)
{
    pickupable = val;
}

void Barrel::update(Terrain& terrain)
{
    Object::update(terrain);

    if (onGround && collideTrigger.isThrown(*this)) //last two conditions are only true if we have been dropped
    {
        setDead(true);
        Sequences::add(false,[start=GetTime(),pos=getPos()](int frames){
                       const Anime* anime = Globals::Game.Sprites.getAnime("death.png");
                       DrawAnime3D(anime->spritesheet,start,anime->info,{pos.x,pos.y,500,500},Globals::Game.getCurrentZ(),0,YELLOW);
                       return isAnimeDone(anime->info,frames);
                       });
    }
}

void BarrelReceiver::onCollide(PhysicsBody& other)
{
    if (other.getKeyVal() == this->getKeyVal() && !activated)
    {
        other.setDead(true);
        activated = true;
        if (onTrigger.get())
        {
            (*onTrigger)(*this);
        }
        //onTrigger(*this);
        /*Globals::Game.addObject(*(new Portal(this->getPos() + this->orient.getNormal()*-100,
                                             this->orient.layer,
                                             {},
                                             this->orient.layer
                                             )),this->orient.layer);*/
       // onTrigger(*this);
    }
}

void AntiGravPod::update(Terrain& t)
{
    Object::update(t);
    if (collideTrigger.isThrown(*this))
    {
        if (!activated && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            activated = true;
            followGravity = false;
            collideTrigger.setPickupable(false);
            tint = YELLOW;
            Globals::Game.terrain.getTerrain(orient.layer)->addPlanet(*this,ANTI);
        }
    }
}

bool operator==(const Key::KeyVal& left, const Key::KeyVal& right)
{
    return left.r == right.r &&
            left.g == right.g &&
            left.b == right.b &&
            left.a == right.a ;
}
