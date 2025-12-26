#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>


#include "../headers/debug.h"
#include "../headers/blocks.h"
#include "../headers/resources_math.h"
#include "../headers/game.h"
#include "rlgl.h"

Shader Terrain::GravityFieldShader;

Vector2 Terrain::roundPos(const Vector2& vec, int blockDimen)
{
    return Vector2(floor(vec.x/blockDimen)*blockDimen,
                   (floor(vec.y/blockDimen) )*blockDimen);
}

Vector2 Terrain::nearestPos(const Vector2& vec)
{
    float remainX = fmod(vec.x,Block::BLOCK_DIMEN);
    float remainY = fmod(vec.y,Block::BLOCK_DIMEN);
    return Vector2(vec.x - remainX + (remainX >= Block::BLOCK_DIMEN/2)*Block::BLOCK_DIMEN,
                    vec.y - remainY + (remainY >= Block::BLOCK_DIMEN/2)*Block::BLOCK_DIMEN);
}

Terrain::Terrain()
{
    blocksTexture = LoadRenderTexture(MAX_TERRAIN_SIZE,MAX_TERRAIN_SIZE);
    //terrain.data.reserve(MAX_WIDTH*MAX_WIDTH*TerrainMap::PALETTE_SIZE);
//    upScaled.resize(upScaled.maxWidth*upScaled.maxWidth);
    //gravityFields.resize(gravityFields.maxWidth*gravityFields.maxWidth);

    BeginTextureMode(blocksTexture);
        ClearBackground(BLANK);
    EndTextureMode();
}

Rectangle Terrain::getBlockRect(const Vector2& pos)
{
    Vector2 rounded = roundPos(pos);
    return {rounded.x,rounded.y,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN};
}

void Terrain::addBlock(const Vector2& pos, const Block& block)
{
    int index = pointToIndex(pos);

    if (index < 0 || index >= pointToIndex({MAX_TERRAIN_SIZE - 1,MAX_TERRAIN_SIZE - 1}))
    {
        return;
    }
    if (index >= terrain.size())
    {
        terrain.resize((index + 1)*TerrainMap::PALETTE_SIZE);
    }
    terrain.setVal(index,block.type);
    //std::cout << pos << "  " << index << " "<< terrain.size()<< "\n";

    Color color;
    switch (block.type)
    {
    case AIR:
        color = {0,0,0,0};
        break;
    case LAVA:
        color = {255,0,0,255};
        break;
    case ANTI:
        color = WHITE;
        break;
    case SOLID:
    default:
        color = block.color;
        break;
    }

    Vector2 rounded = roundPos(pos);
    BeginTextureMode(blocksTexture);

            for (int i = 0; i < 9; i ++)
            {
                Vector2 neighbor = {rounded.x + Block::BLOCK_DIMEN*(i%3 - 1),rounded.y + Block::BLOCK_DIMEN*(i/3 - 1)};
                if (neighbor.x >= 0 && neighbor.y >= 0 &&
                    neighbor.x < blocksTexture.texture.width && neighbor.y < blocksTexture.texture.height &&
                    !blockExists(neighbor))
                    {
                        DrawRectangle(neighbor.x,blocksTexture.texture.height - neighbor.y -  Block::BLOCK_DIMEN,
                                      Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,
                                      Color(color.r*.5,color.g*.5,color.b*.5,255));
                    }
            }
            DrawRectangle(rounded.x,blocksTexture.texture.height - rounded.y - Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,color);
    EndTextureMode();
}

void Terrain::remove(const Vector2& pos, int radius)
{
    forEachPos([this](const Vector2& pos){
               //DrawCircle(pos.x,pos.y,10,GREEN);
                //clear the texture
                BeginTextureMode(blocksTexture);
                    rlSetBlendMode(BLEND_CUSTOM);
                        DrawRectangle(pos.x,blocksTexture.texture.height - pos.y,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN,Fade(BLACK, 0.0f));
                    rlSetBlendMode(BLEND_ALPHA);
                EndTextureMode();
               if (blockExists(pos))
               {
                  //terrain[pointToIndex(pos)] = false;//{Color(0,0,0,0)};
                  terrain.setVal(pointToIndex(pos),BlockType::AIR);
               }
               },pos,radius);

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
    if (dir.x == 0 && dir.y == 0) [[unlikely]]
    {
        return a;
    }

    //std::cout << dir.x << " " << dir.y << "\n";
    Vector2 rounded = roundPos(a);

    if (dir.y == 0) //if line is horizontal, figure out whether we intersect with the left or right edge
    {
        //if going to the right, we intersect with the right edge, otherwise left
        //if we are already on a left edge, make sure we go back one "dimens"
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

        float x = (vertBorder - a.y)/dir.y*dir.x+ a.x; //the x coordinate if "a" continues in direction "dir" until it hits either the top or bottom edge, regardless of left-right border

        float left = rounded.x - dimens*(rounded.x == a.x && dir.x < 0); //left border
        float right = left + dimens; //right border
        x = std::min(std::max(x,left),right); //clamp x to inside the rectangle


        float y = (x - a.x) != 0 ? ((x - a.x)*(dir.y/dir.x) + a.y) : vertBorder; //once clamped, find the corresponding y,use "vertborder" if the line is vertical

        return {x,y};
    }
}

