#include <fstream>

#include "../headers/debug.h"
#include "../headers/game.h"
#include "../headers/factory.h"
#include "../headers/objects.h"
#include "../headers/render.h"
#include "../headers/player.h"

void Editor::drawInterface()
{
    Vector2 mousePos = GetMousePosition();
    Vector2 coords = screenToWorld(mousePos,Globals::Game.camera,{GetScreenWidth(),GetScreenHeight()},
                                    Globals::Game.getCurrentZ());
    DrawText((std::to_string(int(coords.x)) + " " + std::to_string(int(coords.y))).c_str(),mousePos.x,mousePos.y - 20,20,WHITE);
    DrawText("EDITOR",10,50,30,BLUE);

    if (activeSearch)
    {
        DrawText(searchText.c_str(),searchBoxPos.x,searchBoxPos.y,20,WHITE);
    }
}

void Editor::handleInput()
{
    Vector2 mousePos = GetMousePosition();
    Vector2 coords = screenToWorld(mousePos,Globals::Game.camera,{GetScreenWidth(),GetScreenHeight()},
                                    Globals::Game.getCurrentZ());
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        searchBoxPos = GetMousePosition();
        activeSearch = true;
    }

    if (activeSearch)
    {
        int c = GetCharPressed();
        if (c)
        {
            searchText += c;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            auto ptr = construct(searchText);

            if (ptr.get())
            {
                ptr->orient.pos = coords;
                ptr->orient.layer = Globals::Game.getCurrentLayer();
                Globals::Game.addObject(ptr);
            }

            activeSearch = false;
            searchText = "";
        }
        else if (IsKeyPressed(KEY_BACKSPACE))
        {
            searchText = searchText.substr(0,searchText.size() - 1);
        }

    }

    if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_CONTROL))
    {
        std::cout << "SAVING!\n";
        std::ofstream file;
        file.open("levels/custom.txt");
        std::string cereal = Globals::Game.terrain.serialize(0);
        file << cereal;
        file.close();
    }

}


void Cheats::setMode(Cheats::Mode mode_)
{
    mode = mode_;
}

void Cheats::drawInterface()
{
    std::string modeString = "";
    switch (mode)
    {
    case PLANETS:
        modeString = "PLANETS";
        break;
    case OBJECTS:
        modeString = "OBJECTS";
        break;
    case PLAYER:
        modeString = "PLAYER";
        break;
    case ENDPOINT:
        modeString = "ENDPOINT";
        break;
    default:
        modeString = "PLANETS";
        setMode(PLANETS);
    }

    DrawText(("CHEATS: " + modeString).c_str(),10,50,30,RED);
}

void Cheats::handleInput()
{
    Vector2 mousePos = screenToWorld(GetMousePosition(),Globals::Game.camera,{GetScreenWidth(),GetScreenHeight()},Globals::Game.getCurrentZ());
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        switch (mode)
        {
        case PLANETS:
            Globals::Game.getCurrentTerrain()->generatePlanet(mousePos,50,Color(100,255,100,255 ));
            break;
        case OBJECTS:
            {
                Color color = {255,255,255,100};
                if (rand()%2)
                    Globals::Game.addObject(*(new Object<RectCollider,ShapeRenderer<ShapeType::RECT>>({mousePos,Globals::Game.getCurrentLayer()},color,10,10)));
                else
                    Globals::Game.addObject(*(new Object<CircleCollider,ShapeRenderer<ShapeType::CIRCLE>>({mousePos,Globals::Game.getCurrentLayer()},color,10)));
                break;
            }
        case PLAYER:
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                Globals::Game.player->orient.pos = mousePos;
            }
            break;
        case ENDPOINT:
            endpoint = mousePos;
            break;

        }
    }
    else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        Globals::Game.getCurrentTerrain()->remove(mousePos,50);
    }
    else if (IsKeyPressed(KEY_ONE))
    {
       setMode(static_cast<Mode>(((int)mode + 1)%Mode::SIZE));
    }

    if (mode == ENDPOINT)
    {
        Debug::addDeferRender([this,mousePos](){

            float z = Globals::Game.getCurrentZ();
            DrawCircle3D(Vector3(endpoint.x,endpoint.y,z),3,{0,1,0},0,PURPLE);
            DrawLine3D(Vector3(mousePos.x,mousePos.y,z),Vector3(endpoint.x,endpoint.y,z),PURPLE);

            Vector2 onTerrain = Globals::Game.getCurrentTerrain()->lineTerrainIntersect(mousePos,endpoint).pos;
            //DrawCircle3D(Vector3(onTerrain.x,onTerrain.y,z),3,{0,1,0},0,BLACK);
            DrawSphere(Vector3(onTerrain.x,onTerrain.y,z),5,BLACK);

                      });
    }

}

std::list<DeferredCall> Debug::DeferRenders;
Debug::Mode Debug::mode = OFF;
bool Debug::paused = false;
DebugStruct* Debug::currentMode = nullptr;
Cheats Debug::cheats;
Editor Debug::editor;


void Debug::togglePaused()
{
    paused = !paused;
}

bool Debug::isPaused()
{
    return paused;
}


void Debug::addDeferRender(DeferredCall call)
{
    DeferRenders.push_back(call);
}

void Debug::renderDefers()
{
    for (auto func : DeferRenders)
    {
        func();
    }
}

void Debug::clearRenderDefers()
{
    DeferRenders.clear();
}

void Debug::handleInput()
{
    if (IsKeyPressed(KEY_BACKSLASH))
    {
       togglePaused();
    }
    else if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_C))
        {
            currentMode = &cheats;
        }
        else if (IsKeyPressed(KEY_E))
        {
            currentMode = &editor;
        }
    }
    if (currentMode)
    {
        currentMode->handleInput();
    }
}

void Debug::drawInterface()
{
    if (paused)
    {
        DrawText("PAUSED",GetScreenWidth()*.9,10,20,WHITE);
    }
    if (currentMode)
    {
        currentMode->drawInterface();
    }
}
