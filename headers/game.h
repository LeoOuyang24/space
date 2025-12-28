#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <memory>

#include <raylib.h>

#include "terrain.h"
#include "sprites.h"
#include "interface.h"


class Player;
struct Globals
{
    static Globals Game;

    //the bigger the z, the further away from the screen
    static constexpr int MAX_Z = 1000; //furthest away something can be before going out of vision
    static constexpr int SPACE_Z = .01*MAX_Z; //distance between layers if there are 50 layers
    static constexpr int BACKGROUND_Z = MAX_Z - SPACE_Z; //z coordinate of background
    static constexpr int START_Z = BACKGROUND_Z - SPACE_Z*50;
    static constexpr int CAMERA_Z_DISP = START_Z*0.7; //how far the camera is at all times from getCurrentZ()

    static constexpr Vector2 screenDimen = {900,900};

    size_t collects = 0;
    LayerType currentLayer = 0; //layer player is at
    std::shared_ptr<Player> player;

    void init();
    void setLayer(LayerType layer); //set layer, move the player, and the camera

    size_t getCollects();
    void addCollects(int val = 1);
    void setCollects(size_t val);

    float getCurrentZ();
    LayerType getCurrentLayer();
    Terrain* getCurrentTerrain();
    void loadLevel(std::string path);

    void addObject(PhysicsBody& body, LayerType layer);
    void addObject(std::shared_ptr<PhysicsBody> ptr, LayerType layer);
    void addObject(std::shared_ptr<PhysicsBody> ptr);
    PhysicsBody* getPlayer();

    void setCameraFollow(bool val);
    bool getCameraFollow();

    SpritesGlobal Sprites;
    Camera3D camera;
    GlobalTerrain terrain;
    ObjectLookup objects;
    Interface interface;

private:
    bool cameraFollow = false;
    Globals();
};

#endif // GAME_H_INCLUDED
