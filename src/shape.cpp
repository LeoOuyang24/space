#include <iostream>

#include "../headers/shape.h"
#include "../headers/objects.h"

void ifShapeType(const Shape& shape, std::function<void(CircleCollider*)> circleFunc, std::function<void(RectCollider*)> rectFunc)
{
    switch (shape.type)
    {
    case ShapeType::CIRCLE:
        {
            CircleCollider* circle = static_cast<CircleCollider*>(shape.collider);
            circleFunc(circle);
            break;
        }
    case ShapeType::RECT:
        {
            RectCollider* rect = static_cast<RectCollider*>(shape.collider);
            rectFunc(rect);
            break;
        }
    default:
        std::cerr << "ifShapeType ERROR: invalid shape: " << shape.type << "\n";
    }
}

#define DOUBLECAST(type1, type2) type1* ptr1 = static_cast<type1*>(shape1.collider); type2* ptr2 = static_cast<type2*>(shape2.collider);
bool CheckCollision(const Shape& shape1, const Shape& shape2)
{
    if (shape1.type == shape2.type)
    {
        switch (shape1.type)
        {
            case ShapeType::CIRCLE:
                {
                    DOUBLECAST(CircleCollider, CircleCollider);
                    //CircleCollider* ptr1 = static_cast<CircleCollider*>(shape1.collider); CircleCollider* ptr2 = static_cast<CircleCollider*>(shape2.collider);
                    return CheckCollisionCircles(shape1.orient.pos,ptr1->radius,shape2.orient.pos,ptr2->radius);
                }
            case ShapeType::RECT:
                {
                    DOUBLECAST(RectCollider,RectCollider);
                    return CheckCollisionRecs(ptr1->getRect(shape1.orient),ptr2->getRect(shape2.orient));
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
            DOUBLECAST(CircleCollider,RectCollider);
            //rotate the circle around the rect so this can account for the rectangle's rotation
            return CheckCollisionCircleRec(rotatePoint(shape1.orient.pos,shape2.orient.pos,-shape2.orient.rotation),ptr1->radius,ptr2->getRect(shape2.orient));
        }
        else
        {
            return CheckCollision(shape2,shape1);
        }
    }
}
