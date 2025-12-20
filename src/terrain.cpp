#include <sstream>

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
std::shared_ptr<PhysicsBody> ObjectLookup::getObject(PhysicsBody* body)
{
    auto it = objects.find(body);
    if(it == objects.end())
    {
        return std::shared_ptr<PhysicsBody>();
    }
    return it->second;
}

bool GlobalTerrain::isValidObject(PhysicsBody* obj, LayerType layer)
{
    return obj && obj->orient.layer == layer && !obj->isDead();
}

void GlobalTerrain::addObject(std::shared_ptr<PhysicsBody> ptr, LayerType layer)
{
    if (ptr.get() && layer < layers.size())
    {
        layers[layer].objects.emplace_back(ptr);
    }
}

void GlobalTerrain::pushBackTerrain()
{
    layers.emplace_back();
}

void GlobalTerrain::loadTerrain(LayerType layer, std::string imagePath)
{
    if (layer >= layers.size())
    {
        pushBackTerrain();
        layer = layers.size() - 1;
    }
    else
    {
        getTerrain(layer)->clear();
    }
    Terrain* terr = getTerrain(layer);

    layers[layer].info.imagePath = imagePath;

    Image img = LoadImage(imagePath.c_str());
    Color* colors = LoadImageColors(img);
    for (int i = 0; i < std::min(img.width,Terrain::MAX_WIDTH); i += 1)
    {
        for (int j = 0; j < std::min(img.height,Terrain::MAX_WIDTH); j += 1)
        {
            Vector2 point = {i*Block::BLOCK_DIMEN,j*Block::BLOCK_DIMEN};
            int index = terr->pointToIndex(point);

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
            if (color.a > 0)
                {
                    terr->addBlock(point,{color,type});
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
                for (auto jt = objects.begin(); jt != it; ++jt)
                {
                    PhysicsBody* obj2 = jt->lock().get(); //guaranteed to be non-null and in this layer, since jt < it;
                    if (CheckCollision(obj->getShape(),obj2->getShape()))
                    {
                        obj->onCollide(*obj2);
                        obj2->onCollide(*obj);
                    }
                }
                ++it;
            }
            else if (obj->getDead() && obj == Globals::Game.getPlayer()) //player gets reset as opposed to removed
            {
                static_cast<Player*>(obj)->resetPlayer();
            }
            else //otherwise, remove it
            {
                it = objects.erase(it);
            }
        }
    }
}

void GlobalTerrain::render()
{
    //render layers from back all the way to front, not including layers past the camera
    int limit = std::min(layers.size() - 1,std::max(static_cast<LayerType>(0),Globals::Game.getCurrentLayer()));
    //fun fact! -1 >= (sizeof)0 <-- this is true! So "limit" actually has to be an int. Otherwise, even if "i" goes negative, i
    //will still be "larger" than size_t.
    //Alternatively, we can make both "limit" and "i" a size_t and then check if i > layers.size(), but that seems unintuitive
    for (int i = layers.size() - 1; i >= limit ;i--)
    {
        float z = getZOfLayer(i);

        layers[i].terrain.render(z);
        for (auto it = layers[i].objects.begin(); it != layers[i].objects.end(); ++it)
        {
            if (PhysicsBody* obj = it->lock().get()) [[likely]] //highly likely since update removes bad pointers and it always runs first
            {
                obj->render();
            }
        }
    }
}

float GlobalTerrain::getZOfLayer(LayerType index)
{
    if (index >= layers.size()) [[unlikely]]
    {
        return -1;
    }
    return Globals::START_Z + index*static_cast<float>(Globals::BACKGROUND_Z - Globals::START_Z)/(layers.size());
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
