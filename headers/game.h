#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <memory>

#include <raylib.h>

#include "terrain.h"
#include "sprites.h"
#include "interface.h"

//no idea where to put this LUL
//moves camera to look at "pos"
void moveCamera(Camera3D& camera, const Vector2& pos);

class Player;
struct Globals
{
    static Globals Game;

    //the bigger the z, the further away from the screen
    static constexpr int MAX_Z = 1000; //furthest away something can be before going out of vision
    static constexpr int SPACE_Z = .01*MAX_Z;
    static constexpr int BACKGROUND_Z = MAX_Z - SPACE_Z;
    static constexpr int START_Z = BACKGROUND_Z - SPACE_Z*50;
    static constexpr int CAMERA_Z_DISP = START_Z*0.7; //how far the camera is at all times from getCurrentZ()

    size_t collects = 0;
    LayerType currentLayer = 0; //layer player is at
    std::shared_ptr<Player> player;

    void init();
    void setLayer(LayerType layer); //set layer, move the player, and the camera

    size_t getCollects();
    void addCollects(size_t val = 1);
    void setCollects(size_t val);

    float getCurrentZ();
    LayerType getCurrentLayer();
    Terrain* getCurrentTerrain();
    void loadLevel(std::string path);

    void addObject(PhysicsBody& body);
    void addObject(std::shared_ptr<PhysicsBody> ptr);

    template<typename Obj, typename... Ts>
    void addObject(Ts... stuff)
    {
        std::shared_ptr<PhysicsBody> made = std::make_shared<Obj>(stuff...);
        addObject(made);
    }

    SpritesGlobal Sprites;
    Camera3D camera;
    GlobalTerrain terrain;
    ObjectLookup objects;
    Interface interface;

private:
    Globals();
};

#endif // GAME_H_INCLUDED
