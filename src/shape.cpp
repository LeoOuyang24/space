#include <iostream>

#include "../headers/shape.h"
#include "../headers/objects.h"
#include "../headers/colliders.h"

bool CheckCollision(const Shape& shape1, const Shape& shape2)
{
    if (shape1.type == shape2.type)
    {
        switch (shape1.type)
        {
            case ShapeType::CIRCLE:
                {
                    //CircleCollider* ptr1 = static_cast<CircleCollider*>(shape1.collider); CircleCollider* ptr2 = static_cast<CircleCollider*>(shape2.collider);
                    return CheckCollisionCircles(shape1.orient.pos,shape1.collider.radius,shape2.orient.pos,shape2.collider.radius);
                }
            case ShapeType::RECT:
                {
                    return CheckCollisionRecs(shape1.orient.getRect(shape1.collider.dimens),
                                              shape2.orient.getRect(shape2.collider.dimens));
                }
            default:
                std::cerr << "getCollision ERROR: unknown shapes: " << shape1.type << " " << shape2.type << "\n";
                return false;
        }
    }
    else
    {
        if (shape1.type == ShapeType::CIRCLE && shape2.type == ShapeType::RECT)
        {
            return CheckCollisionCircleRec(rotatePoint(shape1.orient.pos,shape2.orient.pos,-shape2.orient.rotation),
                                           shape1.collider.radius,shape2.orient.getRect(shape2.collider.dimens));
        }
        else
        {
            return CheckCollision(shape2,shape1);
        }
    }
}

Vector2 GetDimen(const Shape& shape)
{
    switch(shape.type)
    {
    case CIRCLE:
        return {shape.collider.radius,shape.collider.radius};
    case RECT:
        return shape.collider.dimens;
    default:
        std::cerr << "GetDimen ERROR: unknown shapes: " << shape.type << "\n";
        return {};
    }
}
