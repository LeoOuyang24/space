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
    Player* player = static_cast<Player*>(Globals::Game.getPlayer());
    if (player->getHolding() == &owner)
    {
        lastHeld = GetTime();
    }
    else if (&other == player && GetTime() - lastHeld >= 1)
    {
        player->holding = Globals::Game.objects.getObject(&owner);
    }
}

void Barrel::update(Terrain& terrain)
{
    Object<RectCollider,TextureRenderer,Barrel,PickupComponent>::update(terrain);

    if (onGround && collideTrigger.lastHeld > 0 && Globals::Game.player->getHolding() != this) //last two conditions are only true if we have been dropped
    {
        setDead(true);
        Sequences::add(false,[start=GetTime(),pos=getPos()](int frames){
                       const Anime* anime = Globals::Game.Sprites.getAnime("death.png");
                       DrawAnime3D(anime->spritesheet,start,anime->info,{pos.x,pos.y,500,500},Globals::Game.getCurrentZ(),0,YELLOW);
                       return isAnimeDone(anime->info,frames);
                       });
    }
}

void BarrelSpawner::update(Terrain& terrain)
{
    if (!baby.lock().get())
    {
        Barrel* barrel = new Barrel();
        barrel->setPos(orient.pos + Vector2(10,0));
        Globals::Game.addObject(*barrel,orient.layer);
        baby = std::static_pointer_cast<Barrel>(Globals::Game.objects.getObject(barrel));
    }
    Object::update(terrain);
}

void BarrelSpawner::interactWith(PhysicsBody& other)
{
    if (Barrel* brah = baby.lock().get())
    {
        brah->setDead(true);
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
            std::cout << "triggered\n";
            (*onTrigger)(*this);
        }
        //onTrigger(*this);
        /*Globals::Game.addObject(*(new Portal(this->getPos() + this->orient.getNormal()*-100,
                                             this->orient.layer,
                                             {},
                                             this->orient.layer
                                             )),this->orient.layer);*/
        double start = GetTime();
       // onTrigger(*this);
    }
}

bool operator==(const Key::KeyVal& left, const Key::KeyVal& right)
{
    return left.r == right.r &&
            left.g == right.g &&
            left.b == right.b &&
            left.a == right.a ;
}
