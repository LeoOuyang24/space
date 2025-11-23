#include "../headers/sprites.h"

void SpritesGlobal::addSprites(std::string folder)
{
    for (auto const& file: std::filesystem::directory_iterator{folder})
        {
            if (!file.is_directory())
            {
                addSprite(file.path().filename().string());
            }
        }
}

std::string SpritesGlobal::getSpritePath(Texture2D* sprite)
{
    auto it = spritePaths.find(sprite);
    if (it != spritePaths.end())
    {
        return it->second;
    }
    return "";
}

Texture2D* SpritesGlobal::getSprite(std::string str)
{
    auto it = sprites.find(str);
    if (it != sprites.end())
    {
        return it->second.get();
    }
    std::cerr << "Error: SpritesGlobal::getSprite: unable to get sprite with path " << str << "\n";
    return nullptr;

}
