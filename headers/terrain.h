#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "blocks.h"

#include <list>


typedef size_t LayerType;

class PhysicsBody;

struct ObjectLookup
{
    //every object in a level should be in this map. For most objects, the shared_ptr here will be the only pointer pointing to said object, but there may
    //be exceptions (ie the player);
    std::unordered_map<PhysicsBody*, std::shared_ptr<PhysicsBody>> objects;

    void addObject(PhysicsBody& obj);
    void addObject(std::shared_ptr<PhysicsBody> ptr);
    std::shared_ptr<PhysicsBody> getObject(PhysicsBody& body);
};

class GlobalTerrain
{
    struct Layer
    {
        Terrain terrain;
        //contains weak_ptrs that point to the shared_ptrs in ObjectLookup
        std::list<std::weak_ptr<PhysicsBody>> objects;
    };

    std::vector<Layer> layers;
public:
    //adds an object to a given layer, does nothing if provided layer is out of bounds
    void addObject(std::shared_ptr<PhysicsBody> obj, LayerType layer);
    //removing an object is intersting. We immediately kill it in ObjectLookup. However, the weak_ptr may persist in the old layer.
    //We do a deferred removal in the layers, where when we call update(), if a weak_ptr is invalid (or is valid but object is not in this layer),
    //we remove the weak_ptr. As a result, a weak_ptr may persist in Layer::objects for some time
    void removeObject(PhysicsBody& obj);

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
