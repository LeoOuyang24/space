#ifndef BLOCKS_H_INCLUDED
#define BLOCKS_H_INCLUDED

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <iostream>
#include <functional>


#include <raylib.h>
#include <raymath.h>
#include "resources_math.h"
#include "debug.h"

enum BlockType
{
    AIR = 0,
    SOLID,
    LAVA,
    ANTI,
    WATER,
    BLOCK_TYPES
};

struct Block
{
    constexpr static int BLOCK_DIMEN=3;

    Color color;
    BlockType type = SOLID;
};

struct TerrainMap
{
    //how many bits correspond to one block
    constexpr static size_t PALETTE_SIZE = std::max(1.0,ceil(std::log2(static_cast<uint8_t>((BLOCK_TYPES)))));
    const int blockDimen;
    const int maxWidth;

    std::vector<bool> data;

    void setVal(size_t index,BlockType val)
    {
        for (size_t i = 0; i < PALETTE_SIZE; i++)
        {
            data[index*PALETTE_SIZE + i] = (val >> i ) % 2;
        }
    }
    BlockType operator[] (size_t index) //index will be converted to a multiple of PALETTE SIZE. so index 3 = 3*PALETTE_SIZE for the 4th block
    {
        if (index >= size())
        {
            return AIR;
        }
        uint8_t p = AIR;
        for (size_t i = 0; i < PALETTE_SIZE; i ++)
        {
            p += data[index*PALETTE_SIZE + i] << i;
        }
        return static_cast<BlockType>(p);
    }
    size_t size() //number of blocks, not number of bits
    {
        return data.size()/PALETTE_SIZE;
    }
    void resize(size_t newSize)
    {
        data.resize(newSize);
    }
    void clear()
    {
        data.clear();
    }
};

//returns a position and the type at that position
struct PossibleBlock
{
    BlockType type = AIR;
    Vector2 pos;
};

struct PhysicsBody;
struct Shape;

struct Terrain
{
    static constexpr int MAX_WIDTH = 3000; //maximum number of blocks in the width direction
    static constexpr int PIXEL_SIZE = 1;
    static constexpr float PIXEL_RATIO = PIXEL_SIZE/static_cast<float>(Block::BLOCK_DIMEN); //multiply a world coordinate by this to convert it to texture coords 
    static constexpr int MAX_TERRAIN_SIZE = MAX_WIDTH*Block::BLOCK_DIMEN; //distance in width direction in pixels

    static Shader GravityFieldShader;

    //typedef std::vector<Block> TerrainMap;
    //typedef std::vector<bool> TerrainMap;
    TerrainMap terrain{Block::BLOCK_DIMEN,MAX_WIDTH};
    RenderTexture blocksTexture;
    RenderTexture gravityTexture;
    Terrain();
    void cleanUp();

    static int count;
    void addBlock(const Vector2& pos, const Block& block);
    //remove all blocks in area
    void remove(const Vector2& pos, int radius);
    void clear();

    void generatePlanet(const Vector2& center, int radius, const Color& color );
    void generatePlanets();

    void generateRect(const Rectangle& r, const Color& color);

    void generateRightTriangle(const Vector2& corner, float height, const Color& color); //creates an isoscles right triangle

     //returns the point that an object moving from "a" to "b" would stop at after hitting terrain, returns "b" if there is a clear path
     //isSolid = true if we wish to stop at a SOLID block. False, if we want to stop at any block that is not AIR
    Vector2 lineBlockIntersect(const Vector2& a, const Vector2& b, bool isSolid = true);
    //same as above, except we'll move "a" out of terrain first.
    Vector2 lineTerrainIntersect(const Vector2& a, const Vector2& b, bool isSolid = true);

