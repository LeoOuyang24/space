#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>


#include "../headers/debug.h"
#include "../headers/blocks.h"
#include "../headers/resources_math.h"
#include "../headers/game.h"
#include "rlgl.h"


void Block::render()
{
    //DrawPoly(pos,4,Block::BLOCK_DIMEN,0,color);
    //DrawCircle(pos.x,pos.y,2,RED);

    //DrawRectangle(pos.x - BLOCK_DIMEN/2,pos.y - BLOCK_DIMEN/2,BLOCK_DIMEN,BLOCK_DIMEN,color);
}

Vector2 roundPos(const Vector2& vec)
{
    return Vector2(floor(vec.x/Block::BLOCK_DIMEN)*Block::BLOCK_DIMEN,
                   (floor(vec.y/Block::BLOCK_DIMEN) )*Block::BLOCK_DIMEN);
}

Vector2 nearestPos(const Vector2& vec)
{
    float remainX = fmod(vec.x,Block::BLOCK_DIMEN);
    float remainY = fmod(vec.y,Block::BLOCK_DIMEN);
    return Vector2(vec.x - remainX + (remainX >= Block::BLOCK_DIMEN/2)*Block::BLOCK_DIMEN,
                    vec.y - remainY + (remainY >= Block::BLOCK_DIMEN/2)*Block::BLOCK_DIMEN);
}

Terrain::Terrain()
{
    blocksTexture = LoadRenderTexture(MAX_WIDTH*Block::BLOCK_DIMEN,MAX_WIDTH*Block::BLOCK_DIMEN);
    BeginTextureMode(blocksTexture);
        ClearBackground(BLANK);
    EndTextureMode();
}

Rectangle Terrain::getBlockRect(const Vector2& pos)
{
    Vector2 rounded = roundPos(pos);
    return {rounded.x,rounded.y,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN};
}

void Terrain::addBlock(const Vector2& pos, Block block)
{
    int index = pointToIndex(pos);
    //std::cout << index << "\n";

    if (index < 0 || index >= pointToIndex({MAX_WIDTH*Block::BLOCK_DIMEN,MAX_WIDTH*Block::BLOCK_DIMEN}))
    {
        return;
    }
    if (index >= terrain.size())
    {
        terrain.resize(index + 1);
    }
    terrain[index] = block;

    Vector2 rounded = roundPos(pos);
    BeginTextureMode(blocksTexture);
        DrawRectangle(rounded.x,blocksTexture.texture.height -rounded.y,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,block.color);
        //DrawPoly({rounded.x + Block::BLOCK_DIMEN/2,blocksTexture.texture.height -rounded.y + Block::BLOCK_DIMEN/2},10,Block::BLOCK_DIMEN*2,0,block.color);
        //DrawCircle(rounded.x + Block::BLOCK_DIMEN/2,blocksTexture.texture.height -rounded.y + Block::BLOCK_DIMEN/2,Block::BLOCK_DIMEN*2,block.color );
    EndTextureMode();
}

void Terrain::remove(const Vector2& pos, int radius)
{
    std::vector<Vector2> blocks;
    forEachPos([this,&blocks](const Vector2& pos){
               //DrawCircle(pos.x,pos.y,10,GREEN);
               if (blockExists(pos))
               {
                   blocks.push_back(pos);
                  terrain[pointToIndex(pos)] = {Color(0,0,0,0)};

               }
               },pos,radius);
    BeginTextureMode(blocksTexture);
        rlSetBlendMode(BLEND_CUSTOM);
        for (int i = 0; i < blocks.size(); i ++)
        {
            //DrawCircle(blocks[i].x + Block::BLOCK_DIMEN/2,blocksTexture.texture.height -blocks[i].y + Block::BLOCK_DIMEN/2,Block::BLOCK_DIMEN*2,BLANK );
            //DrawPoly({blocks[i].x + Block::BLOCK_DIMEN/2,blocksTexture.texture.height -blocks[i].y + Block::BLOCK_DIMEN/2},10,Block::BLOCK_DIMEN*2,0,BLANK);
            DrawRectangle(blocks[i].x,blocksTexture.texture.height - blocks[i].y,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,Fade(BLACK, 0.0f));
        }
        rlSetBlendMode(BLEND_ALPHA);
    EndTextureMode();
}

