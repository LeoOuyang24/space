#ifndef SHAPE_H_INCLUDED
#define SHAPE_H_INCLUDED

#include <functional>

enum ShapeType
{
    CIRCLE = 0,
    RECT
};


struct Orient;
struct CircleCollider;
struct RectCollider;
struct Shape
{
    ShapeType type;
    Orient& orient;
    void* collider = nullptr;
};

//runs a function depending on what shape "shape" is
//doing this instead of just writing the if statements standardizes the static cast we have to do
void ifShapeType(const Shape& shape, std::function<void(CircleCollider*)> circleFunc, std::function<void(RectCollider*)> rectFunc);

//same as above but which function to run is determined at compile time
template<ShapeType type>
void ifShapeType(const Shape& shape, std::function<void(CircleCollider*)> circleFunc, std::function<void(RectCollider*)> rectFunc)
{
    if constexpr (type == ShapeType::CIRCLE)
    {
        CircleCollider* circle = static_cast<CircleCollider*>(shape.collider);
        circleFunc(circle);
    }
    else if (type == ShapeType::RECT)
    {
        RectCollider* rect = static_cast<RectCollider*>(shape.collider);
        rectFunc(rect);
    }
    else
    {
        std::cerr << "ifShapeType (template version) ERROR: unknown shape " << type << "\n";
    }
}

bool CheckCollision(const Shape& shape1, const Shape& shape2);


#endif // SHAPE_H_INCLUDED
