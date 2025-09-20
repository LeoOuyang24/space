#include <iostream>

#include "../headers/game.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"

Globals Globals::Game;

void Globals::setLayer(LayerType layer)
{
    if (terrain.getTerrain(layer))
    {
        currentLayer = layer;

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
    return terrain.getTerrain(currentLayer);
}

Globals::Globals()
{

}
