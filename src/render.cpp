#include "../headers/render.h"

void TextureRenderer::setSprite(Texture2D& sprite_)
{
    sprite = &sprite_;
}
void TextureRenderer::render(const Shape& shape, const Color& color)
{
    if (sprite)
    {
        Vector2 dimen;

        ifShapeType(shape,[&dimen](CircleCollider* circle){

                        dimen = {circle->radius,circle->radius};

                    },
                    [&dimen](RectCollider* rect){

                        dimen = {rect->width,rect->height};

                    }
                    );


        DrawTexturePro(*sprite,
                       Rectangle(0,0,sprite->width,sprite->height),
                       Rectangle(shape.orient.pos.x,shape.orient.pos.y,dimen.x,dimen.y),
                       Vector2(dimen.x/2,dimen.y/2),
                       shape.orient.rotation*RAD2DEG ,color
                       );
    }


}
