#include <iostream>
#include <fstream>
#include <filesystem>

#include "../headers/terrain.h"
#include "../headers/game.h"
#include "../headers/sequencer.h"
#include "../headers/player.h"
#include "../headers/factory.h"
#include "../headers/audio.h"

Globals Globals::Game;

void Globals::init()
{

    SetExitKey(DEBUG ? KEY_ESCAPE : KEY_NULL);

    Sprites.addSprites("sprites");
    Sprites.addSprites("sprites/ui");
    Sprites.addAnime({1,10,10},"sprites/animes/rover2_anime.png","rover2.png");
    Sprites.addAnime({1,1,1},"sprites/animes/rover_off.png","rover_off.png");
    SoundLibrary::loadSounds("sounds");

    //Globals::Game.terrain.loadTerrain("sprites/layers/level2.png");

    Player* ptr = new Player(Vector2(5500,5500));
    player.reset(ptr);
    objects.addObject(player);


   // setLayer(0);

    camera.projection = CAMERA_PERSPECTIVE;
    camera.position = Vector3(0,0,getCurrentZ() - Globals::CAMERA_Z_DISP);
    camera.target = Vector3(0,0,Globals::BACKGROUND_Z);
    //camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    //camera.rotation = 0.0f;
    //camera.zoom = 1.0f;
    camera.up = {0,-1,0};
    camera.fovy = 90;

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

        Sequences::add({[this,endZ=getCurrentZ(),startZ = camera.target.z](int runTimes){

                       lookAt(Lerp(startZ,endZ,runTimes/50.0));
                       return runTimes >= 50;

                       }},false);
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
                    addObject(construct(line),terrain.getLayerCount() - 1);
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
    for (const auto & entry : std::filesystem::directory_iterator(path))
   {
       std::string extension = entry.path().filename().extension().string();
       if (extension == ".png")
       {
            world.bg_path = entry.path().string();
            world.bg = LoadTexture(world.bg_path.c_str());
       }
       else if (extension == ".txt")
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
    }

    curWorld = cur;


}

Texture2D Globals::getBG()
{
    if (curWorld >= worlds.size())
    {
        std::cerr << "Globals::getBG ERROR: cannot return background of a world at index " << curWorld << "\n";
        return {};
    }
    return worlds[curWorld].bg;
}

void Globals::addObject(PhysicsBody& body, LayerType layer)
{
    body.setOrient({body.getPos(),layer});
    objects.addObject(body);
    terrain.addObject(objects.getObject(&body),layer);
}

void Globals::addObject(std::shared_ptr<PhysicsBody> ptr, LayerType layer)
{
    if (ptr.get())
    {
        ptr->setOrient({ptr->getPos(),layer});
        objects.addObject(ptr);
        terrain.addObject(ptr,layer);
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

void Globals::setCameraFollow(bool val)
{
    cameraFollow = val;
}

bool Globals::getCameraFollow()
{
    return cameraFollow;
}


void Globals::moveCamera(const Vector3& pos)
{
    moveCamera(Vector2{pos.x,pos.y});

    camera.position.z = pos.z ;
    camera.target.z = pos.z + Globals::CAMERA_Z_DISP;

}

void Globals::moveCamera(const Vector2& pos)
{
    float disp = Globals::CAMERA_Z_DISP*tan(camera.fovy/2*DEG2RAD); //distance from the edge of the screen

    //clamps camera to level area
    Vector2 clampedPos = {
        Clamp(pos.x,disp,Terrain::MAX_TERRAIN_SIZE - disp),
        Clamp(pos.y,disp,Terrain::MAX_TERRAIN_SIZE - disp)
    };
    assignVector(camera.position,clampedPos);
    assignVector(camera.target,clampedPos);
}

void Globals::moveCamera(float z)
{
    moveCamera(Vector3(camera.position.x,camera.position.y,z));
}

void Globals::lookAt(float z)
{
    moveCamera(z - Globals::CAMERA_Z_DISP);
}

const Camera3D& Globals::getCamera()
{
    return camera;
}

Globals::Globals()
{

}
