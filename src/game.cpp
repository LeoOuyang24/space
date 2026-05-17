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

    interface.init();

    //Globals::Game.terrain.loadTerrain("sprites/layers/level2.png");

    Player* ptr = new Player(Vector2(5500,5500));
    player.reset(ptr);
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

    worlds.push_back(world);
    world.signals = getWorldsSet(1);//worlds.size() - 1);
}

void Globals::onWorldLoaded()
{
    if (terrain.getLayerCount() > 0 && Globals::Game.getPlayer())
    {
        Globals::Game.setLayer(0);
        Globals::Game.getPlayer()->setPos(terrain.getLayerInfo(0).playerPos);
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
        terrain.setSignalSet(worlds[curWorld].signals);
        startLoadWorld(worlds[cur]);
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
        ptr->setOrient({ptr->getPos() + Vector2(1500,1500),layer,ptr->orient.rotation,ptr->orient.facing});
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
