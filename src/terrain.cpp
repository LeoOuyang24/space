#include <sstream>
#include <fstream>
#include <algorithm>

#include "../headers/terrain.h"
#include "../headers/game.h"
#include "../headers/objects.h"
#include "../headers/player.h"


void ObjectLookup::addObject(PhysicsBody& body)
{
    if (objects.find(&body) == objects.end())
    {
        objects[&body].reset(&body);
    }
}

void ObjectLookup::addObject(std::shared_ptr<PhysicsBody> ptr)
{
    if (ptr.get() && objects.find(ptr.get()) == objects.end())
    {
        objects[ptr.get()] = ptr;
    }
}

void ObjectLookup::eraseObject(PhysicsBody& obj)
{
    auto it = objects.find(&obj);
    if (it != objects.end())
    {
        objects.erase(it);
    }
}

std::shared_ptr<PhysicsBody> ObjectLookup::getObject(PhysicsBody* body)
{
    auto it = objects.find(body);
    if(it == objects.end())
    {
        return std::shared_ptr<PhysicsBody>();
    }
    return it->second;
}

void ObjectLookup::clear()
{
    objects.clear();
}

bool GlobalTerrain::isValidObject(PhysicsBody* obj, LayerType layer)
{
    return obj && obj->orient.layer == layer && !obj->isDead();
}

void GlobalTerrain::addObject(std::shared_ptr<PhysicsBody> ptr, LayerType layer)
{
    if (ptr.get() && layer < layers.size())
    {
        layers[layer].objects.insert(ptr);
    }
}

void GlobalTerrain::moveObject(std::shared_ptr<PhysicsBody> obj, LayerType layer)
{
    addObject(obj,layer);
}

void GlobalTerrain::pushBackTerrain()
{
    layers.emplace_back();
}

void GlobalTerrain::loadTerrain(LayerType layer, const Image& img)
{
    if (layer >= layers.size())
    {
        layers.resize(layer + 1);
    }
    if (!IsImageValid(img))
    {
        std::cerr << "ERROR : GlobalTerrain::loadTerrain: invalid image for layer " << layer << "\n";
        return;
    }
    Terrain* terr = getTerrain(layer);

    Color* colors = LoadImageColors(img);

    terr->blocksTexture.texture = LoadTextureFromImage(img);

    for (int i = 0; i < std::min(img.width,Terrain::MAX_WIDTH); i += 1)
    {
        for (int j = 0; j < std::min(img.height,Terrain::MAX_WIDTH); j += 1)
        {

            Vector2 point = {i*Block::BLOCK_DIMEN,j*Block::BLOCK_DIMEN};

            Color color = GetImageColor(img,i,j);

            BlockType type = SOLID;

            if (color.r == 255 && color.g == 255 && color.b == 255)
            {
                type = ANTI;
            }
            else if (color.r == 255 && color.g == 0 && color.b == 0)
            {
                type = LAVA;
            }
            else if (color.r == 0 && color.g == 0 && color.b == 255)
            {
                type = WATER;
            }
            if (color.a > 0)
            {
                terr->addBlock(point,{color,type},false);
            }
        }
    }

    delete[] colors;

}
void GlobalTerrain::setLayerInfo(LayerType layer, const LayerInfo& info)
{
    if (layer < layers.size()) [[likely]]
    {
        layers[layer].info = info;
    }
}

LayerType GlobalTerrain::getLayerCount()
{
    return layers.size();
}

Terrain* GlobalTerrain::getTerrain(LayerType layer)
{
    return (layer >= layers.size()) ? nullptr : &layers[layer].terrain;
}

void GlobalTerrain::update(LayerType layer)
{
    if (layer < layers.size())
    {
        auto& objects = layers[layer].objects;
        for (auto it = objects.begin(); it != objects.end();)
        {
            PhysicsBody* obj = it->lock().get();
            if (isValidObject(obj,layer)) //if object is non-null and in this layer and not dead, update it!
            {
                obj->update(*getTerrain(layer));
                if (obj->isTangible())
                {
                    for (auto jt = objects.begin(); jt != it; ++jt)
                    {
                        PhysicsBody* obj2 = jt->lock().get();
                        if (isValidObject(obj2,layer) && obj2->isTangible() && CheckCollision(obj->getShape(),obj2->getShape()))
                        {
                            obj->onCollide(*obj2);
                            obj2->onCollide(*obj);
                        }
                    }
                }
                ++it;
            }
            else if (obj == Globals::Game.getPlayer() && obj && obj->getDead()) //player gets reset as opposed to removed
            {
                static_cast<Player*>(obj)->resetPlayer();
            }
            else //otherwise, remove it
            {
                it = objects.erase(it);
                Globals::Game.objects.eraseObject(*obj);
            }
        }
    }
}