    size_t pointToIndex(const Vector2& vec,int blockDimen = Block::BLOCK_DIMEN, int maxWidth = MAX_WIDTH);
    Vector2 indexToPoint(size_t index,int blockDimen = Block::BLOCK_DIMEN, int maxWidth = MAX_WIDTH);
    Vector2 roundPos(const Vector2& vec, int blockDimen = Block::BLOCK_DIMEN);
    Vector2 nearestPos(const Vector2& vec);
    Rectangle getBlockRect(const Vector2& vec); //returns the rectangle of a block at that position
    Vector2 pointBoxEdgeIntersect(const Vector2& a, const Vector2& dir, int dimens); //returns point of intersection with block that "a" is in if we move in the "dir" direction
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

    /**
     * @brief Given a position, returns whether that position is not AIR
     *
     * @param pos, the position to check
     * @param checkPlanets, true if we want to check planets
     * @param checkEdge, true to check neighbors as well
     *
     * @returns true if position does not contain air
     */
    bool blockExists(const Vector2& pos, bool checkPlanets = true, bool checkEdge = true); 
    /**
     * @brief Given a position, returns whether that position is a certain type
     *
     * @param pos, the position to check
     * @param checkPlanets, true if we want to check planets
     * @param checkEdge, true to check neighbors as well
     *
     * @returns true if position contains the provided type
     */
    bool isBlockType(const Vector2& pos,BlockType type, bool checkPlanets = true, bool checkEdge = true); //true if block at position is "type".

    /**
     * @brief returns true if the perimeter of a Shape collides with terrain. Always checks Planets
     * 
     * @param shape 
     * @return true 
     */
    bool blockExists(const Shape& shape);
    /**
     * @brief returns true if shape perimeter collides with a block of a certain type. Always checks Planets
     * 
     * @param shape 
     * @param type 
     * @return true 
     */
    bool isBlockType(const Shape& shape, BlockType type); 

    void addPlanet(PhysicsBody& planet, BlockType type);

    //"i" is index relative to current layer (0 if we are on layer 2 and rendering layer 2)
    //"z" is z coordinate to render at (absolute)
    void render(int i = 0, int z = 0);


    struct EntityPlanet
    {
        std::weak_ptr<PhysicsBody> ptr;
        BlockType type;
    };
    std::vector<EntityPlanet> planets;

    void drawBlocks();
    void endDrawBlocks();
private:
    typedef std::function<bool(BlockType)> CheckFunc; //used by various terrain functions to check for the type of a block
    static constexpr auto blockExistsCheck = [](BlockType other){return other != AIR;}; 
    static constexpr auto isBlockTypeCheck = [](BlockType type){return [type](BlockType other){ return other == type;};}; 

    /**
     * @brief Given a position, returns whether that position has a block of a certain type. This is the workhorse function of blockExists and isBlockType
     *
     * @param pos, the position to check
     * @param checkPlanets, true if we want to check planets. You almost always want this to be true, but sometimes it can be expensive (O(n), where n is the number of planets)
     * @param check, a function that, given a blocktype, returns true or false. This can allow us to check if a block at a position has a certain type or if it is NOT a certain type
     * @param checkEdge, false if we only care about the block at the actual position, as opposed to adjacent blocks as well. You almost always want this to be true
     * 
     * @returns true if "check" returns true on the block at "pos" or any neighboring points if its on an edge
     */
    bool checkBlocks(const Vector2& pos, bool checkPlanets, CheckFunc check, bool checkEdge = true);
    bool checkBlocks(const Shape& shape, bool checkPlanets, CheckFunc check); //same as above for a shape

    /**
     * @brief Returns the point between "a" and "b" if "a" were to move to "b" and stop at terrain. "b" if there is no terrain in the way
     * 
     * @param a 
     * @param b 
     * @param isSolid 
     * @return Vector2 
     */
    Vector2 lineBlockIntersect(const Vector2& a, const Vector2& b, CheckFunc check);
    //same as above, except we'll move "a" out of terrain first.
    Vector2 lineTerrainIntersect(const Vector2& a, const Vector2& b, CheckFunc check);

    bool isDrawing = false; //true if BeginTextureMode has been called, allowing us to batch draw planets

};



#endif // BLOCKS_H_INCLUDED
