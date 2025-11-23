#include <filesystem>

#include "../headers/sprites.h"

Texture2D* SpritesGlobal::getSprite(std::string str)
{
    auto it = sprites.find(str);
    if (it != sprites.end())
    {
        return it->second.get();
    }
    std::cerr << "ERROR SpritesGlobal::getSpritePath: unable to find sprite: " << str << "\n";
    return nullptr;

}

std::string SpritesGlobal::getSpritePath(Texture2D* sprite)
{
    auto it = paths.find(sprite);
    if (it != paths.end())
    {
        return it->second;
    }
    return "";
}

void SpritesGlobal::addSprites(std::string folderPath)
{
   for (const auto & entry : std::filesystem::directory_iterator(folderPath))
   {
       if (!std::filesystem::is_directory(entry.path()))
       {
            addSprite(entry.path().filename().string());
       }
   }
}
