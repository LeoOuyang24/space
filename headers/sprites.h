#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <iostream>
#include <unordered_map>
#include <raylib.h>
#include <memory>

class SpritesGlobal
{
    std::unordered_map<std::string,std::unique_ptr<Texture2D>> sprites;
    std::unordered_map<Texture2D*,std::string> paths;

public:
    void addSprite()
    {

    }
    template<typename Path, typename... Paths>
    void addSprite(Path path, Paths... paths)
    {

        std::string fullPath = std::string("sprites/") + std::string(path);
        Texture2D* sprite = new Texture2D(LoadTexture(fullPath.c_str()));

        if (!IsTextureValid(*sprite))
        {
            std::cerr << "ERROR SpritesGlobal::addSprite: unable to load sprite: " << path << "\n";
            return;
        }
        sprites[path].reset(sprite);
        this->paths[sprite] = path;
        addSprite(paths...);
    }

    void addSprites(std::string folderPath);
    Texture2D* getSprite(std::string str);
    std::string getSpritePath(Texture2D*);
};

#endif // SPRITES_H_INCLUDED
