#include <filesystem>

#include "../headers/sprites.h"

void SpritesGlobal::addAnime(const AnimeInfo& info, std::string_view fullPath, std::string_view fileName)
{

    Texture2D sprite = LoadTexture(fullPath.data());
    if (IsTextureValid(sprite))
    {
        animes[fileName.data()] = {info,sprite};
    }
    else
    {
        std::cerr << "ERROR SpritesGlobal::addSprite: unable to load sprite: " << fullPath << "\n";
        return;
    }
}

const Anime* const SpritesGlobal::getAnime(std::string_view name)
{
    auto it = animes.find(name.data());
    if (it != animes.end())
    {
        return &it->second;
    }
    std::cerr << "ERROR SpritesGlobal::getAnime: unable to find sprite: " << name << "\n";
    return nullptr;
}

void SpritesGlobal::addSprite(std::string_view fullPath, std::string_view fileName)
{

    Texture2D sprite = LoadTexture(fullPath.data());
    if (IsTextureValid(sprite))
    {
        sprites[fileName.data()] = sprite;
        spritePaths[sprite.id] = fullPath;
    }
    else
    {
        std::cerr << "ERROR SpritesGlobal::addSprite: unable to load sprite: " << fullPath << "\n";
        return;
    }
}

void SpritesGlobal::addSprites(std::string folderPath)
{
   for (const auto & entry : std::filesystem::directory_iterator(folderPath))
   {
       if (!std::filesystem::is_directory(entry.path()))
       {
            addSprite(entry.path().string(),entry.path().filename().string());
       }
   }
}

Texture2D SpritesGlobal::getSprite(std::string_view str)
{
    auto it = sprites.find(str.data());
    if (it != sprites.end())
    {
        return it->second;
    }
    std::cerr << "ERROR SpritesGlobal::getSpritePath: unable to find sprite: " << str << "\n";
    return {};

}

std::string SpritesGlobal::getSpritePath(Texture2D sprite)
{

    auto it = spritePaths.find(sprite.id);
    if (it != spritePaths.end())
    {
        return it->second;
    }
    return "";
}
