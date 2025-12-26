#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "blocks.h"

#include <list>


typedef size_t LayerType;

class Orient;
class PhysicsBody;

struct ObjectLookup
{
    //every object in a level should be in this map. For most objects, the shared_ptr here will be the only pointer pointing to said object, but there may
    //be exceptions (ie the player);
    std::unordered_map<PhysicsBody*, std::shared_ptr<PhysicsBody>> objects;

    void addObject(PhysicsBody& obj);
    void addObject(std::shared_ptr<PhysicsBody> ptr);
    std::shared_ptr<PhysicsBody> getObject(PhysicsBody* body);

};



struct GlobalTerrain
{
    struct LayerInfo //carries info of this layer for when it needs to be saved
    {
        std::string configPath = ""; //path to the layer config file.
        std::string imagePath = "";
        Vector2 playerPos = {};
    };
    //adds an object to a given layer, does nothing if provided layer is out of bounds
    void addObject(std::shared_ptr<PhysicsBody> obj, LayerType layer);
    //removing an object is intersting. We immediately kill it in ObjectLookup. However, the weak_ptr may persist in the old layer.
    //We do a deferred removal in the layers, where when we call update(), if a weak_ptr is invalid (or is valid but object is not in this layer),
    //we remove the weak_ptr. As a result, a weak_ptr may persist in Layer::objects for some time
    void removeObject(PhysicsBody& obj);

    void pushBackTerrain(); //add terrain to furthest layer
    //load an image as the block data for the terrain at the provided layer
    //if it's out of bounds, push back a layer and modify that layer
    void loadTerrain(LayerType layer, std::string imagePath);
    void setLayerInfo(LayerType layer, const LayerInfo& info); //set a level's info.
    LayerType getLayerCount();
    Terrain* getTerrain(LayerType layer); //null if index is not valid
    void update(LayerType layer);
    void render();

    int getZOfLayer(LayerType index); //-1 if index is out of bounds. Returns an int because floating point values causes billboards to not be rendered
    Vector3 orientToVec3(const Orient& orient);

    LayerInfo getLayerInfo(LayerType index);
    std::string serialize(LayerType index);
private:
    struct Layer
    {
        Terrain terrain;
        //contains weak_ptrs that point to the shared_ptrs in ObjectLookup
        std::list<std::weak_ptr<PhysicsBody>> objects;
        LayerInfo info;
    };

    std::vector<Layer> layers;

    bool isValidObject(PhysicsBody* obj, LayerType layer); //true if obj should be updated (is alive, belongs in this layer, is not null
};

#endif // TERRAIN_H_INCLUDED
