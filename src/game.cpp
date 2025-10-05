#include <iostream>

#include "../headers/game.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"

Globals Globals::Game;

void Globals::init()
{
    Player::PlayerSprite = LoadTexture("sprites/guy.png");

    Globals::Game.terrain.loadTerrain("sprites/layers/curve.png");
    Globals::Game.terrain.loadTerrain("sprites/layers/circle.png");

    Player* ptr = new Player(Vector2(0,150));
    addObject(*(ptr));
    player = static_pointer_cast<Player>(objects.getObject(*ptr));


    setLayer(0);

    camera.projection = CAMERA_PERSPECTIVE;
    camera.position = Vector3(0,0,getCurrentZ() - Globals::CAMERA_Z_DISP);
    camera.target = Vector3(0,0,Globals::BACKGROUND_Z);
    //camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    //camera.rotation = 0.0f;
    //camera.zoom = 1.0f;
    camera.up = {0,-1,0};
    camera.fovy = 90;

}

void Globals::setLayer(LayerType layer)
{
    if (terrain.getTerrain(layer))
    {
        currentLayer = layer;
        if (player.get())
        {
            player->orient.layer = layer;
            terrain.addObject(player,layer);
        }

        RunThis runThis = {[](int){return true;}};

        Sequences::add({[camera=&(this->camera),endZ=getCurrentZ()  - Globals::CAMERA_Z_DISP,startZ = camera.position.z](int runTimes){

                       camera->position.z = Lerp(startZ,endZ,runTimes/50.0);
                       return runTimes == 50;

                       }},false);
    }
    else
    {
        std::cerr << "Globals::setLayer ERROR: layer " << layer << " is invalid!\n";
    }
}


float Globals::getCurrentZ()
{
    return terrain.getZOfLayer(currentLayer);
}

LayerType Globals::getCurrentLayer()
{
    return currentLayer;
}

Terrain* Globals::getCurrentTerrain()
{
    return terrain.getTerrain(getCurrentLayer());
}

void Globals::addObject(PhysicsBody& body)
{
    objects.addObject(body);
    terrain.addObject(objects.getObject(body),body.orient.layer);
}

void Globals::addObject(std::shared_ptr<PhysicsBody> ptr)
{
    objects.addObject(ptr);
    terrain.addObject(ptr,ptr->orient.layer);
}

Globals::Globals()
{

}
