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
        ifShapeType<shapeType>(shape,
                               [&color,&shape](CircleCollider* circle){

                            //DrawCircle(shape.orient.pos.x,shape.orient.pos.y,circle->radius,color);
                            DrawCircle3D({shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)},circle->radius,{0,0,0},0,WHITE);

                    },
                                [&color,&shape](RectCollider* rect){
                            DrawCube(Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),
                                     rect->width,rect->height,1,color);

                            //DrawPlane({shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)},{rect->width,rect->height},color);
                            //DrawRectanglePro({shape.orient.pos.x,shape.orient.pos.y,rect->width,rect->height},{rect->width/2,rect->height/2},shape.orient.rotation*RAD2DEG,color);

                    }

                    );
    }
};

struct TextureRenderer
{
    Texture2D* sprite = nullptr;

    void setSprite(Texture2D& sprite_);
    virtual void render(const Shape& shape, const Color& color);

};

#endif // RENDER_H_INCLUDED
