#include <iostream>
#include "../headers/blocks.h"

void Block::render()
{
    //DrawCircle(pos.x,pos.y,BLOCK_DIMEN,color);
    DrawPoly(pos,6,BLOCK_DIMEN,0,color); //it should be noted this slightly overrenders. Render at half BLOCK_DIMEN to have no polygons overlap
    //DrawRectangle(pos.x - BLOCK_DIMEN/2,pos.y - BLOCK_DIMEN/2,BLOCK_DIMEN,BLOCK_DIMEN,color);
}

Vector2 roundPos(const Vector2& vec)
{
    return Vector2(vec.x - ((int)vec.x)%Block::BLOCK_DIMEN,vec.y - ((int)vec.y)%Block::BLOCK_DIMEN);
}

void GlobalTerrain::generatePlanet(const Vector2& center, int radius )
{

    forEachPos([this,&center,radius](const Vector2& pos){

               if (Vector2Distance(pos,center) <= radius)
               {
                    terrain[pos] = std::unique_ptr<Block>(new Block{pos,GREEN});
               }


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

void GlobalTerrain::forEachPos(std::function<void(const Vector2&)> func, const Vector2& pos, int radius)
{
    for (int i = pos.x - radius; i <= pos.x + radius; i+= Block::BLOCK_DIMEN)
    {
        for (int j = pos.y - radius; j <= pos.y + radius; j+=Block::BLOCK_DIMEN)
        {
            Vector2 point = roundPos({i,j});

            func(point);
        }
    }
}

void GlobalTerrain::forEachPosTest(std::function<void(const Vector2&)> func, const Vector2& pos, int radius)
{
    int units = radius/Block::BLOCK_DIMEN;
    Vector2 center = roundPos(pos);

    //int i = rand()%4;
    for (int x = 0; x <= units; x++ )
    {
        int height = sqrt(radius*radius - pow(x*Block::BLOCK_DIMEN,2))/Block::BLOCK_DIMEN;
        for (int y = 0; y <= height; y++)
        {

            for (int i = 2*(x == 0); i < 4 - (x == 0 && y == 0); i += ((y == 0) + 1))
            {
                //order is botRight quadrant, topRight, botLeft, topLeft
                //so for example if x = 1 and y = 1, we do center + (x,y), then center + (x,-y), center + (-x,y), center + (-x,-y);

                //if x == 0, only do the last two quadrants (botLeft, topLeft, basically only y matters)
                //if y == 0, only do the 1st and 3rd quadrants (botRight, botLeft, basically only x matters)
                //and if x and y == 0, only do one quadrant, (botLeft, arbitrary though), since that is the center
                Vector2 point = center + Vector2(x*(1 - i/2*2), y*(1 - i%2*2)) * Block::BLOCK_DIMEN;
                func(point);
                //func(point);
                //func(point);
                //func(point);
            }
        }
    }
}

void GlobalTerrain::forEachPosPerim(std::function<void(const Vector2&)> func, const Vector2& pos, int radius)
{
    int units = radius/Block::BLOCK_DIMEN;
    Vector2 center = roundPos(pos);
    int i = rand()%4;

    for (int x = 0; x <= units; x++ )
    {
        int height = sqrt(radius*radius - pow(x*Block::BLOCK_DIMEN,2))/Block::BLOCK_DIMEN;
        int oldHeight = sqrt(radius*radius - pow(std::min(units,x+1)*Block::BLOCK_DIMEN,2))/Block::BLOCK_DIMEN;
        for (int y = oldHeight; y <= height; y++)
        {

            //for (int i = 2*(x == 0); i < 4 - (x == 0 && y == 0); i += ((y == 0) + 1))
            {
                //order is botRight quadrant, topRight, botLeft, topLeft
                //so for example if x = 1 and y = 1, we do center + (x,y), then center + (x,-y), center + (-x,y), center + (-x,-y);

                //if x == 0, only do the last two quadrants (botLeft, topLeft, basically only y matters)
                //if y == 0, only do the 1st and 3rd quadrants (botRight, botLeft, basically only x matters)
                //and if x and y == 0, only do one quadrant, (botLeft, arbitrary though), since that is the center
                Vector2 point = center + Vector2(x*(1 - i/2*2), y*(1 - i%2*2)) * Block::BLOCK_DIMEN;
                func(point);
            }
        }
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
