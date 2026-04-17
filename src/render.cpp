#include "../headers/render.h"

void NoRenderer::render(const Shape& shape, const Color& color)
{
    if (Debug::isDebugOn())
    {
        Rectangle rect = shape.orient.getRect(GetDimen(shape));
        for (int i = 0; i < 4; i ++)
        {
            DrawLine3D(toVector3(Vector2(rect.x,rect.y) + Vector2(rect.width*(i%3 == 0),rect.height*(i > 1))),
                       toVector3(Vector2(rect.x,rect.y) + Vector2(rect.width*((i+1)%4%3 == 0),rect.height*((i + 1)%4 > 1))),
                    WHITE,10);
        }
    }
}

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
    if (!IsTextureValid(sprite))
    {
        sprite = Globals::Game.Sprites.getSprite("squares.png");
        if (!IsTextureValid(sprite))
        {
            std::cerr << "ERROR: TextureRenderer::render: We're cooked! Not only is sprite invalid, the error sprite is too!\n";
        }
    }
    Vector2 dimen = GetDimen(shape);
    int flip = (shape.orient.facing)*2 - 1;

    DrawBillboardPro(Globals::Game.getCamera(),sprite,Rectangle(0,0,sprite.width*flip,sprite.height),
                     Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),Vector3(0,-1,0),dimen,
                     dimen*0.5,shape.orient.rotation*RAD2DEG*-1,color);
    if (Debug::isDebugOn())
    {
        Vector2 dimens = GetDimen(shape);
        for (size_t i = 0; i < getShapePoints(shape.type); i ++)
        {
            DrawLine3D(toVector3(getIthShapePoint(shape,i)),toVector3(getIthShapePoint(shape,i+1)),RED);
        }
    }

}

AnimeRenderer::AnimeRenderer(const std::initializer_list<std::string_view>& lst)
{
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
