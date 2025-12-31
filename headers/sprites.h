#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <iostream>
#include <unordered_map>
#include <raylib.h>
#include <memory>

#include "raylib_helper.h"

struct Anime
{
    AnimeInfo info;
    Texture2D spritesheet;
};

class SpritesGlobal
{
    std::unordered_map<std::string,Texture2D> sprites;
    std::unordered_map<unsigned int,std::string> spritePaths; //map of Texture id to sprite path

    std::unordered_map<std::string,Anime> animes;

public:
    void addAnime(const AnimeInfo& info, std::string_view fullPath, std::string_view name);
    const Anime* const getAnime(std::string_view name);
    void addSprite(std::string_view fullPath, std::string_view fileName);
    void addSprites(std::string folder);
    std::string getSpritePath(Texture2D sprite);
    Texture2D getSprite(std::string_view str);
};

#endif // SPRITES_H_INCLUDED