/*PossiblePoint Terrain::lineIntersectWithTerrain(const Vector2& a, const Vector2& b) //return the point closest to "a" that intersects with the terrain
{
    Vector2 current = a;
    while (current != b)
    {
        if (isBlockType(current,SOLID))
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
}*/

Vector2 Terrain::lineTerrainIntersect(const Vector2& a, const Vector2& b, bool isSolid)
{

    Vector2 newA = a;
    Vector2 dir = Vector2Normalize(b - a);
    if ( (isSolid && isBlockType(newA,SOLID) || blockExists(newA))  && !Vector2Equals(dir,{0,0}))
    {
        Vector2 oldA;
        while ((isSolid && isBlockType(newA,SOLID) || blockExists(newA))) //move "A" backwards until we encounter non-solid block
        {
            oldA = newA;
            newA -= dir*Block::BLOCK_DIMEN;
            //std::cout << dir.x << " " << dir.y << "\n";
        }

        PossiblePoint surface = segmentIntersectRect(oldA,newA,getBlockRect(oldA)); //if newA is stuck on the surface of a block, move a little more
        if (surface.exists)
        {
            newA = surface.pos - dir;
        }
    }
    return lineBlockIntersect(newA,b,isSolid);
}

Vector2 Terrain::lineBlockIntersect(const Vector2& a, const Vector2& b, bool isSolid)
{

    if (pointToIndex(a) == pointToIndex(b)) //if a and b are in teh same box, it comes down to whether or not there's empty space there
    {
        return (isSolid && isBlockType(a,SOLID,true) || blockExists(a,true)) ? a : b;
    }

    Vector2 dir = Vector2Normalize(b - a);

    Vector2 current = a;
    bool past = false;
    //loop until we have gone past b or we hit a wall
    while (((isSolid && !isBlockType(current,SOLID,true)) || !blockExists(current,true)) && !past)
    {
        current = pointBoxEdgeIntersect(current,dir,Block::BLOCK_DIMEN);
        past = abs(current.y - a.y) > abs(b.y - a.y) || abs(current.x - a.x) > abs(b.x - a.x);

        /*Debug::addDeferRender([current,this](){

                              DrawCircle3D(toVector3(current),2,{},0,blockExists(current,true) ? RED : WHITE);

                              });*/
    }

    return past ? b : current;

}
size_t Terrain::pointToIndex(const Vector2& vec,int blockDimen, int maxWidth)
{
    //std::cout <<static_cast<int>(vec.y)/Block::BLOCK_DIMEN*MAX_WIDTH + static_cast<int>(vec.x)/Block::BLOCK_DIMEN << "\n";
    return static_cast<int>(vec.y)/blockDimen*maxWidth + static_cast<int>(vec.x)/blockDimen;
}

Vector2 Terrain::indexToPoint(size_t index,int blockDimen, int maxWidth)
{
    return {index%maxWidth*blockDimen,index/maxWidth*blockDimen};
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

bool Terrain::blockExists(const Vector2& pos, bool checkEdge)
{

    size_t index = pointToIndex(pos);
    if (index >= terrain.size())
    {
        return false;
    }

    bool answer = terrain[index] != AIR;
    if ( !answer && checkEdge)
    {
        if ( static_cast<int>(pos.x) % Block::BLOCK_DIMEN == 0)
        {
            answer = (pos.x > 0 && terrain[index - 1] != AIR);
        }
        if (!answer && static_cast<int>(pos.y) % Block::BLOCK_DIMEN == 0)
        {
            answer = (pos.y > 0 && terrain[index - MAX_WIDTH] != AIR);
        }
    }

    return answer;
}

bool Terrain::isBlockType(const Vector2& pos, BlockType type, bool checkEdge)
{
    size_t index = pointToIndex(pos);
    if (index >= terrain.size())
    {
        return false;
    }
    bool answer = terrain[index] == type;
    if ( !answer && checkEdge)
    {
        if ( static_cast<int>(pos.x) % Block::BLOCK_DIMEN == 0)
        {
            answer = (pos.x > 0 && terrain[index - 1] == type);
        }
        if (!answer && static_cast<int>(pos.y) % Block::BLOCK_DIMEN == 0)
        {
            answer = (pos.y > 0 && terrain[index - MAX_WIDTH] == type);
        }
    }
    return answer;

}

void Terrain::render(int i, int z)
{

   //Vector3 white = Vector3{255,255,255}*std::max(0.0f,std::min(1.0f,(1 - static_cast<float>(z - Globals::Game.camera.position.z - Globals::Game.CAMERA_Z_DISP)/Globals::MAX_Z)));

   Vector3 white = Vector3(255,255,255)*pow(.5,i);

   Color balls = {white.x,white.y,white.z,255}; //can't do math with raylib colors breaking_bad_crawl_space.gif

   //BeginShaderMode(GravityFieldShader);

   DrawBillboardPro(Globals::Game.camera,blocksTexture.texture,Rectangle(0,0,blocksTexture.texture.width,blocksTexture.texture.height)
                    ,Vector3(blocksTexture.texture.width/2,blocksTexture.texture.height/2,z),Vector3(0,-1,0),
                    Vector2(blocksTexture.texture.width,blocksTexture.texture.height),Vector2(blocksTexture.texture.width/2,blocksTexture.texture.height/2),
                    0,balls);
    //EndShaderMode();
}
