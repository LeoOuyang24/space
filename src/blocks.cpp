#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#include "../headers/blocks.h"



void Block::render()
{
    //DrawCircle(pos.x,pos.y,BLOCK_DIMEN,color);
    DrawPoly(pos,6,Block::BLOCK_DIMEN,0,color); //it should be noted this slightly overrenders. Render at half BLOCK_DIMEN to have no polygons overlap
    //DrawRectangle(pos.x - BLOCK_DIMEN/2,pos.y - BLOCK_DIMEN/2,BLOCK_DIMEN,BLOCK_DIMEN,color);
}

Vector2 roundPos(const Vector2& vec)
{
    return Vector2(vec.x - fmod(vec.x,Block::BLOCK_DIMEN),vec.y - fmod(vec.y,Block::BLOCK_DIMEN));
}

void GlobalTerrain::generatePlanet(const Vector2& center, int radius )
{

    forEachPos([this,&center,radius](const Vector2& pos){
                    terrain[pos] = std::unique_ptr<Block>(new Block{pos,GREEN});
               },center,radius);

}


void GlobalTerrain::generatePlanets()
{
    const int range = 1000;

    terrain.clear();

    int numPlanets = rand()%3 + 3;

    for (int i = 0; i < numPlanets; i++)
    {
        Vector2 randomCenter = Vector2(0,0); //first planet is always at 0,0

        while (blockExists(randomCenter))
        {
            randomCenter = Vector2(rand()%range - range/2,rand()%range - range/2);
        }

        int radius = rand()%(100) + 100;

        generatePlanet(randomCenter,radius);
    }
}
bool GlobalTerrain::blockExists(const Vector2& pos)
{
    return terrain.find(pos) != terrain.end();
}

void GlobalTerrain::remove(const Vector2& pos, int radius)
{
    forEachPos([this](const Vector2& pos){
               auto it = terrain.find(pos);
               if (it != terrain.end())
               {
                  terrain.erase(it);
               }
               },pos,radius);
}

void GlobalTerrain::render()
{
    for (auto it = terrain.begin(); it != terrain.end(); ++it)
    {
        if (it->second.get())
        {
            it->second->render();
        }
    }


}
