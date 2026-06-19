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
        if (player->getHolding() == &owner) //if currently being held by player
        {
            lastHeld = GetTime();
        }
        else if (&other == player && GetTime() - lastHeld >= 1) //if not being held by player but collided with player, we can be picked back up if it's been 1 second
        {
            player->setHolding(owner);
            onPickup();
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
    Debug::debugForces(*this);
    Object::update(terrain);
    if ((onGround || terrain.blockExists(getShape()) ) && collideTrigger.isThrown(*this)) //disappear if colliding with terrain after being thrown
    {
        setDead(true);
        Sequences::add(false,[start=GetTime(),pos=getPos()](int frames){
                       const Anime* anime = Globals::Game.Sprites.getAnime("death.png");
                       DrawAnime3D(anime->spritesheet,start,anime->info,{pos.x,pos.y,500,500},Globals::Game.getCurrentZ(),0,YELLOW);
                       return isAnimeDone(anime->info,frames);
                       });
    }
}

/*void BarrelReceiver::onCollide(PhysicsBody& other)
{
    if (other.getKeyVal() == this->getKeyVal() && !activated)
    {

        //onTrigger(*this);
        /*Globals::Game.addObject(*(new Portal(this->getPos() + this->orient.getNormal()*-100,
                                             this->orient.layer,
                                             {},
                                             this->orient.layer
                                             )),this->orient.layer);
       // onTrigger(*this);
    }
}*/

/*void BarrelReceiver::render()
{
    if (activated)
    {
        DrawSprite3D(Globals::Game.Sprites.getSprite("barrel.png"),Rectangle(getPos().x,getPos().y,30,60),-orient.rotation);
    }
    Object::render();
}*/

void TerrainPod::update(Terrain& t)
{
    Object::update(t);
    if (collideTrigger.isThrown(*this))
    {
        if (!activated && IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
        {
            activated = true;
            followGravity = false;
            collideTrigger.setPickupable(false);
            collider.radius *= 1.5;
            tint = YELLOW;
            Globals::Game.terrain.getTerrain(orient.layer)->addPlanet(*this,type);
        }
    }
}

void BigGear::onPickup()
{
    set_followGravity(true);
}

void BigGear::update(Terrain& t)
{
    if (t.blockExists(getShape()) && collideTrigger.isThrown(*this))
    {
        setDead(true);
            Sequences::add(false,[start=GetTime(),pos=getPos()](int frames){
                       const Anime* anime = Globals::Game.Sprites.getAnime("death.png");
                       DrawAnime3D(anime->spritesheet,start,anime->info,{pos.x,pos.y,500,500},Globals::Game.getCurrentZ(),0,YELLOW);
                       return isAnimeDone(anime->info,frames);
                       });
    }
    else
    {
        Object::update(t);
    }
}

bool operator==(const Key::KeyVal& left, const Key::KeyVal& right)
{
    return left.r == right.r &&
            left.g == right.g &&
            left.b == right.b &&
            left.a == right.a ;
}