void Terrain::clear()
{
    terrain.clear();
    BeginTextureMode(blocksTexture);
        ClearBackground(BLANK);
    EndTextureMode();
}


Vector2 Terrain::pointBoxEdgeIntersect(const Vector2& a, const Vector2& dir,int dimens)
{
    //std::cout << dir.x << " " << dir.y << "\n";
    Vector2 rounded = roundPos(a);

    if (dir.y == 0) //if line is horizontal, figure out whether we intersect with the left or right edge
    {
        return {rounded.x + dimens*((dir.x > 0) - (dir.x < 0 && a.x == rounded.x)),a.y};
    }
    else
    {
        //calculate the border of the rectangle based on our vertical direction.
        //vertBorder is the top or bottom edge of the rectangle, depending on if our direction is up or down
        float vertBorder = (dir.y < 0) ? //if going up
                                rounded.y == a.y ? //because we always round down, a vector that is going up but exactly on the bottom border of a rect will be rounded to the bottom border as opposed to the top border.
                                    rounded.y - dimens : //so we adjust it
                                    rounded.y    :       //otherwise we round
                                rounded.y + dimens;     //if going down, it's the bottom border

        float x = (vertBorder - a.y)/dir.y*dir.x+ a.x; //the x coordinate if "a" continues in direction "dir" until it hits either the top or bottom edge
        Debug::addDeferRender([dir,rounded,x,vertBorder,dimens](){

                              DrawRectangle(rounded.x,vertBorder,dimens,dimens,WHITE);

                              });

        float left = rounded.x - dimens*(rounded.x == a.x && dir.x < 0);
        float right = left + dimens;
        x = std::min(std::max(x,left),right); //clamp x to inside the rectangle


        float y = (x - a.x) != 0 ? ((x - a.x)*(dir.y/dir.x) + a.y) : vertBorder; //once clamped, find the corresponding y,use "vertborder" if the line is vertical

        return {x,y};
    }
}

PossiblePoint Terrain::lineIntersectWithTerrain(const Vector2& a, const Vector2& b) //return the point closest to "a" that intersects with the terrain
{
    Vector2 current = a;
    while (current != b)
    {
        if (blockExists(current))
        {
            Rectangle block = getBlockRect(current);

            PossiblePoint pos = segmentIntersectRect(current,b,block);
            if (pos.exists)
            {
                return pos;
            }
        }

        current = Vector2MoveTowards(current,b,Block::BLOCK_DIMEN);
    }
    return {false};
}

PossiblePoint Terrain::lineTerrainIntersect(const Vector2& a, const Vector2& b)
{

    Vector2 newA = a;
    Vector2 dir = Vector2Normalize(b - a);
    if (blockExists(newA) && !Vector2Equals(dir,{0,0}))
    {
        Vector2 oldA;
       // newA = indexToPoint(pointToIndex(newA));
        while (blockExists(newA))
        {
            oldA = newA;
            newA -= dir*Block::BLOCK_DIMEN;
            //std::cout << dir.x << " " << dir.y << "\n";
        }

        PossiblePoint surface = segmentIntersectRect(oldA,newA,getBlockRect(oldA));
        if (surface.exists)
        {
            newA = surface.pos - dir;
        }
    }
    return lineBlockIntersect(newA,b);
}

