#include <iostream>

#include "../headers/shape.h"
#include "../headers/objects.h"
#include "../headers/colliders.h"

bool CheckCollisionPointRecRotated(const Vector2& pos, const Rectangle& rect, float angle)
{
    Vector2 rotated = rotatePoint(pos,{rect.x + rect.width/2, rect.y + rect.height/2},-angle);
    return CheckCollisionPointRec(rotated,rect);
}


bool CheckCollisionRecsRotated(const Rectangle& r1, const Rectangle& r2, float rot1, float rot2)
{
    if (rot1 == 0 && rot2 == 0)
    {
        return CheckCollisionRecs(r1,r2);
    }

    //just check if any of the rotated edges intersect
    //too lazy to learn separating axis theorem
    //though if I understand it correctly, this algorithm basically is the same but specifically for rectangles
    Vector2 center1 = {r1.x + r1.width/2,r1.y + r1.height/2};
    Vector2 center2 = {r2.x + r2.width/2,r2.y + r2.height/2};

    if (CheckCollisionPointRecRotated(center2,r1,rot1) ||
        CheckCollisionPointRecRotated(center1,r2,rot2)) //check for containment
    {
        return true;
    }

    Vector2 corners1[4] = { //corners in clockwise order
        rotatePoint(Vector2(r1.x,r1.y),center1,rot1), //topleft
        rotatePoint(Vector2(r1.x + r1.width,r1.y),center1,rot1), //topright
        rotatePoint(Vector2(r1.x + r1.width,r1.y + r1.height),center1,rot1), //bottomright
        rotatePoint(Vector2(r1.x,r1.y + r1.height),center1,rot1) //bottom left
    };

    Vector2 corners2[4] = {
        rotatePoint(Vector2(r2.x,r2.y),center2,rot2),
        rotatePoint(Vector2(r2.x + r2.width,r2.y),center2,rot2),
        rotatePoint(Vector2(r2.x + r2.width,r2.y + r2.height),center2,rot2),
        rotatePoint(Vector2(r2.x,r2.y + r2.height),center2,rot2)
    };

    for (int i = 0; i < 4; i ++)
    {
        for (int j = 0; j < 4; j ++)
        {
            Vector2 coll = {};
            if (CheckCollisionLines(corners1[i], corners1[(i+1)%4],corners2[j],corners2[(j+1)%4],&coll))
            {
                return true;
            }
        }
    }

    return false;


}

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
                    return CheckCollisionRecsRotated(shape1.orient.getRect(shape1.collider.dimens),
                                              shape2.orient.getRect(shape2.collider.dimens),
                                                shape1.orient.rotation,
                                                shape2.orient.rotation);
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
        return {shape.collider.radius*2,shape.collider.radius*2};
    case RECT:
        return shape.collider.dimens;
    default:
        std::cerr << "GetDimen ERROR: unknown shapes: " << shape.type << "\n";
        return {};
    }
}
