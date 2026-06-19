#include <iostream>
#include <fstream>
#include <filesystem>

#include "../headers/terrain.h"
#include "../headers/game.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"
#include "../headers/factory.h"
#include "../headers/audio.h"
#include "../headers/signals.h"

void StateLoader::setState(GameState newState, bool strict)
{
    if (strict)
    {
        if (state == GameState::PLAYING && newState == GameState::MAIN_MENU)
        {
            //unimplemented
        }
        else if (state == GameState::MAIN_MENU && newState == GameState::PLAYING)
        {
            Globals::Game.interface.setMenu(NONE);
            Globals::Game.setLayer(0);
            SoundLibrary::toggleBGM(true);
            Globals::Game.Camera.startQueue();
                Globals::Game.Camera.setCameraFollow(false,0);
                Globals::Game.Camera.setCameraFollow(toVector2(Globals::Game.Camera.getCamera().position));
                Globals::Game.Camera.lookAt(Globals::Game.terrain.getZOfLayer(0),300);
                Globals::Game.Camera.setCameraFollow(true,100);
                //Globals::Game.Camera.lookAt(toVector3(Globals::Game.getPlayer()->getPos()),100);
            Globals::Game.Camera.stopQueue();
            state = newState;

        }
        else if ( state == GameState::WORLD_MAP && newState == GameState::PLAYING)
        {
            //unimplemented
            state = newState;
        }
        else if (state == GameState::PLAYING && newState == GameState::WORLD_MAP )
        {
            Globals::Game.Camera.setCameraFollow(false);

            Globals::Game.Camera.moveCamera(-Globals::BACKGROUND_Z*2,60);
            auto sequence = Sequences::waitFor(std::bind(Globals::Game.Camera.isDone,&Globals::Game.Camera),false);
            sequence->push_back(RunThis([this,newState](int){
                Globals::Game.interface.setMenu(Menus::WORLD_MAP); 
                Globals::Game.Camera.setCameraFollow(true);
                state = newState;
                return true;
            }));
        }
        else
        {
            std::cerr << "ERROR StateLoader::setState: invalid transition: from " << static_cast<int>(state) << " to " << static_cast<int>(newState) << "\n";
            return;
        }
    }
    else
    {
        state = newState;
    }
}

GameState StateLoader::getState()
{
    return state;
}

Globals Globals::Game;


void Globals::init()
{

    SetExitKey(DEBUG ? KEY_ESCAPE : KEY_NULL);

    Sprites.addSprites("sprites");
    Sprites.addSprites("sprites/ui");

    Sprites.addAnime({1,30,1},"sprites/animes/death.png","death.png");
    Sprites.addAnime({1,10,10},"sprites/animes/rover2_anime.png","rover2.png");
    Sprites.addAnime({1,1,1},"sprites/animes/rover_off.png","rover_off.png");
    SoundLibrary::loadSounds("sounds");

    interface.init();

    //Globals::Game.terrain.loadTerrain("sprites/layers/level2.png");

    Player* ptr = new Player(Vector2(5500,5500));
    player.reset(ptr);
    Camera.init({Terrain::MAX_TERRAIN_SIZE,Terrain::MAX_TERRAIN_SIZE},CAMERA_Z_DISP);

}

//benchmark to test how fast terrain collision is
void benchmark()
{
    for (int i = 0; i < 150; i ++)
    {
        Vector2 start = {3000,3000};
        Vector2 end = {3000,4000};
        Globals::Game.terrain.getTerrain(0)->lineBlockIntersect(start,end,false);
    }
}

void Globals::update()
{
    if (!levelLoader.isReady())
    {
        levelLoader.monitor();
    }
    else
    {
    //SoundLibrary::update();

    float deltaTime = GetFrameTime();
     if ( IsKeyPressed(KEY_RIGHT_BRACKET) || !Debug::isPaused())
            {
            accum += deltaTime;
            Debug::clearRenderDefers();
            }

            while (accum >= tick/speed )
            {
                //player.update(*getCurrentTerrain());
                auto time = GetTime();
                terrain.update(getCurrentLayer());
                double afterUpdate = GetTime();
                Sequences::runPhysics();
                double afterPhysics = GetTime();

                std::cout << afterUpdate - time << " " << afterPhysics - afterUpdate << "\n";
            
                accum -= tick/speed;
                frames ++;
            }
            frames = 0;

            if (GetMouseWheelMove())
            {

                //camera.zoom += ((float)GetMouseWheelMove()*0.05f);
                float move = GetMouseWheelMove()*5;
                //camera.position.z += move;
                //camera.target.z += move;
                Camera.moveCamera(Camera.getCamera().position + Vector3(0,0,move));
            }
            Camera.update();
            if constexpr (Globals::DEBUG)
                Debug::handleInput();
    }
}
void Globals::render()
{
    if (levelLoader.isReady())
    {
        if (stateLoader.getState() != GameState::WORLD_MAP)
        {
        BeginMode3D(Camera.getCamera());
            ClearBackground(BLACK);

            Texture2D bg  = getBG();
            DrawBillboardRec(Camera.getCamera(),bg,Rectangle(0,0,bg.width,bg.height),
                            Vector3(Terrain::MAX_TERRAIN_SIZE/2,Terrain::MAX_TERRAIN_SIZE/2,Globals::BACKGROUND_Z),
                            Vector2(bg.width,bg.height),WHITE);

            terrain.render();

            Sequences::runRenders();
            if constexpr (DEBUG)
                Debug::renderDefers();

        EndMode3D();

        interface.process();
        Debug::drawInterface();
        }
        else
        {
            interface.process();
        }
    }
    else
    {
        Rectangle rect = {0.1*GetScreenWidth(), 0.8*GetScreenHeight(), 0.7*GetScreenWidth(),0.1*GetScreenHeight()};
        DrawRectangle(rect.x,rect.y,rect.width,rect.height,WHITE);
        DrawRectangle(rect.x,rect.y,rect.width*levelLoader.getProgress(),rect.height,RED);
    }
}

