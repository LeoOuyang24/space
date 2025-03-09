#ifndef BLOCKS_H_INCLUDED
#define BLOCKS_H_INCLUDED

#include <math.h>
#include <memory>
#include <functional>


#include <raylib.h>
#include <raymath.h>

struct Block
{
    constexpr static int BLOCK_DIMEN=10;

    Vector2 pos;
    Color color;

    void render();
};

struct Particle
{
    Color color;
    Vector2 pos;
};

Vector2 roundPos(const Vector2& vec);

template<>
struct std::hash<Vector2>
{
    size_t operator()(const Vector2& vec) const
    {

        Vector2 round = roundPos(vec);

        int a = (round.x >= 0 ? 2*round.x : -2*round.x-1);
        int b = (round.y >= 0 ? 2*round.y : -2*round.y-1);
        return (a + b) * (a + b + 1) / 2 + a;
    }
};

template<>
struct std::equal_to<Vector2>
{
    bool operator()(const Vector2& v1, const Vector2& v2) const
    {
        return std::hash<Vector2>{}(v1) == std::hash<Vector2>{}(v2);
    }
};

struct GlobalTerrain
{
    typedef std::unordered_map<Vector2,std::unique_ptr<Block>> TerrainMap;
    TerrainMap terrain;

    void generatePlanet(const Vector2& center, int radius );
    void generatePlanets();

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

    //remove all blocks in area
    void remove(const Vector2& pos, int radius);
    bool blockExists(const Vector2& pos); //true if block is at position

    void render();
};

#endif // BLOCKS_H_INCLUDED
