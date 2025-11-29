#include <iostream>
#include <fstream>

#include "../headers/game.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"
#include "../headers/factory.h"

void moveCamera(Camera3D& camera, const Vector2& pos)
{

    float disp = Globals::CAMERA_Z_DISP*tan(camera.fovy/2*DEG2RAD); //distance from the edge of the screen

    //clamps camera to level area
    Vector2 clampedPos = {
        Clamp(pos.x,disp,Terrain::MAX_TERRAIN_SIZE - disp),
        Clamp(pos.y,disp,Terrain::MAX_TERRAIN_SIZE - disp)
    };
    assignVector(camera.position,clampedPos);
    assignVector(camera.target,clampedPos);

}

Globals Globals::Game;

void Globals::init()
{

    Sprites.addSprites("sprites");

    //Globals::Game.terrain.loadTerrain("sprites/layers/level2.png");

    Player* ptr = new Player(Vector2(5500,5500));
    player.reset(ptr);
    objects.addObject(player);


   // setLayer(0);

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

void Globals::loadLevel(std::string path)
{
    std::ifstream levelFile;
    levelFile.open(path);

    if (levelFile.is_open())
    {
        std::string line;
        int lineNum = 0;
        while(std::getline(levelFile,line))
        {
            switch (lineNum)
            {
            case 0: //first line is terrain image
                terrain.loadTerrain(line,0);
                break;
            default:
                addObject(construct(line));
                break;
            }
            lineNum ++;
        }
        levelFile.close();
    }
    else
    {
        std::cerr << "ERROR Globals::loadLevel: error loading level: " << path << "\n";
    }
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