void Globals::addCollects(int val)
{
    collects += val;
}

size_t Globals::getCollects()
{
    return collects;
}

void Globals::setCollects(size_t val)
{
    collects = val;
}

void Globals::setLayer(LayerType layer)
{
    if (terrain.getTerrain(layer))
    {
        currentLayer = layer;
        if (player.get())
        {
            player->setLayer(layer);
            terrain.addObject(player,layer);
        }
        
        //Camera.lookAt(getCurrentZ(),100);

    }
    else
    {
        std::cerr << "Globals::setLayer ERROR: layer " << layer << " is invalid!\n";
    }
}


float Globals::getCurrentZ()
{
    return terrain.getZOfLayer(currentLayer);
}

Camera3D Globals::getCamera()
{
    return Camera.getCamera();
}

LayerType Globals::getCurrentLayer()
{
    return currentLayer;
}

Terrain* Globals::getCurrentTerrain()
{
    return terrain.getTerrain(getCurrentLayer());
}

void Globals::addWorld(std::string_view path)
{
    World world;
    for (const auto & entry : std::filesystem::directory_iterator(path))
   {
       std::string extension = entry.path().filename().extension().string();
       if (extension == ".png")
       {
            world.bg_path = entry.path().string();
            world.bg = LoadTexture(world.bg_path.c_str());
       }
       else if (extension == ".txt" && entry.path().filename().string().substr(0,5) == "layer")
       {
            world.layers.push_back(entry.path().string());
       }
   }

    world.signals = getWorldsSet(1);//worlds.size() - 1);
    worlds.push_back(world);

}

void Globals::onWorldLoaded()
{
    if (terrain.getLayerCount() > 0 && Globals::Game.getPlayer())
    {
        setLayer(0);
        terrain.setSignalSet(worlds[curWorld].signals);
        getPlayer()->setPos(terrain.getLayerInfo(0).playerPos);
        objects.addObject(player);
    }
}

void Globals::startLoadWorld(const World& world)
{
    terrain.clear();
    objects.clear();
    levelLoader.loadWorld(world);
}

void Globals::setCurWorldThreaded(CurrentWorld cur)
{
    if (curWorld != cur && cur < worlds.size())
    {
        terrain.setSignalSet(worlds[cur].signals);
        startLoadWorld(worlds[cur]);
    }
    curWorld = cur;
}

CurrentWorld Globals::getCurWorld()
{
    return curWorld;
}

Texture2D Globals::getBG()
{
    if (curWorld >= worlds.size())
    {
        return Globals::Game.Sprites.getSprite("bg_scatter.png");
    }
    return worlds[curWorld].bg;
}

void Globals::addObject(PhysicsBody& body, Orient o)
{
    body.setOrient(o);
    body.orient.setStartingPos(body.getPos());
    objects.addObject(body);
    terrain.addObject(objects.getObject(&body),o.layer);
    body.onAdd();
}

void Globals::addObject(PhysicsBody& body, LayerType layer)
{
    addObject(body,{body.getPos(),layer});

}

void Globals::addObject(std::shared_ptr<PhysicsBody> ptr, LayerType layer)
{
    if (ptr.get())
    {
        ptr->setOrient({ptr->getPos(),layer,ptr->orient.rotation,ptr->orient.facing});
        ptr->orient.setStartingPos(ptr->getPos());
        objects.addObject(ptr);
        terrain.addObject(ptr,layer);
        ptr->onAdd();
    }

}

void Globals::addObject(std::shared_ptr<PhysicsBody> ptr)
{
    if (ptr.get())
    {
        addObject(ptr,ptr->getOrient().layer);
    }

}

PhysicsBody* Globals::getPlayer()
{
    return player.get();
}

void Globals::setState(GameState newState)
{
    stateLoader.setState(newState,true);
}

Globals::Globals()
{

}
