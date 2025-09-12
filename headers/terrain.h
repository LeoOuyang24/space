#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "blocks.h"

class TerrainManager
{
    std::vector<Terrain> layer;
public:
    void pushBackTerrain(); //add terrain to furthest layer
    Terrain* getTerrain(size_t layer); //null if index is not valid
    void render();
};

#endif // TERRAIN_H_INCLUDED