PossiblePoint Terrain::lineBlockIntersect(const Vector2& a, const Vector2& b)
{

    if (pointToIndex(a) == pointToIndex(b)) //if a and b are in teh same box, it comes down to whether or not there's empty space there
    {
        return {blockExists(a),blockExists(a) ? a : b};
    }

    Vector2 dir = Vector2Normalize(b - a);

    Vector2 current = a;

    Vector2 nudge = current + dir*.001;

    int i = 0;

    bool past = false; //true if we have gone past b and not hit a wall
    //loop until we have gone past b or we hit a wall
    while (!blockExists(nudge) && i < 10 && !past)
    {
       // i ++;
        current = pointBoxEdgeIntersect(current,dir,Block::BLOCK_DIMEN);
       // std::cout << curre
        //std::cout << pointToIndex(nudge) << " " << current.x << " " << current.y << " " << dir.x << " " << dir.y <<"\n";
        nudge = current + dir*.001;
        past = Vector2DistanceSqr(current,a) > Vector2DistanceSqr(b,a);

       // std::cout << "CURRENT: " << current.x  << " " << current.y << " " << a.x << " " << a.y << "\n";
    }
   // std::cout << "done\n";

    return {!past,past ? b : current};

}
size_t Terrain::pointToIndex(const Vector2& vec)
{
    //std::cout <<static_cast<int>(vec.y)/Block::BLOCK_DIMEN*MAX_WIDTH + static_cast<int>(vec.x)/Block::BLOCK_DIMEN << "\n";
    return static_cast<int>(vec.y)/Block::BLOCK_DIMEN*MAX_WIDTH + static_cast<int>(vec.x)/Block::BLOCK_DIMEN;
}

Vector2 Terrain::indexToPoint(size_t index)
{
    return {index%MAX_WIDTH*Block::BLOCK_DIMEN,index/MAX_WIDTH*Block::BLOCK_DIMEN};
}

void Terrain::generatePlanet(const Vector2& center, int radius, const Color& color )
{

    forEachPos([this,&center,radius,color](const Vector2& pos){

                    addBlock(pos,{color});

               },center,radius);

}


void Terrain::generatePlanets()
{
    const int range = 1000;

    terrain.clear();

    int numPlanets = rand()%3 + 3;

    for (int i = 0; i < numPlanets; i++)
    {
        Vector2 randomCenter = Vector2(rand()%MAX_WIDTH,rand()%MAX_WIDTH);
        int radius = rand()%(100) + 100;

        generatePlanet(randomCenter,radius,RED);
    }
}

void Terrain::generateRect(const Rectangle& rect, const Color& color)
{
    Vector2 origin = roundPos({rect.x,rect.y});

    for (int i = origin.x; i <= rect.x + rect.width; i += Block::BLOCK_DIMEN)
    {
        for (int j = origin.y; j <= rect.y + rect.height; j+= Block::BLOCK_DIMEN)
        {
            addBlock({i,j},{color});
        }
    }
}

void Terrain::generateRightTriangle(const Vector2& corner, float height, const Color& color)
{
    Vector2 origin = roundPos({corner.x,corner.y - height});
    int modHeight = height/Block::BLOCK_DIMEN;


    for (int i = 0; i <= modHeight; i += 1)
    {
        for (int j = i; j <= modHeight; j+= 1)
        {
            Vector2 pos = origin + Vector2(i,j)*Block::BLOCK_DIMEN;
            addBlock({i,j},{color});
        }
    }
}

bool Terrain::blockExists(const Vector2& pos)
{
    return pointToIndex(pos) < terrain.size() && terrain[pointToIndex(pos)].color.a != 0;
}



void Terrain::render(int z)
{

   Vector3 white = Vector3{255,255,255}*std::max(0.0f,std::min(1.0f,(1 - static_cast<float>(z - Globals::Game.camera.position.z - Globals::Game.CAMERA_Z_DISP)/Globals::MAX_Z)));
   //std::cout << (1 - static_cast<float>(z - Globals::Game.currentZ)/Globals::MAX_Z) << "\n";

   Color balls = {white.x,white.y,white.z,255}; //breaking_bad_crawl_space.gif
   DrawBillboardPro(Globals::Game.camera,blocksTexture.texture,Rectangle(0,0,blocksTexture.texture.width,blocksTexture.texture.height)
                    ,Vector3(blocksTexture.texture.width/2,blocksTexture.texture.height/2,z),Vector3(0,-1,0),
                    Vector2(blocksTexture.texture.width,blocksTexture.texture.height),Vector2(blocksTexture.texture.width/2,blocksTexture.texture.height/2),
                    0,balls);

}
