#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "blocks.h"


typedef size_t LayerType;

class PhysicsBody;
struct Layer
{
    Terrain terrain;

    //smart pointers? more like FART pointers
    std::vector<PhysicsBody*> objects;

    void render(float z);
    void update();
};

class GlobalTerrain
{
    std::vector<Layer> layers;
public:
    //adds an object to a given layer, does nothing if provided layer is out of bounds
    void addObject(PhysicsBody& obj, LayerType layer);

    void pushBackTerrain(); //add terrain to furthest layer
    //load an image as the block data for the terrain at the provided layer
    //if it's out of bounds, push back a layer and modify that layer
    void loadTerrain(std::string imagePath, LayerType layer = -1);
    Terrain* getTerrain(LayerType layer); //null if index is not valid
    void update(LayerType layer);
    void render();
    float getZOfLayer(LayerType index); //-1 if index is out of bounds
};

#endif // TERRAIN_H_INCLUDED
