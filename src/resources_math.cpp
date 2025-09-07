#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../headers/resources_math.h"

float trunc(float x, int places)
{
    return static_cast<int>(x*pow(10,places))/pow(10,places);
}

Vector2 trunc(const Vector2& p, int places)
{
    return {trunc(p.x,places),trunc(p.y,places)};
}

Vector2 rotatePoint(const Vector2& p, const Vector2& rotateAround, float angle)
{
    Vector2 point = {p.x - rotateAround.x,p.y-rotateAround.y};//distances between target and pivot point
    return Vector2Rotate(p - rotateAround, angle) + rotateAround;
}

float pointInRectAngle(const Vector2& p1, const Rectangle& rect)
{
    float slope = rect.height/rect.width;
    float angle = 0;
    Vector2 center = Vector2(rect.x + rect.width/2, rect.y + rect.height/2);
    float y2 = (p1.x - center.x)*slope;
    float y1 = p1.y - center.y;


    if (y1 > y2 && y1 > -y2) //bottom
    {
        return M_PI/2;
    }
    else if (y1 < y2 && y1 > -y2) //right
    {
        return 0;
    }
    else if (y1 < y2 && y1 < -y2) //top
    {
        return -M_PI/2;
    }
    else if (y1 > y2 && y1 < -y2) //LEFT
    {
        return M_PI;
    }
    else if (y1 == y2 && y1 <-y2) //perfectly at the top left corner
    {
        return -135*DEG2RAD;
    }
    else if (y1 == y2 && y1 < -y2) //perfectly at the bot right corner
    {
        return 45*DEG2RAD;
    }
    else if (y1 == -y2 && y1 < y2) //top right corner
    {
        return -45*DEG2RAD;
    }
    else //bottom left corner;
    {
        return 135*DEG2RAD;
    }
}

PossiblePoint segmentIntersect(const Vector2& a1, const Vector2& a2, const Vector2& b1, const Vector2& b2)
{

    Vector2 v0 = a2 - a1;
    Vector2 v1 = b2 - b1;

    float det = v1.x * v0.y - v0.x*v1.y;
    if (det == 0) //if determinant is 0, lines are parallel
    {
        if (a1.x == a2.x && b1.x == a1.x && //both are coincident, vertical lines
            (std::max(a1.y,a2.y) >= std::min(b1.y,b2.y) && std::min(b1.y,b2.y) >= std::min(a1.y,a2.y) ||
            (std::max(b1.y,b2.y) >= std::min(a1.y,a2.y) && std::min(a1.y,a2.y) >= std::min(b1.y,b2.y))))
        {
            return {true,std::max(std::min(b1.y,b2.y),std::min(a1.y,a2.y))}; //return 2nd lowest point, which since there is an intersection, must be on both lines
        }
        else //non-vertical lines
        {


            float r1 = (b1.x - a1.x)/(v0.x); //r1*v0.x + a1.x = b1.x;
            float r2 = (b2.x - a1.x)/(v0.x); //r2*v1.x + a1.x = b2.x;
            //std::cout << a1.x << " " << a2.x << " " << b1.x << " " << b2.x << " " << "\n";

            //if either r1 or r2 are between 0 and 1, then the lines are coincident and there is an intersection between the segments
            if ((r1 >= 0 && r1 <= 1))
            {
                return {true, v0*r1 + a1};
            }
            else if (r2 >= 0 && r2 <= 1)
            {
                return {true, v0*r2 + a1};
            }
        }
        return {false,{}};
    }

    //https://stackoverflow.com/questions/4977491/determining-if-two-line-segments-intersect/4977569#4977569
    float b = ((a1.x - b1.x)*v0.y - (a1.y - b1.y)*v0.x)/det;
    float a = ((a1.x - b1.x)*v1.y - (a1.y - b1.y)*v1.x)/det;

    if (a >= 0 && a <= 1 && b >= 0 && b <= 1)
    {

        return {true, (a2 - a1)*a + a1};
    }
    else
    {
        return {false,{}};
    }

}

PossiblePoint segmentIntersectRect(const Vector2& a1, const Vector2& a2, const Rectangle& rect)
{
    Vector2 points[4] =  {
            Vector2(rect.x,rect.y),
            Vector2(rect.x + rect.width, rect.y),
            Vector2(rect.x + rect.width, rect.y + rect.height),
            Vector2(rect.x,rect.y + rect.height)
            };


    PossiblePoint answer;
    for (int i = 0; i < 4; i ++)
    {
        //DrawLine(points[i].x,points[i].y,points[(i + 1)%4].x,points[(i + 1)%4].y,Color{255,0,0,255});
        PossiblePoint p = segmentIntersect(a1,a2,points[i],points[(i + 1)%4]);

        if (p.exists)
        {
            answer = p;
            break;
        }
    }

    return answer;
}

PossiblePoint segmentIntersectTriangle(const Vector2& a1, const Vector2& a2, const Vector2& t1, const Vector2& t2, const Vector2& t3)
{
    PossiblePoint answer = segmentIntersect(a1,a2,t1,t2);
    if (!answer.exists)
    {
        answer = segmentIntersect(a1,a2,t2,t3);
        if (!answer.exists)
        {
            answer = segmentIntersect(a1,a2,t3,t1);
        }
    }
    return answer;
}

