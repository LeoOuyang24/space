#include "../headers/gravity.h"
#include "../headers/raylib_helper.h"
#include "../headers/game.h"
#include "../headers/blocks.h"

GravityField::GravityField(int gravRad) : gravityRadius(gravRad)
{

}

void GravityField::addBlock(const Vector2& pos)
{
    int baseIndex = pointToIndex(pos,FIELD_WIDTH,WIDTH);

    const float ratio = 1.0f/gravityRadius;

    Vector2 center = roundPos(pos,FIELD_WIDTH);

    for (int i = -gravityRadius; i < gravityRadius; i ++)
    {
        for (int j = -gravityRadius; j < gravityRadius; j++)
        {
            int index = baseIndex - j*WIDTH - i;
            if (index >= 0 && index < fields.size())
            fields[index] += Vector2Normalize({i,j})*(pow(ratio,std::max(abs(i),abs(j))));
        }
    }
}

void GravityField::debugRender()
{
    for (int i = 0; i < fields.size(); i ++)
    {

        if (fields[i].total)
        {
            Vector3 center = toVector3(indexToPoint(i,FIELD_WIDTH,WIDTH) + Vector2(FIELD_WIDTH/2,FIELD_WIDTH/2));
            Debug::addDeferRender([center,dir=fields[i].totalDir/fields[i].total](){

                DrawCubeWires(center,FIELD_WIDTH,FIELD_WIDTH,0,BLUE);
                DrawArrow3D(Vector2Normalize(dir)*FIELD_WIDTH/2,center,RED,1);

            });
        }
    }
}