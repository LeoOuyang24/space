#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <memory>
#include <raylib.h>

#include "terrain.h"
#include "sprites.h"
#include "interface.h"
#include "camera.h"

typedef uint16_t Frame;

class Player;


struct Globals
{
    static Globals Game;
    static constexpr bool DEBUG = true; //set to true for debug mode
    static constexpr Vector2 screenDimen = DEBUG ? Vector2{900,900} : Vector2{1920,1080};

    //the bigger the z, the further away from the screen
    static constexpr int MAX_Z = 2500; //furthest away something can be before going out of vision
    static constexpr int SPACE_Z = .01*MAX_Z; //distance between layers if there are 50 layers
    static constexpr int BACKGROUND_Z = MAX_Z - SPACE_Z; //z coordinate of background
    static constexpr int START_Z = BACKGROUND_Z - SPACE_Z*50;
    static constexpr int CAMERA_Z_DISP = 500; //how far the camera is at all times from getCurrentZ(). Constant magic number    static constexpr int CAMERA_Z_DISP = 500; //how far the camera is at all times from getCurrentZ(). Constant magic number



    size_t collects = 0;
    LayerType currentLayer = 0; //layer player is at
    std::shared_ptr<Player> player;

    void init();
    void update();
    void render();

    void setLayer(LayerType layer); //set layer, move the player, and the camera

    size_t getCollects();
    void addCollects(int val = 1);
    void setCollects(size_t val);

    float getCurrentZ();
    Camera3D getCamera();
    LayerType getCurrentLayer();
    Terrain* getCurrentTerrain();
    void addWorld(std::string_view path); //adds a new world from folder
    void startLoadWorld(const World& world); //load a preexisting world
    void setCurWorldThreaded(CurrentWorld cur);
    /**
     * @brief Run when a world is loaded
     * 
     */
    void onWorldLoaded(); 
    Texture2D getBG();

    void addObject(PhysicsBody& body, Orient o);
    void addObject(PhysicsBody& body, LayerType layer);
    void addObject(std::shared_ptr<PhysicsBody> ptr, LayerType layer);
    void addObject(std::shared_ptr<PhysicsBody> ptr);
    PhysicsBody* getPlayer();

    SpritesGlobal Sprites;
    GlobalTerrain terrain;
    ObjectLookup objects;
    Interface interface;
    GameCamera Camera;
    LevelLoader levelLoader;

    Worlds worlds;
    CurrentWorld curWorld = -1;

private:

    float accum = 0;
    float tick = 1/60.0f;
    float speed = 1;
    int frames = 0;

    Globals();
};

#endif // GAME_H_INCLUDED
