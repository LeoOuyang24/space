#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <iostream>

#include "shape.h"
#include "objects.h"
#include "game.h"

template<ShapeType shapeType>
struct ShapeRenderer
{
    void render(const Shape& shape, const Color& color)
    {
        switch (shape.type)
        {
        case CIRCLE:
            DrawCircle3D({shape.orient.pos.x,shape.orient.pos.y,
                         Globals::Game.terrain.getZOfLayer(shape.orient.layer)},shape.collider.radius,{0,0,0},0,color);
            break;
        case RECT:
             DrawCube(Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),
                                     shape.collider.dimens.x,shape.collider.dimens.y,1,color);
            break;
        }

    }
};


struct TextureRenderer
{
    Texture2D sprite = {};
    bool facing = true; //true if facing to the right
    TextureRenderer()
    {

    }
    TextureRenderer(const Texture2D& sprite);
    void setSprite(const Texture2D& sprite_);
    virtual void render(const Shape& shape, const Color& color);

};


class AnimeRenderer
{
    std::unordered_map<std::string, std::string> states; //map of state name to sprite name
    const Anime* current = nullptr;
    double start = -1;

public:
    //list of state and sprites. even index strings are state names, odd index strings are sprite names
    AnimeRenderer(const std::initializer_list<std::string_view>& lst);
    void setState(std::string_view stateName);
    void render(const Shape& shape, const Color& color);
};
#endif // RENDER_H_INCLUDED
