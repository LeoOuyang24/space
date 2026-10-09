#include "../headers/gravity.h"
#include "../headers/raylib_helper.h"
#include "../headers/game.h"
#include "../headers/objects.h"
#include "../headers/blocks.h"

GravityField::GravityField(size_t width_, size_t fieldWidth_, int gravityRadius_) :  width(width_), fieldWidth(fieldWidth_), gravityRadius(gravityRadius_), fields(width*width)
{

}

void GravityField::addBlock(const Vector2& pos, bool remove, float magnitude)
{
    int baseIndex = pointToIndex(pos,fieldWidth,width);

    float ratio = 0.5;

    for (int i = -gravityRadius; i < gravityRadius; i ++)
    {
        for (int j = -gravityRadius; j < gravityRadius; j++)
        {
            int index = baseIndex - j*width - i;
            if (index >= 0 && index < fields.size())
            {
                //{i,j} is indicies and the actual force we want to apply, since "pos" is in the center of this for loop
                float falloff = pow(ratio,std::max(abs(i),abs(j)));//(i == 0 && j == 0) ? 1 : 1.0f/Vector2Length({i,j});
                Vector2 gravAmount = Vector2Normalize(Vector2{i,j})*(falloff)*GlobalTerrain::GRAVITY_CONSTANT*magnitude;
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
    if (!Debug::isPaused())
    {
        const int renderHowMany = 50;
        Vector2 pos = Globals::Game.getPlayer()->getPos();
        int start = pointToIndex(pos - Vector2{renderHowMany/2*fieldWidth,renderHowMany/2*fieldWidth},fieldWidth,width);
        for (int i = 0; i < renderHowMany; i ++)
        {
            for (int j = 0; j < renderHowMany; j ++)
            {
                int index = start + i + j*width;
                if (index < fields.size() && index >= 0)
                {
                    if (fields[index].total)
                    {
                        Vector3 center = toVector3(indexToPoint(index,fieldWidth,width) + Vector2(fieldWidth/2,fieldWidth/2));
                        Debug::addDeferRender([center,dir=fields[index].totalDir/fields[index].total,fieldWidth=fieldWidth](){

                            //DrawCubeWires(center,fieldWidth,fieldWidth,0,BLUE);
                            DrawArrow3D(Vector2Normalize(dir)*fieldWidth/2,center,RED,1);

                        });
                    }
                }
                else
                {
                    break;
                }
            }
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