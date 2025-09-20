#include "../headers/terrain.h"
#include "../headers/game.h"
#include "../headers/objects.h"

void Layer::update()
{
    for (int i = 0; i < objects.size(); i ++)
    {
        for (int j = 0; j < i; j ++)
        {
            if (CheckCollision(objects[i]->getShape(),objects[j]->getShape()))
            {
                objects[i]->collideWith(*objects[j]);
                objects[j]->collideWith(*objects[i]);
            }
        }
    }
}

void Layer::render(float z)
{
    terrain.render(z);
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->render();
    }
}

void GlobalTerrain::addObject(PhysicsBody& body, LayerType layer)
{
    if (layer >= layers.size())
    {
        return;
    }

    layers[layer].objects.emplace_back(&body);
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
        layers[layer].update();
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
        layers[i].render(getZOfLayer(i));
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
