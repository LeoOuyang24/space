#include "../headers/gravity.h"
#include "../headers/raylib_helper.h"
#include "../headers/game.h"
#include "../headers/blocks.h"

GravityField::GravityField(size_t width_, size_t fieldWidth_, int gravityRadius_) :  width(width_), fieldWidth(fieldWidth_), gravityRadius(gravityRadius_), fields(width*width)
{

}

void GravityField::addBlock(const Vector2& pos, bool remove)
{
    int baseIndex = pointToIndex(pos,fieldWidth,width);

    const float ratio = 0.5;//1.0f/gravityRadius;

    Vector2 center = roundPos(pos,fieldWidth);

    for (int i = -gravityRadius; i < gravityRadius; i ++)
    {
        for (int j = -gravityRadius; j < gravityRadius; j++)
        {
            int index = baseIndex - j*width - i;
            if (index >= 0 && index < fields.size())
            {
                //{i,j} is indicies and the actual force we want to apply, since "pos" is in the center of this for loop
                Vector2 gravAmount = Vector2Normalize(Vector2{i,j})*(pow(ratio,std::max(abs(i),abs(j))))*GlobalTerrain::GRAVITY_CONSTANT;
                if (remove)
                {
                    fields[index] -= gravAmount;
                }
                else
                {
                    fields[index] += gravAmount;
                }
            }
        }
    }
}

Vector2 GravityField::getFieldAtPos(const Vector2& pos)
{
    size_t index = pointToIndex(pos,fieldWidth,width);
    if (index < fields.size())
    {
        if (fields[index].total) 
        {
            return fields[index].totalDir/fields[index].total;
        }
        else
        {
            return Vector2{};
        }
    }
    return {};
}

void GravityField::debugRender()
{
    for (int i = 0; i < fields.size(); i ++)
    {

        if (fields[i].total)
        {
            Vector3 center = toVector3(indexToPoint(i,fieldWidth,width) + Vector2(fieldWidth/2,fieldWidth/2));
            if (!Debug::isPaused())
            Debug::addDeferRender([center,dir=fields[i].totalDir/fields[i].total,fieldWidth=fieldWidth](){

                DrawCubeWires(center,fieldWidth,fieldWidth,0,BLUE);
                DrawArrow3D(Vector2Normalize(dir)*fieldWidth/2,center,RED,1);

            });
        }
    }
}

GravityField::GravField GravityField::GravField::operator+(const Vector2& pos)
{
    totalDir += pos;
    total += 1;

    return *this;
}

GravityField::GravField GravityField::GravField::operator-(const Vector2& pos)
{
    total -= 1;

    if (total == 0)
    {
        totalDir = {};
    }
    else
    {
        totalDir -= pos;
    }

    return *this;      
}

void GravityField::GravField::operator+=(const Vector2& pos)
{
    *this = *this + pos;
}

void GravityField::GravField::operator-=(const Vector2& pos)
{
    *this = *this - pos;
}