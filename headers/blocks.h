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

    //run a function for each position within a distance
    void forEachPos(std::function<void(const Vector2&)> func, const Vector2& center, int radius);
    void forEachPosTest(std::function<void(const Vector2&)> func, const Vector2& center, int radius);
    //run a function for each position that is EXACTLY "radius" away
    void forEachPosPerim(std::function<void(const Vector2&)> func, const Vector2& center, int radius);

    //remove all blocks in area
    void remove(const Vector2& pos, int radius);
    bool blockExists(const Vector2& pos); //true if block is at position

    void render();
};

#endif // BLOCKS_H_INCLUDED
