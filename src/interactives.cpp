#include "../headers/interactives.h"
#include "../headers/audio.h"
#include "../headers/item.h"
#include "../headers/player.h"

Sign::Sign() : Object({},
                      std::make_tuple(SIGN_DIMEN.x,SIGN_DIMEN.y),
                      std::make_tuple(Globals::Game.Sprites.getSprite(SIGN_SPRITE_PATH)))
{

}

void Sign::interactWith(PhysicsBody& other)
{

    Globals::Game.interface.setMessage(this);
}

std::string Sign::getMessage(size_t index) const
{
    return index < message.size() ? message[index] : "";
}

size_t Sign::getMessagesSize()
{
    return message.size();
}

Rover::Rover() : Object({},std::make_tuple(ROVER_DIMEN.x,ROVER_DIMEN.y),
                        std::make_tuple(std::initializer_list<std::string_view>{"DEFAULT",ROVER_SPRITE_PATH,"OFF","rover_off.png"}))
{
    renderer.setState("DEFAULT");
}

void Rover::interactWith(PhysicsBody& other)
{
    if (on)
    {
        on = false;
        Globals::Game.addObject(*(new Battery()),{getPos(),orient.layer});
        SoundLibrary::loadBGM( on ? "music/world0_together.wav" : "music/world0_off.wav");
        renderer.setState(on ? "DEFAULT" : "OFF");
    }
}

BigSign::BigSign() : Object({},{},{})
{

}

GravitySwitch::GravitySwitch()
{
    collider.width = 100;
    collider.height = 100;
}

void GravitySwitch::interactWith(PhysicsBody& other)
{
    //Globals::Game.terrain.flipGravity();
    Globals::Game.getPlayer()->getForces().addForce(gravityDir*100.0f,Forces::GRAVITY);
    //Globals::Game.getPlayer()->setTangible(false);

    /*Sequences::add(true,
        [](int x){ Globals::Game.getPlayer()->setTangible(false); return x > 100;},
        [](int){Globals::Game.getPlayer()->setTangible(true);return true;});*/
}

void GravitySwitch::render()
{
    Object::render();

    Vector2 pos = getPos();
    float z = Globals::Game.terrain.getZOfLayer(orient.layer);

    float layers = Globals::Game.worlds[0].layers.size();
    float ratio = 3*Globals::CAMERA_Z_DISP/(Globals::CAMERA_Z_DISP + (Globals::BACKGROUND_Z - Globals::START_Z)/layers);

    DrawSprite3D(Globals::Game.Sprites.getSprite("left.png"),
                {1608*3,1643.5*3,
                    766*ratio,667*ratio},
                0,WHITE

            );


    DrawArrow3D(toVector3(pos - gravityDir*collider.width/2,z),
                toVector3(pos + gravityDir*collider.width/2,z),
                WHITE,3);
}

DestroyLaser::DestroyLaser()
{
    collider.width = 100;
    collider.height = 100;
    followGravity = false;
    renderer.sprite = Globals::Game.Sprites.getSprite("laser_beamer_off.png");
}

void DestroyLaser::interactWith(PhysicsBody& other)
{
    activated = true;
}

void DestroyLaser::render()
{
    Object::render();
    if (activated)
    {
        DrawLine3D(toVector3(getPos()),
                   toVector3(getPos()+orient.getFacingVector()*300)
                   ,RED,20);
    }
}


void Telescope::setActivated(bool b)
{
    activated = b;
    if (b)
    {
        justSet = true;
        Vector2 focalPoint = focusPoint + (getPos()*(!absolute));//getOrient().layer + layerDisp};
        Globals::Game.Camera.setCameraFollow({focalPoint.x,focalPoint.y,
            Globals::Game.terrain.getZOfLayer(getOrient().layer) + zDisp - Globals::CAMERA_Z_DISP});
    }
    else
    {
        Globals::Game.Camera.setCameraFollow(true);
    }
}

void Telescope::interactWith(PhysicsBody& other)
{
    setActivated(true);
}

void Telescope::update(Terrain& t)
{
    if (activated)
    {
        if (GetKeyPressed() && !justSet)
        {
            setActivated(false);
        }
    }
    justSet = false;
    Object::update(t);
}

void LifePod::onCollide(PhysicsBody& other)
{
    if (&other == Globals::Game.getPlayer())
    {
        Player* player = static_cast<Player*>(&other);
        if (player->getHolding() && player->getHolding()->getKeyVal() == keyVal)
        {
            Sequences::add({[shape=getShape()](int){
                suggestButtonPress(shape,"E");
                return true;
            }},false);
            if (IsKeyPressed(KEY_E))
            {
                interactWith(other);
            }
        }
        else
        {
            Sequences::add({[pos=getPos()](int){
                DrawSprite3D(Globals::Game.Sprites.getSprite("no_battery.png"),{pos.x,pos.y,100,100});
                return true;
            }},false);
        }
    }
}

void LifePod::interactWith(PhysicsBody& other)
{
    //Globals::Game.interface.setMenu(Menus::WORLD_MAP);
    Globals::Game.Camera.moveCamera(-Globals::BACKGROUND_Z*2,60);
    auto sequence = Sequences::waitFor(std::bind(Globals::Game.Camera.isDone,&Globals::Game.Camera),false);
    sequence->push_back(RunThis([](int){
        Globals::Game.interface.setMenu(Menus::WORLD_MAP); 
        //Globals::Game.terrain.clear();
        return true;
    }));
}