void GlobalTerrain::render()
{
    //current layer
    int limit = std::min(layers.size() - 1,std::max(static_cast<LayerType>(0),Globals::Game.getCurrentLayer()));
    //fun fact! -1 >= (sizeof)0 <-- this is true! So "limit" actually has to be an int. Otherwise, even if "i" goes negative, i
    //will still be "larger" than size_t.
    //Alternatively, we can make both "limit" and "i" a size_t and then check if i > layers.size(), but that seems unintuitive


    //render layers from back all the way to front, not including layers past the camera
    for (int i = layers.size() - 1; i >= limit ;i--)
    {
        layers[i].terrain.render(i - Globals::Game.getCurrentLayer(),getZOfLayer(i));

    }

    //only render entities of the current layer
    for (auto it = layers[limit].objects.begin(); it != layers[limit].objects.end(); ++it)
    {
        if (PhysicsBody* obj = it->lock().get()) [[likely]] //highly likely since update removes bad pointers and it always runs first
        {
            obj->render();
        }
    }

}

void GlobalTerrain::clear()
{
    for (auto& layer: layers)
    {
        layer.terrain.cleanUp();
        layer.objects.clear();
    }
}

int GlobalTerrain::getZOfLayer(LayerType index)
{
    if (index >= layers.size()) [[unlikely]]
    {
        return -1;
    }
    return (Globals::START_Z + index*static_cast<float>(Globals::BACKGROUND_Z - Globals::START_Z)/(layers.size()));
}

Vector3 GlobalTerrain::orientToVec3(const Orient& orient)
{
    return {orient.pos.x,orient.pos.y,getZOfLayer(orient.layer)};
}

GlobalTerrain::LayerInfo GlobalTerrain::getLayerInfo(LayerType index)
{
    if (index >= layers.size()) [[unlikely]]
    {
        return {};
    }
    return layers[index].info;
}
std::string GlobalTerrain::serialize(LayerType index)
{
    if (index >= layers.size()) [[unlikely]]
    {
        return "";
    }
    std::string cereal = "";
    Layer& layer = layers[index];
    cereal += layer.info.imagePath + "\n" + toString(layer.info.playerPos) + "\n";
    for (auto it = layer.objects.begin(); it != layer.objects.end(); ++it)
    {
        PhysicsBody* obj = it->lock().get();
        if (isValidObject(obj,index))
        {
            cereal += obj->serialize() + "\n";
        }
    }
    return cereal;
}

void GlobalTerrain::flipGravity()
{
    gravityMode = (gravityMode == DOWN ? PLANET : DOWN);
}

void GlobalTerrain::setSignalSet(SignalSet& set)
{
    signals = &set;
}

void GlobalTerrain::emitSignal(SignalName name, void* data)
{
    if (signals && signals->find(name) != signals->end())
    {
        (*signals)[name](data);
    }
}

void LevelLoader::loadWorld(const World& world)
{
    ready = false;
    loaded = 0;

    size_t layers = world.layers.size();
    threads.resize(layers);
    preloads.resize(layers);

    for (size_t i = 0; i < layers; i ++)
    {
        threads[i] = std::jthread(&LevelLoader::loadPreLayer,this,std::ref(preloads[i]),world.layers[i]);
    }
}

bool LevelLoader::getReady()
{
    return ready;
}

LevelLoader::PreLayer LevelLoader::getLoadedLayer(size_t i)
{
    if (i >= preloads.size())
    {
        return {};
    }
    else
    {
        return preloads[i];
    }
} 

void LevelLoader::monitor()
{
    if (!getIsLoading())
    {
        for (size_t i = 0; i < preloads.size(); i ++)
        {
            Globals::Game.terrain.loadTerrain(i,preloads[i].blocks);
            Globals::Game.terrain.setLayerInfo(i,preloads[i].info);
            std::for_each(preloads[i].objs.begin(),preloads[i].objs.end(),[i](std::shared_ptr<PhysicsBody>& ptr)
            {
                Globals::Game.addObject(ptr,i);
            });
        }
        Globals::Game.onWorldLoaded();
        clear();
        ready = true;
    }
}


void LevelLoader::loadPreLayer(PreLayer& preloaded, std::string layerPath)
{
    std::ifstream levelFile;
    levelFile.open(layerPath.data());

    if (levelFile.is_open())
    {
        std::string line;
        int lineNum = 0;
        preloaded.info = {layerPath};
        while(std::getline(levelFile,line))
        {
            if (line != "") //skip blank lines
            {
                switch (lineNum)
                {
                case 0: //first line is terrain image
                    {
                        preloaded.info.imagePath = line;
                        Image img = LoadImage(line.c_str());
                        preloaded.blocks = img;
                        break;
                    }
                case 1: //2nd line is player position
                    {
                        Vector2 pos = fromString<Vector2>(line);
                        preloaded.info.playerPos = pos;
                        break;
                    }
                default:
                    preloaded.objs.push_back(ClassDeserializer::construct(line));
                    break;
                }
                lineNum ++;
            }
        }
        loaded++;
        levelFile.close();
    }

}

bool LevelLoader::getIsLoading()
{
    return loaded < preloads.size();
}

void LevelLoader::clear()
{
    for (auto& pre : preloads)
    {
        pre.info = {};
        pre.objs.clear();
        if (IsImageValid(pre.blocks))
        {
            UnloadImage(pre.blocks);
        }
    }
}