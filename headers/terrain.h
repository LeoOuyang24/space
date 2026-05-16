#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "blocks.h"
#include "checkFields.h"

#include "signals.h"

#include <list>
#include <set>
#include <thread>
#include <atomic>

typedef size_t LayerType;

class Orient;
class PhysicsBody;

struct ObjectLookup
{
    //every object in a level should be in this map. For most objects, the shared_ptr here will be the only pointer pointing to said object, but there may
    //be exceptions (ie the player);
    std::unordered_map<PhysicsBody*, std::shared_ptr<PhysicsBody>> objects;

    void addObject(PhysicsBody& obj);
    void addObject(std::shared_ptr<PhysicsBody> ptr); //not passing by reference here because when adding the player shared_ptr, it has to be converted from player pointer to physicsbody pointer, and that makes this fail. 
    //removes an object from "objects". If there are any other shared pointers, the object will not be destroyed
    //for example, this commonly happens when the player dies after picking up a gear and the gear has to be reset
    void eraseObject(PhysicsBody& obj);
    std::shared_ptr<PhysicsBody> getObject(PhysicsBody* body);

    void clear();

};



struct GlobalTerrain
{
    static constexpr float GRAVITY_CONSTANT = 0.25f;
    struct LayerInfo //carries info of this layer for when it needs to be saved
    {
        std::string configPath = ""; //path to the layer config file.
        std::string imagePath = "";
        Vector2 playerPos = {};
    };

    /**
     * @brief adds an object to a given layer, does nothing if provided layer is out of bounds
     * 
     */
    void addObject(std::shared_ptr<PhysicsBody> obj, LayerType layer);

    /**
     * @brief Moves "body" to newLayer. Does NOT add it to ObjectLookup, it is assumed it is already in there. If the object is the player, we explicity remove it from our layer
     * 
     * @param body
     * @param newLayer
     */
    void moveObject(std::shared_ptr<PhysicsBody> body, LayerType newLayer);


    void pushBackTerrain(); //add terrain to furthest layer
 
    /**
     * @brief Loads blocks at index "layer" using the provided Image. DOES NOT CLEAN UP THE IMAGE
     * 
     * @param layer the layer index
     * @param img, the image that contains our blocks
     */
    void loadTerrain(LayerType layer, const Image& img);
    void setLayerInfo(LayerType layer, const LayerInfo& info); //set a level's info.
    LayerType getLayerCount();
    Terrain* getTerrain(LayerType layer); //null if index is not valid
    void update(LayerType layer);
    void render();
    void clear(); //clears out each layer's blocks and objects, but doesn't delete the layers to reuse them later

    int getZOfLayer(LayerType index); //-1 if index is out of bounds. Returns an int because floating point values causes billboards to not be rendered
    Vector3 orientToVec3(const Orient& orient);

    LayerInfo getLayerInfo(LayerType index);
    std::string serialize(LayerType index);
    enum GravityMode
    {
        PLANET, //gravity is based on nearby terrain
        DOWN, //gravity moves down
        POINT, //gravity focuses on "gravityCenter"
        ENUM_SIZE
    };

    make_getter(gravityMode,GravityMode);
    make_setter(gravityMode,GravityMode);
    void flipGravity(); //swap between planet and down gravity

    void setSignalSet(SignalSet& set);
    void emitSignal(SignalName,void* val);

private:
    SignalSet* signals = nullptr;

    GravityMode gravityMode = PLANET;
    Vector2 gravityCenter = {}; //point at which gravity is focusing on when gravityMode is POINT

    struct Layer
    {
        Terrain terrain;
        //contains weak_ptrs that point to the shared_ptrs in ObjectLookup
        std::set<std::weak_ptr<PhysicsBody>,std::owner_less<std::weak_ptr<PhysicsBody>>> objects;
        LayerInfo info;
    };

    std::vector<Layer> layers;

    bool isValidObject(PhysicsBody* obj, LayerType layer); //true if obj should be updated (is alive, belongs in this layer, is not null

};

struct World
{
    SignalSet signals;
    std::vector<std::string> layers;
    std::string bg_path = "";
    Texture2D bg; //worlds own their backgrounds; these sprites are not accessible in Globals::Game.Sprites (just doesn't seem necessary)
};

typedef std::vector<World> Worlds;
typedef size_t CurrentWorld;

struct LevelLoader
{

    //an object that represents an unloaded layer. Can be made by multithreaded processes then loaded all at once to load a world
    struct PreLayer
    {
        GlobalTerrain::LayerInfo info;
        Image blocks; //image containing all the blocks.
        std::vector<std::shared_ptr<PhysicsBody>> objs;//loaded objects to be added to the level
    };  
    /**
     * @brief Begins loading a world at given path
     * 
     * @param worldPath 
     */
    void loadWorld(const World& world);
    /**
     * @brief  Returns true if any thread is still loading a level
     * 
     * @return true 
     * @return false 
     */
    bool getIsLoading();

    /**
     * @brief Get the ith preloaded layer. Make sure to call getIsLoading to maek sure loading is not in progress!
     * 
     * @param i 
     * @return PreLayer 
     */
    PreLayer getLoadedLayer(size_t i); 

    /**
     * @brief Return true if world is fully loaded
     * 
     * @return true 
     * @return false 
     */
    bool getReady();

    /**
     * @brief check if we have are done pre-loading. If so, load the world
     * 
     */
    void monitor();
private:
    /**
     * @brief clear preload list
     * 
     */
    void clear(); 
    void loadPreLayer(PreLayer& preload, std::string layerFile);

    std::vector<std::jthread> threads;
    std::vector<PreLayer> preloads;
    std::atomic<int> loaded = 0;
    bool ready = false;
};

#endif // TERRAIN_H_INCLUDED
