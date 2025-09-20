#ifndef BLOCKS_H_INCLUDED
#define BLOCKS_H_INCLUDED

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <functional>


#include <raylib.h>
#include <raymath.h>
#include "../headers/resources_math.h"

struct Block
{
    constexpr static int BLOCK_DIMEN=3;

    Color color;

    void render();
};

Vector2 roundPos(const Vector2& vec);
Vector2 nearestPos(const Vector2& vec);

struct Terrain
{
    static constexpr int MAX_WIDTH = 1000; //maximum number of blocks in the width direction

    typedef std::vector<Block> TerrainMap;
    TerrainMap terrain;

    RenderTexture blocksTexture;

    Terrain();


    void addBlock(const Vector2& pos, Block block);
    //remove all blocks in area
    void remove(const Vector2& pos, int radius);
    void clear();

    void generatePlanet(const Vector2& center, int radius, const Color& color );
    void generatePlanets();

    void generateRect(const Rectangle& r, const Color& color);

    void generateRightTriangle(const Vector2& corner, float height, const Color& color); //creates an isoscles right triangle

    PossiblePoint lineIntersectWithTerrain(const Vector2& a, const Vector2& b); //return the point closest to "a" that intersects with the terrain
    PossiblePoint lineBlockIntersect(const Vector2& a, const Vector2& b); //return the point of intersection between the line a-b and the block closest to b as well as whether there even was a collision
    PossiblePoint lineTerrainIntersect(const Vector2& a, const Vector2& b); //same as above except it'll loop until it can return a point that is out of terrain

    size_t pointToIndex(const Vector2& vec);
    Vector2 indexToPoint(size_t index);
    Rectangle getBlockRect(const Vector2& vec); //returns the rectangle of a block at that position
    Vector2 pointBoxEdgeIntersect(const Vector2& a, const Vector2& dir, int dimens);
    //run a function ( (const Vector2&) -> void or bool) for each position within a distance
    //"edge" = true if we only care points along the edge
    //if the function returns true, terminate early
    template<typename T>
    void forEachPos(T func, const Vector2& pos, int radius, bool edge = false)
    {
        int units = radius/Block::BLOCK_DIMEN;
        Vector2 center = roundPos(pos);
        for (int x = 0; x <= units; x++ )
        {
            int height = sqrt(radius*radius - pow(x*Block::BLOCK_DIMEN,2))/Block::BLOCK_DIMEN;
            //0 if we are processing all points
            //otherwise, start at the edge
            int start =  edge*sqrt(radius*radius - pow(std::min(units,x+1)*Block::BLOCK_DIMEN,2))/Block::BLOCK_DIMEN;
            for (int y = start; y <= height; y++)
            {
                for (int i = 2*(x == 0); i < 4 - (x == 0 && y == 0); i += ((y == 0) + 1))
                {
                    //order is botRight quadrant, topRight, botLeft, topLeft
                    //so for example if x = 1 and y = 1, we do center + (x,y), then center + (x,-y), center + (-x,y), center + (-x,-y);

                    //if x == 0, only do the last two quadrants (botLeft, topLeft, basically only y matters)
                    //if y == 0, only do the 1st and 3rd quadrants (botRight, botLeft, basically only x matters)
                    //and if x and y == 0, only do one quadrant, (botLeft, arbitrary though), since that is the center
                    Vector2 point = center + Vector2(x*(1 - i/2*2), y*(1 - i%2*2)) * Block::BLOCK_DIMEN;
                   if constexpr (std::is_same<decltype(func(std::declval<const Vector2&>())),bool>::value)
                    {
                       // std::cout << "DONE\n";
                        if (func(point))
                        {
                            return;
                        }
                    }
                    else
                    {
                        func(point);
                    }
                }
            }
        }
    }
    //same as above except "func" is run on a sample of points instead.
    //"sample" is the proportion of the population we want to sample from instead. You could put a number >= 1.0 to sample more
    //if this function runs every frame.
    //the same point can be selected multiple times

    //the way this works is it will choose a random row or column and sample all points in that row/column
    //so there's definitely bias, but it's an evenly distributed bias? copium
    template<typename T>
    void forEachPosSample(T func, const Vector2& pos, int radius, float sample)
    {
        //Approximate number of points we have in our population.
        //We construct a square of Block::BLOCK_DIMEN dimensions. About PI/4 of those points will be in our circle.
        //Then we multiply by the proportion we want to sample
        int totalPoints = pow(2*radius/(Block::BLOCK_DIMEN),2)*M_PI/4 * sample;
        int i = 0;

        int units = radius/Block::BLOCK_DIMEN;
        while (i < totalPoints)
        {

            int randX = rand()%(units*2 + 1) - units;
            int height =  sqrt(radius*radius - pow(randX*Block::BLOCK_DIMEN,2))/Block::BLOCK_DIMEN;
            bool flip = rand()%2;
            for (int y = -height; y <= height; y ++)
            {
                Vector2 finalPos = pos + (flip ? Vector2(randX,y) : Vector2(y,randX))*Block::BLOCK_DIMEN;
               if constexpr (std::is_same<decltype(func(std::declval<const Vector2&>())),bool>::value)
                {
                    if (func(finalPos))
                    {
                        return;
                    }
                }
                else
                {
                    func(finalPos);
                }
                i ++;
            }
        }
    }

    bool blockExists(const Vector2& pos); //true if block is at position
    bool isSolid(const Vector2& pos); //returns true if there is terrain at position, not whether or not the pos is in the terrain

    void render(int z = 0);
};



#endif // BLOCKS_H_INCLUDED
