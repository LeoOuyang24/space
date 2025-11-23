#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <iostream>
#include <unordered_map>
#include <raylib.h>
#include <memory>

class SpritesGlobal
{
    std::unordered_map<std::string,std::unique_ptr<Texture2D>> sprites;
    std::unordered_map<Texture2D*,std::string> spritePaths;

public:
    void addSprite()
    {

    }
    template<typename Path, typename... Paths>
    void addSprite(Path path, Paths... paths)
    {
        Texture2D* sprite = new Texture2D(LoadTexture((std::string("sprites/") + std::string(path)).c_str()));
        std::cout << path << "\n";
        if (IsTextureValid(*sprite))
        {
            sprites[path].reset(sprite);
            spritePaths[sprite] = path;
        }
        else
        {
            std::cerr << "ERROR SpritesGlobal::addSprite: unable to load sprite: " << path << "\n";
        }
        addSprite(paths...);
    }
    void addSprites(std::string folder);
    std::string getSpritePath(Texture2D* sprite);
    Texture2D* getSprite(std::string str);
};

#endif // SPRITES_H_INCLUDED
