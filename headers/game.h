#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <raylib.h>

struct Globals
{
    static Globals Game;
    static constexpr int MAX_Z = 1000; //furthest away something can be before going out of vision
    static constexpr int SPACE_Z = .01*MAX_Z;
    static constexpr int BACKGROUND_Z = MAX_Z - SPACE_Z;
    static constexpr int START_Z = BACKGROUND_Z - SPACE_Z*50;

    int currentZ = START_Z;


    Camera3D camera;


private:
    Globals()
    {

    }
};

#endif // GAME_H_INCLUDED
