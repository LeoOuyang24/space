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
    static constexpr bool DEBUG = false; //set to true for debug mode
    static constexpr Vector2 screenDimen = DEBUG ? Vector2{900,900} : Vector2{1920,1080};

    //the bigger the z, the further away from the screen
    static constexpr int MAX_Z = 1000; //furthest away something can be before going out of vision
    static constexpr int SPACE_Z = .01*MAX_Z; //distance between layers if there are 50 layers
    static constexpr int BACKGROUND_Z = MAX_Z - SPACE_Z; //z coordinate of background
    static constexpr int START_Z = BACKGROUND_Z - SPACE_Z*50;
    static constexpr int CAMERA_Z_DISP = START_Z*0.8; //how far the camera is at all times from getCurrentZ()



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

    void moveCamera(const Vector3& pos);
    void moveCamera(const Vector2& pos);
    void moveCamera(float z); //set position to z
    void lookAt(float z); //set target to z
    const Camera3D& getCamera();

    SpritesGlobal Sprites;
    GlobalTerrain terrain;
    ObjectLookup objects;
    Interface interface;

private:
    bool cameraFollow = false;
    Camera3D camera;

    Globals();
};

#endif // GAME_H_INCLUDED
