#include "../headers/render.h"

TextureRenderer::TextureRenderer(Texture2D* sprite)
{
    setSprite(sprite);
}

void TextureRenderer::setSprite(Texture2D* sprite_)
{
    sprite = sprite_;
}
void TextureRenderer::render(const Shape& shape, const Color& color)
{
    if (sprite)
    {
        Vector2 dimen = GetDimen(shape);
        int flip = (facing)*2 - 1;

        DrawBillboardPro(Globals::Game.camera,*sprite,Rectangle(0,0,sprite->width*flip,sprite->height),
                         Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),Vector3(0,-1,0),dimen,
                         dimen*0.5,shape.orient.rotation*RAD2DEG*-1,color);
    }
}
