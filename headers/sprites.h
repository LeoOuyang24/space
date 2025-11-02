#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <iostream>
#include <unordered_map>
#include <raylib.h>
#include <memory>

class SpritesGlobal
{
    std::unordered_map<std::string,std::unique_ptr<Texture2D>> sprites;

public:
    void addSprite()
    {

    }
    template<typename Path, typename... Paths>
    void addSprite(Path path, Paths... paths)
    {
        Texture2D sprite = LoadTexture((std::string("sprites/") + std::string(path)).c_str());

        if (IsTextureValid(sprite))
        {
            sprites[path] = std::make_unique<Texture2D>(std::move(sprite));
        }
        else
        {
            std::cerr << "ERROR SpritesGlobal::addSprite: unable to load sprite: " << path << "\n";
        }
        addSprite(paths...);
    }

    Texture2D* getSprite(std::string str);
};

#endif // SPRITES_H_INCLUDED
