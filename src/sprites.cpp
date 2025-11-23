#include <filesystem>

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
<<<<<<< HEAD
    std::cerr << "ERROR SpritesGlobal::getSpritePath: unable to find sprite: " << str << "\n";
=======
    std::cerr << "Error: SpritesGlobal::getSprite: unable to get sprite with path " << str << "\n";
>>>>>>> factory_new
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
