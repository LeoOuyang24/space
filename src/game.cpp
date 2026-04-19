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

    //Globals::Game.terrain.loadTerrain("sprites/layers/level2.png");

    Player* ptr = new Player(Vector2(5500,5500));
    player.reset(ptr);
    objects.addObject(player);


    Camera.init();

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
        if (player.get() && player->orient.layer != layer)
        {
            player->orient.layer = layer;
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

void Globals::loadLevel(std::string_view path)
{
    std::ifstream levelFile;
    levelFile.open(path.data());

    if (levelFile.is_open())
    {
        std::string line;
        int lineNum = 0;
        GlobalTerrain::LayerInfo info = {path.data()};
        while(std::getline(levelFile,line))
        {
            if (line != "") //skip blank lines
            {
                switch (lineNum)
                {
                case 0: //first line is terrain image
                    info.imagePath = line;
                    terrain.loadTerrain(-1,line);
                    break;
                case 1: //2nd line is player position
                    {
                        Vector2 pos = fromString<Vector2>(line);
                        info.playerPos = pos;
                        if (terrain.getLayerCount() == 1) //first layer
                        {
                            Globals::Game.player->setPos(pos);
                        }
                        break;
                    }
                default:
                    addObject(ClassDeserializer::construct(line),terrain.getLayerCount() - 1);
                    break;
                }
                lineNum ++;
            }
        }
        terrain.setLayerInfo(terrain.getLayerCount() - 1,info);

        levelFile.close();
    }
    else
    {
        std::cerr << "ERROR Globals::loadLevel: error loading level: " << path << "\n";
    }
}

void Globals::addWorld(std::string_view path)
{
    World world;
    world.signals = getWorldsSet(1);
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

   worlds.push_back(world);
}

void Globals::loadWorld(const World& world)
{
    terrain.clear();
    objects.clear();

    for (std::string_view str : world.layers)
    {
        loadLevel(str);
    }
}

void Globals::setCurWorld(CurrentWorld cur)
{
    if (curWorld != cur && cur < worlds.size())
    {
        loadWorld(worlds[cur]);
        terrain.setSignalSet(worlds[cur].signals);
    }
    curWorld = cur;


}

Texture2D Globals::getBG()
{
    if (curWorld >= worlds.size())
    {
        return Globals::Game.Sprites.getSprite("bg_scatter.png");
    }
    return worlds[curWorld].bg;
}

void Globals::addObject(PhysicsBody& body, LayerType layer)
{
    body.setOrient({body.getPos(),layer});
    body.orient.setStartingPos(body.getPos());
    objects.addObject(body);
    terrain.addObject(objects.getObject(&body),layer);
    body.onAdd();
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


Globals::Globals()
{

}
