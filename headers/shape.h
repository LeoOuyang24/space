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
    bool facing = true; //true if facing to the right
    Vector2 startingPos = {};

    inline Vector2 getFacingVector() const
    {
        return Vector2Rotate(Vector2(facing*2 - 1,0),rotation);
    }
    inline Vector2 getNormal() const
    {
        return Vector2Rotate(Vector2(0,1),rotation);
    }
    inline Rectangle getRect(const Vector2& dimens) const
    {
        return {pos.x - dimens.x/2, pos.y - dimens.y/2, dimens.x, dimens.y};
    }

    void setStartingPos(const Vector2& start); //ideally this should only be called when an object is first added into the world
    Vector2 getStartingPos();


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
bool CheckCollisionPointShape(const Vector2& pos, const Shape& shape1);
bool CheckCollisionPointRecRotated(const Vector2& pos, const Rectangle& rect, float angle);
//check collision between two rotated rectangles
bool CheckCollisionRecsRotated(const Rectangle& r1, const Rectangle& r2, float rot1, float rot2);
bool CheckCollision(const Shape& shape1, const Shape& shape2);

Vector2 lineShapeIntersect(const Shape& shape1, const Vector2& a, const Vector2& b); //returns the point closest to "a", that collides with "shape1", "b" if no such intersection

Vector2 GetDimen(const Shape& shape1);


/**
 * @brief Get the Ith Point on a shape. 
 * 
 * @param shape 
 * @param i, the ith point
 * @return Vector2 
 */
Vector2 getIthShapePoint(const Shape& shape, int i); 

/**
 * @brief Return the maximum number that "i" can be in getIthShapePoint (larger i's are still expected to be handeled, but will probably just loop back around like in a modulo)
 * 
 * @param type 
 * @return constexpr size_t 
 */
constexpr size_t getShapePoints(ShapeType type)
{
    switch (type)
    {
        case ShapeType::CIRCLE:
            return 10; //magic number, make larger for more expensive but accurate calculations
        case ShapeType::RECT:
            return 4;
        default:
            return 0;
    }  
}

#endif // SHAPE_H_INCLUDED
