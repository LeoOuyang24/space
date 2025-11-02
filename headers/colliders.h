#ifndef COLLIDERS_H_INCLUDED
#define COLLIDERS_H_INCLUDED

#include "shape.h"

//life is too short to have to write the getShapeType function for each collider
#define GET_SHAPE_TYPE(type) ShapeType getShapeType() {return type;}

struct CircleCollider
{
    int radius = 0;


    bool isOnGround(const Orient& orient, Terrain& t);
    float getLandingAngle(Orient& orient, Terrain& terrain);

    GET_SHAPE_TYPE(ShapeType::CIRCLE);
};

struct RectCollider
{
    //orient.pos is considered to be the center of the rectangle
    float width = 0, height = 0;

    bool isOnGround(const Orient& orient, Terrain& t);
    Rectangle getRect(const Orient& orient);
    float getLandingAngle(Orient& orient, Terrain& terrain);

    GET_SHAPE_TYPE(ShapeType::RECT);
};


#endif // COLLIDERS_H_INCLUDED
