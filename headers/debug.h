#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <functional>
#include <list>
#include <iostream>

#include "raylib.h"

//handles debugging

//a tool for debugging, should probably be called DebugTool
struct DebugStruct
{
    virtual void drawInterface() = 0;
    virtual void handleInput() = 0;
};

//editor is a tool used to edit and save levels
struct Editor : public DebugStruct
{
    void drawInterface();
    void handleInput();
private:
    std::string searchText = "";
    Vector2 searchBoxPos = {0,0};
    bool activeSearch = false;
};

//Cheats allows us to "cheat" while playing the game.
//Supports different modes, which can be swapped between with the "1" key
class Cheats : public DebugStruct
{
    enum Mode
    {
        PLANETS = 0, //spawn planets
        OBJECTS, //spawn objects
        PLAYER, //move the player to mouse cursor
        ENDPOINT, //check for terrain intersection between the last place we right clicked and current mouse position
        SIZE //used to indicate size of enum
    };
    Mode mode = PLANETS;
    Vector2 endpoint = {0,0}; //used for ENDPOINT mode

    void setMode(Cheats::Mode mode);
public:
    void drawInterface();
    void handleInput();
};

//master class that controls debugging
//can swap between editor and cheats
//also allows for "DeferedRenders", which are renders that are done for debugging purposes
//can also pause/unpause the game. Debug renders will persist while game is paused
typedef std::function<void()> DeferredCall;
class Debug
{
    enum Mode
    {
        OFF, //not debugging
        CHEATING, //CTRL + C for cheats
        EDITING //CTRL + E for editor
    };
    static Mode mode;
    static DebugStruct* currentMode;
    static Cheats cheats;
    static Editor editor;

    static bool paused;


    static std::list<DeferredCall> DeferRenders; //deferred render calls

public:
    static void togglePaused();
    static bool isPaused();

    static void addDeferRender(DeferredCall call);

    static void clearRenderDefers();
    static void renderDefers();

    static void handleInput();
    static void drawInterface();


};


#endif // DEBUG_H_INCLUDED
