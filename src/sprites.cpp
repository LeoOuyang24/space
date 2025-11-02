#include "../headers/sprites.h"

Texture2D* SpritesGlobal::getSprite(std::string str)
{
    auto it = sprites.find(str);
    if (it != sprites.end())
    {
        return it->second.get();
    }
    return nullptr;

}
