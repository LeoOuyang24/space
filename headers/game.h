#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <memory>

#include <raylib.h>

#include "terrain.h"

class Player;
struct Globals
{
    static Globals Game;

    //the bigger the z, the further away from the screen
    static constexpr int MAX_Z = 1000; //furthest away something can be before going out of vision
    static constexpr int SPACE_Z = .01*MAX_Z;
    static constexpr int BACKGROUND_Z = MAX_Z - SPACE_Z;
    static constexpr int START_Z = BACKGROUND_Z - SPACE_Z*50;
    static constexpr int CAMERA_Z_DISP = START_Z; //how far the camera is at all times from getCurrentZ()

    LayerType currentLayer = 0; //layer player is at
    std::shared_ptr<Player> player;

    void init();
    void setLayer(LayerType layer); //set layer, move the player, and the camera

    float getCurrentZ();
    LayerType getCurrentLayer();
    Terrain* getCurrentTerrain();

    void addObject(PhysicsBody& body);
    void addObject(std::shared_ptr<PhysicsBody> ptr);

    template<typename Obj, typename... Ts>
    void addObject(Ts... stuff)
    {
        std::shared_ptr<PhysicsBody> made = std::make_shared<Obj>(stuff...);
        addObject(made);
    }

    Camera3D camera;
    GlobalTerrain terrain;
    ObjectLookup objects;

private:
    Globals();
};

#endif // GAME_H_INCLUDED
