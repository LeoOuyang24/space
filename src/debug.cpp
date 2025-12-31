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
    Vector2 coords = screenToWorld(mousePos,Globals::Game.getCamera(),{GetScreenWidth(),GetScreenHeight()},
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
    Vector2 coords = screenToWorld(mousePos,Globals::Game.getCamera(),{GetScreenWidth(),GetScreenHeight()},
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
        else if (IsKeyPressed(KEY_TAB))
        {
            searchText += '\t';
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            auto ptr = construct(searchText);

            if (ptr.get())
            {
                ptr->orient.pos = coords;
                Globals::Game.addObject(ptr,Globals::Game.getCurrentLayer());
            }

            activeSearch = false;
            searchText = "";
        }
        else if (IsKeyPressed(KEY_BACKSPACE))
        {
            searchText = searchText.substr(0,searchText.size() - 1);
        }

    }
    //serialize a layer
    //it would also make sense for this code to be in the GlobalTerrain class
    //however, I want to keep std::couts purely for debugging purposes. I think this allows me to separate the two
    if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_CONTROL))
    {

        std::ofstream file;
        LayerType layer = Globals::Game.getCurrentLayer();
        GlobalTerrain::LayerInfo info = Globals::Game.terrain.getLayerInfo(layer);
        std::string config = info.configPath;
        if (config == "")
        {
            config = "levels/custom_layer_" + std::to_string(layer) + ".txt";
        }

        std::string imagePath = info.imagePath;
        if (imagePath == "")
        {
            imagePath = "sprites/layers/custom_layer_" + std::to_string(layer) + ".png";
        }

        std::string cereal = Globals::Game.terrain.serialize(layer);
        if (cereal.size() > 0) //if serialization is blank, we have a problem (probably somehow getCurrentLayer() returned an invalid layer)
        {
            std::cout << "SAVING config to " << config << "\n";
            file.open(config);
            file << cereal;
            file.close();
            /*
            TODO: currently saving terrain has the problem that it saves both the terrain outline and each pixel is 3 blocks big.
            std::cout << "SAVING terrain to " << imagePath << "\n";
            ExportImage(LoadImageFromTexture(Globals::Game.terrain.getLayerImage(layer)), imagePath.c_str());
            */
            std::cout << "DONE!\n";
        }
        else
        {
            std::cerr << "ERROR SERIALIZING TERRAIN!\n";
        }
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
    Vector2 mousePos = screenToWorld(GetMousePosition(),Globals::Game.getCamera(),{GetScreenWidth(),GetScreenHeight()},Globals::Game.getCurrentZ());
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
                    Globals::Game.addObject(*(new Object<RectCollider,
                                              ShapeRenderer<ShapeType::RECT>,EMPTY_TYPE>(
                                                {mousePos},
                                                std::make_tuple(10,10),{})),Globals::Game.getCurrentLayer());
                else
                    Globals::Game.addObject(*(new Object<CircleCollider,
                                              ShapeRenderer<ShapeType::CIRCLE>,EMPTY_TYPE>(
                                                {mousePos},
                                                std::make_tuple(10),{})),Globals::Game.getCurrentLayer());
                break;
            }
        case PLAYER:
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                Globals::Game.player->setPos(mousePos);
                if (Globals::Game.player->getDead())
                {
                    Globals::Game.player->setDead(false);
                    Globals::Game.terrain.addObject(Globals::Game.player,Globals::Game.getCurrentLayer());
                }
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

            Vector2 onTerrain = Globals::Game.getCurrentTerrain()->lineTerrainIntersect(mousePos,endpoint,false);
            DrawCircle3D(Vector3(onTerrain.x,onTerrain.y,z),3,{0,1,0},0,PURPLE);

            LayerType layer = Globals::Game.getCurrentLayer();
            Globals::Game.terrain.getTerrain(layer)->forEachPos([layer](const Vector2& pos){
                                Terrain* terr = Globals::Game.terrain.getTerrain(layer);
                                Vector2 rounded = terr->roundPos(pos);
                                DrawCube({rounded.x + Block::BLOCK_DIMEN/2.0,rounded.y + Block::BLOCK_DIMEN/2.0,Globals::Game.getCurrentZ()},
                                         Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,0,
                                         terr->blockExists(pos,false) ? RED : BLUE);
                                },
                                screenToWorld(GetMousePosition(),
                                              Globals::Game.getCamera(),
                                              {GetScreenWidth(),GetScreenHeight()},
                                              Globals::Game.getCurrentZ()),2*Block::BLOCK_DIMEN);
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
        else if (IsKeyPressed(KEY_D))
        {
            currentMode = nullptr;
        }
    }
    else if (IsKeyPressed(KEY_EQUAL))
    {
        Globals::Game.setLayer(Globals::Game.getCurrentLayer() + 1);
    }
    else if (IsKeyPressed(KEY_MINUS))
    {
        Globals::Game.setLayer(Globals::Game.getCurrentLayer() - 1);
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
    Vector2 screenDimen = {GetScreenWidth(),GetScreenHeight()};

    if (currentMode)
    {
        currentMode->drawInterface();
        if (GetMousePosition().x >= 0.9*screenDimen.x)
        {
            Globals::Game.moveCamera(Globals::Game.getCamera().position + Vector3(10,0,0));
        }
        else if (GetMousePosition().x <= 0.1*screenDimen.x)
        {
            Globals::Game.moveCamera(Globals::Game.getCamera().position + Vector3(-10,0,0));
        }

        if (GetMousePosition().y >= 0.9*screenDimen.y)
        {
            Globals::Game.moveCamera(Globals::Game.getCamera().position + Vector3(0,10,0));
        }
        else if (GetMousePosition().y <= 0.1*screenDimen.y)
        {
            Globals::Game.moveCamera(Globals::Game.getCamera().position + Vector3(0,-10,0));
        }
    }
}

bool Debug::isDebugOn()
{
    return currentMode;
}
