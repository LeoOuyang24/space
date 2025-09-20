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


        /*DrawTexturePro(*sprite,
                       Rectangle(0,0,sprite->width,sprite->height),
                       Rectangle(shape.orient.pos.x,shape.orient.pos.y,dimen.x,dimen.y),
                       Vector2(dimen.x/2,dimen.y/2),
                       shape.orient.rotation*RAD2DEG ,color
                       );*/
        DrawBillboardPro(Globals::Game.camera,*sprite,Rectangle(0,0,sprite->width,sprite->height),
                         Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),Vector3(0,-1,0),dimen,
                         dimen*0.5,shape.orient.rotation*RAD2DEG,color);
    }


}
