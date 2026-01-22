#ifndef SHAPE_H_INCLUDED
#define SHAPE_H_INCLUDED

#include <functional>
#include <iostream>

#include "raylib.h"

#include "terrain.h"

enum ShapeType
{
    CIRCLE = 0,
    RECT
};

struct Orient
{
    Vector2 pos = {0,0};

    LayerType layer = -1;

    float rotation = 0; // IN RADIANS


    inline Vector2 getFacing() const
    {
        return Vector2Rotate(Vector2(1,0),rotation);
    }
    inline Vector2 getNormal() const
    {
        return Vector2Rotate(Vector2(0,1),rotation);
    }
    inline Rectangle getRect(const Vector2& dimens) const
    {
        return {pos.x - dimens.x/2, pos.y - dimens.y/2, dimens.x, dimens.y};
    }


};

union ShapeCollider
{
    Vector2 dimens;
    int radius;
};
struct Shape
{
    ShapeType type;
    Orient orient;
    ShapeCollider collider;

};
//check collision between two rotated rectangles
bool CheckCollisionPointRecRotated(const Vector2& pos, const Rectangle& rect, float angle);
bool CheckCollisionRecsRotated(const Rectangle& r1, const Rectangle& r2, float rot1, float rot2);
bool CheckCollision(const Shape& shape1, const Shape& shape2);
Vector2 GetDimen(const Shape& shape1);


#endif // SHAPE_H_INCLUDED
