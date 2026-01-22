#include "../headers/render.h"

TextureRenderer::TextureRenderer(const Texture2D& sprite)
{
    setSprite(sprite);
}

void TextureRenderer::setSprite(const Texture2D& sprite_)
{
    sprite = sprite_;
}
void TextureRenderer::render(const Shape& shape, const Color& color)
{
    if (IsTextureValid(sprite))
    {
        Vector2 dimen = GetDimen(shape);
        int flip = (facing)*2 - 1;

        DrawBillboardPro(Globals::Game.getCamera(),sprite,Rectangle(0,0,sprite.width*flip,sprite.height),
                         Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),Vector3(0,-1,0),dimen,
                         dimen*0.5,shape.orient.rotation*RAD2DEG*-1,color);
    }
    else
    {
        std::cerr << "TextureRenderer::render WARNING: invalid texture!\n";
    }
}

AnimeRenderer::AnimeRenderer(const std::initializer_list<std::string_view>& lst)
{
    int i = 0;
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        std::string_view stateName = *it;
        ++it;
        if (it != lst.end())
        {
            states[stateName.data()] = *it;
        }
    }
}

void AnimeRenderer::setState(std::string_view str)
{
    auto it = states.find(str.data());
    if (it != states.end())
    {
        current = Globals::Game.Sprites.getAnime(it->second);
    }
}

void AnimeRenderer::render(const Shape& shape, const Color& color)
{
    if (current && IsTextureValid(current->spritesheet))
    {
        if (start == -1)
        {
            start = GetTime();
        }
        Vector2 dimen = GetDimen(shape);
        DrawAnime3D(current->spritesheet,start,current->info,
                    Rectangle(shape.orient.pos.x,shape.orient.pos.y, dimen.x,dimen.y),
                    Globals::Game.terrain.getZOfLayer(shape.orient.layer),shape.orient.rotation,color);
    }
    //auto it = states.find(currentState.data())
    /*if (currentAnime)
    {
        //DrawBillboardPro()
    }
 */
}
