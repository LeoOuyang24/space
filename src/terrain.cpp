#include "../headers/terrain.h"
#include "../headers/game.h"
#include "../headers/objects.h"


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
std::shared_ptr<PhysicsBody> ObjectLookup::getObject(PhysicsBody& body)
{
    auto it = objects.find(&body);
    if(it == objects.end())
    {
        return std::shared_ptr<PhysicsBody>();
    }
    return it->second;
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

void GlobalTerrain::loadTerrain(std::string imagePath, LayerType layer)
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

    Image img = LoadImage(imagePath.c_str());
    Color* colors = LoadImageColors(img);
    Color c = GetImageColor(img,424,700);
    for (int i = 0; i < std::min(img.width,Terrain::MAX_WIDTH); i += 1)
    {
        for (int j = 0; j < std::min(img.height,Terrain::MAX_WIDTH); j += 1)
        {
            Vector2 point = {i*Block::BLOCK_DIMEN,j*Block::BLOCK_DIMEN};
            int index = terr->pointToIndex(point);

            Color color = GetImageColor(img,i,j);
            if (color.a > 0)
                {
                    terr->addBlock(point,{color});
                }

        }
    }
    delete[] colors;


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
            if (obj && obj->orient.layer == layer) //if object is non-null and in this layer, update it!
            {
                obj->update(*getTerrain(layer));
                for (auto jt = objects.begin(); jt != it; ++jt)
                {
                    PhysicsBody* obj2 = jt->lock().get(); //guaranteed to be non-null and in this layer, since jt < it;
                    if (CheckCollision(obj->getShape(),obj2->getShape()))
                    {
                        obj->collideWith(*obj2);
                        obj2->collideWith(*obj);
                    }
                }
                ++it;
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
