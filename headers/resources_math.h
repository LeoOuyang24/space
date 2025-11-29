#ifndef RESOURCES_MATH_H_INCLUDED
#define RESOURCES_MATH_H_INCLUDED

#include <utility>

#include <raylib.h>
#include "raymath.h"


//generic math functions I use


//round "x" to "places" decimal places
float trunc(float x, int places);
Vector2 trunc(const Vector2& p, int places);


struct PossiblePoint //a point that may not exist
{
    bool exists = false;
    Vector2 pos = {0,0};
};

std::ostream& operator<<(std::ostream &os,const Vector2& vec);

Vector2 rotatePoint(const Vector2& p, const Vector2& rotateAround, float angle);

//returns which quadrant p1 is in rect. -M_PI/2 = top quadrant, M_PI/2 = bottom quadrant
float pointInRectAngle(const Vector2& p1, const Rectangle& rect);

//converts a 2d point on the screen to a point in a 3d world at the desired "z" value
Vector2 screenToWorld(const Vector2& screenPoint, Camera3D& camera, const Vector2& screenDimen, float z);

PossiblePoint segmentIntersect(const Vector2& a1, const Vector2& a2, const Vector2& b1, const Vector2& b2); //returns false if the lines segments don't exist
PossiblePoint segmentIntersectRect(const Vector2& a1, const Vector2& a2, const Rectangle& rect);
PossiblePoint segmentIntersectTriangle(const Vector2& a1, const Vector2& a2, const Vector2& t1, const Vector2& t2, const Vector2& t3);

template<typename T>
concept has_y = requires { &T::y; };

template<typename T>
concept has_z = requires { &T::z; };

template<typename T>
concept has_a = requires { &T::a; };

//assigns different dimension vectors to each other
// example: Vector2 can now be assigned to a Vector3, setting only the x and y fields
template<typename Vector, typename OtherVector>
void assignVector(Vector& v, const OtherVector& o)
{
    v.x = o.x;

    if constexpr (has_y<Vector> && has_y<OtherVector>)
    {
        v.y = o.y;
    }

    if constexpr (has_z<Vector> && has_z<OtherVector>)
    {
        v.z = o.z;
    }

    if constexpr (has_a<Vector> && has_a<OtherVector>)
    {
        v.a = o.a;
    }
}


#endif // RESOURCES_MATH_H_INCLUDED
