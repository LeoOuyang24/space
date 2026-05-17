#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>


#include "../headers/debug.h"
#include "../headers/blocks.h"
#include "../headers/resources_math.h"
#include "../headers/game.h"
#include "../headers/enemy.h"

#include "rlgl.h"

Shader Terrain::TerrainOutline;
//Terrain::CheckFunc Terrain::blockExistsCheck = [](BlockType other){return other != AIR;}; 
//Terrain::CheckFunc Terrain::isBlockTypeCheck = [](BlockType type){return [type](BlockType other){ return other == type;};}; 

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
    blocksTexture = LoadRenderTexture(MAX_TERRAIN_SIZE*PIXEL_RATIO,MAX_TERRAIN_SIZE*PIXEL_RATIO);
    //blocksTexture = LoadRenderTexture(MAX_WIDTH,MAX_WIDTH);
    //gravityTexture = LoadRenderTexture(MAX_TERRAIN_SIZE,MAX_TERRAIN_SIZE);
    //terrain.data.reserve(MAX_WIDTH*MAX_WIDTH*TerrainMap::PALETTE_SIZE);
//    upScaled.resize(upScaled.maxWidth*upScaled.maxWidth);
    //gravityFields.resize(gravityFields.maxWidth*gravityFields.maxWidth);

    cleanUp();
   /* BeginTextureMode(gravityTexture);
        ClearBackground(BLANK);
    EndTextureMode();*/
} 

void Terrain::cleanUp()
{
    terrain.clear();
    BeginTextureMode(blocksTexture);
        ClearBackground(BLANK);
    EndTextureMode();
}

Rectangle Terrain::getBlockRect(const Vector2& pos)
{
    Vector2 rounded = roundPos(pos);
    return {rounded.x,rounded.y,Block::BLOCK_DIMEN,Block::BLOCK_DIMEN};
}
void Terrain::addBlock(const Vector2& pos, const Block& block, bool draw)
{

    size_t index = pointToIndex(pos);

    if (index < 0 || index >= pointToIndex({MAX_TERRAIN_SIZE - 1,MAX_TERRAIN_SIZE - 1}))
    {
        return;
    }
    if (index >= terrain.size())
    {
        terrain.resize((index + 1)*TerrainMap::PALETTE_SIZE);
    }
    terrain.setVal(index,AIR);
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
    case WATER:
        color = BLUE;
        break;
    case SOLID:
    default:
        color = block.color;
        break;
    }
    Vector2 rounded = roundPos(pos);
    terrain.setVal(pointToIndex(rounded),block.type);
    if (draw)
    {
        rounded *= PIXEL_RATIO;
        if (!isDrawing) //false if this is was called as part of another function, so don't start/stop drawing to texture because that is controlled by the outer function
        {
            BeginTextureMode(blocksTexture);
        }
                /*for (int i = 0; i < 9; i ++)
                {
                    Vector2 neighbor = {rounded.x + PIXEL_SIZE*(i%3 - 1),rounded.y + PIXEL_SIZE*(i/3 - 1)};
                    if (neighbor.x >= 0 && neighbor.y >= 0 &&
                        neighbor.x < blocksTexture.texture.width && neighbor.y < blocksTexture.texture.height &&
                        !blockExists(neighbor/PIXEL_RATIO))
                        {
                            //Vector2 pos = {neighbor.x,blocksTexture.texture.height - neighbor.y -  PIXEL_SIZE};

                                DrawRectangle(pos.x,pos.y,
                                                pixelSize,pixelSize,
                                                Color(color.r*.5,color.g*.5,color.b*.5,255));


                        }
                }*/
                DrawRectangle(rounded.x,blocksTexture.texture.height - rounded.y - PIXEL_SIZE,PIXEL_SIZE,PIXEL_SIZE,color);
        if (!isDrawing)
        {
            EndTextureMode();
        }
    }
}

void Terrain::remove(const Vector2& pos, int radius)
{
    drawBlocks();
        rlSetBlendMode(BLEND_CUSTOM);
    //double time = GetTime();
    forEachPos([this](const Vector2& pos){
            //for (int i = 0; i < 9; i ++)
            {
                //Vector2 neighbor = pos;//{pos.x + Block::BLOCK_DIMEN*(i%3 - 1),pos.y + Block::BLOCK_DIMEN*(i/3 - 1)};
                //terrain.setVal(pointToIndex(neighbor),BlockType::AIR);
                //DrawRectangle(neighbor.x*PIXEL_RATIO,(blocksTexture.texture.height - neighbor.y)*PIXEL_RATIO -  PIXEL_SIZE,PIXEL_SIZE,PIXEL_SIZE,Fade(BLACK, 0.0f));

                addBlock(pos,{BlockType::AIR},true);

            }
               },pos,radius+Block::BLOCK_DIMEN*2*(PIXEL_RATIO));
        rlSetBlendMode(BLEND_ALPHA);
    endDrawBlocks();
   // std::cout << GetTime() - time << "\n";
}

void Terrain::clear()
{
    terrain.clear();
    cleanUp();
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

Vector2 Terrain::lineTerrainIntersect(const Vector2& a, const Vector2& b, bool isSolid)
{
    if (isSolid)
    {
        return lineTerrainIntersect(a,b,isBlockTypeCheck(SOLID));
    }
    else
    {
        return lineTerrainIntersect(a,b,static_cast<CheckFunc>(blockExistsCheck));
    }
}

Vector2 Terrain::lineBlockIntersect(const Vector2& a, const Vector2& b, bool isSolid)
{
    if (isSolid)
    {
        return lineBlockIntersect(a,b,isBlockTypeCheck(SOLID));
    }
    else
    {
        return lineBlockIntersect(a,b,static_cast<CheckFunc>(blockExistsCheck));
    }
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
    drawBlocks();

    forEachPos([this,color](const Vector2& pos){
                    addBlock(pos,{color});
               },center,radius);
    endDrawBlocks();

}


void Terrain::generatePlanets()
{
    terrain.clear();

    int numPlanets = rand()%3 + 3;
    drawBlocks();
        for (int i = 0; i < numPlanets; i++)
        {
            Vector2 randomCenter = Vector2(rand()%MAX_WIDTH,rand()%MAX_WIDTH);
            int radius = rand()%(100) + 100;

            generatePlanet(randomCenter,radius,RED);
        }
    endDrawBlocks();
}

void Terrain::generateRect(const Rectangle& rect, const Color& color)
{
    Vector2 origin = roundPos({rect.x,rect.y});

    drawBlocks();
    for (int i = origin.x; i <= rect.x + rect.width; i += Block::BLOCK_DIMEN)
    {
        for (int j = origin.y; j <= rect.y + rect.height; j+= Block::BLOCK_DIMEN)
        {
            addBlock({i,j},{color});
        }
    }
    endDrawBlocks();
}

void Terrain::generateRightTriangle(const Vector2& corner, float height, const Color& color)
{
    int modHeight = height/Block::BLOCK_DIMEN;

    drawBlocks();
    for (int i = 0; i <= modHeight; i += 1)
    {
        for (int j = i; j <= modHeight; j+= 1)
        {
            addBlock({i,j},{color});
        }
    }
    endDrawBlocks();
}

bool Terrain::blockExists(const Vector2& pos, bool checkPlanets, bool checkEdge)
{
    return checkBlocks(pos,checkPlanets,blockExistsCheck, checkEdge);
}

bool Terrain::isBlockType(const Vector2& pos, BlockType type, bool checkPlanets, bool checkEdge)
{
    return checkBlocks(pos,checkPlanets,isBlockTypeCheck(type), checkEdge);
}

bool Terrain::blockExists(const Shape& shape)
{
    return checkBlocks(shape,true,blockExistsCheck);
}

bool Terrain::isBlockType(const Shape& shape, BlockType type)
{
    return checkBlocks(shape,true,isBlockTypeCheck(type));
}

bool Terrain::checkBlocks(const Vector2& pos, bool checkPlanets, std::function<bool(BlockType)> check, bool checkEdge)
{
    if (checkPlanets)
    {
        for (auto it = planets.begin(); it != planets.end();)
        {
            if (it->ptr.lock() && check(it->type) && CheckCollisionPointShape(pos,it->ptr.lock()->getShape()))
            {
                return true;
            }
            else if (!it->ptr.lock())
            {
               it = planets.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    size_t index = pointToIndex(pos);
    if (index >= terrain.size())
    {
        return false;
    }
    bool answer = check(terrain[index]);
    if ( !answer && checkEdge)
    {
        if ( static_cast<int>(pos.x) % Block::BLOCK_DIMEN == 0)
        {
            answer = (pos.x > 0 && check(terrain[index - 1]));
        }
        if (!answer && static_cast<int>(pos.y) % Block::BLOCK_DIMEN == 0)
        {
            answer = (pos.y > 0 && check(terrain[index - MAX_WIDTH]));
        }
    }

    return answer;    
}

bool Terrain::checkBlocks(const Shape& shape, bool checkPlanets, std::function<bool(BlockType)> check)
{
    switch (shape.type)
    {
        case ShapeType::CIRCLE:
        {
            for (size_t i = 0; i < getShapePoints(ShapeType::CIRCLE); i ++)
            {
                if (checkBlocks(getIthShapePoint(shape,i),checkPlanets,check))
                {
                    return true;
                }
            }
            return false;
        }
        case ShapeType::RECT:
            for (size_t i = 0; i < getShapePoints(ShapeType::RECT); i ++) //top right, bot right, bot left, top left
            {
                Vector2 prevCorner = getIthShapePoint(shape,i); //top left
                Vector2 corner = getIthShapePoint(shape,i+1);

                Vector2 intersect = lineBlockIntersect(prevCorner,corner,check);

                //if (intersect.exists)
                if (!Vector2Equals(intersect,corner)) //something is in the way!
                {
                    return true;
                }

                prevCorner = corner;
            }
            return false;
    }
    return false;
}

Vector2 Terrain::lineTerrainIntersect(const Vector2& a, const Vector2& b, CheckFunc check)
{

    Vector2 newA = a;
    Vector2 dir = Vector2Normalize(b - a);
    //move "a" back until its not in terrain any more
    if (!Vector2Equals(dir,{0,0}))
    {
        Vector2 oldA;
        //it's kind of okay for thsi function to expensively check for planets
        //it's primarily only used for adusting angle, which only calls this twice per frame per entity
        //and it's rare for this to loop for long
        while (checkBlocks(newA,true,check))//(check(terrain[pointToIndex(newA)])) //move "A" backwards until we encounter non-solid block
        {
            oldA = newA;
            newA -= dir*Block::BLOCK_DIMEN;
        }
    }
    return lineBlockIntersect(newA,b,check);
}

Vector2 Terrain::lineBlockIntersect(const Vector2& a, const Vector2& b, CheckFunc check)
{

    if (pointToIndex(a) == pointToIndex(b)) //if a and b are in teh same box, it comes down to whether or not there's empty space there
    {
        return check(terrain[pointToIndex(a)]) ? a : b;
    }

    Vector2 dir = Vector2Normalize(b - a);

    Vector2 current = a;
    bool past = false;
    //loop until we have gone past b or we hit a wall. ignoring planets, since we calculate it later
    int i = 0;
    while (!checkBlocks(current,false,check) && !past)
    {
        current = pointBoxEdgeIntersect(current,dir,Block::BLOCK_DIMEN);
        past = abs(current.y - a.y) > abs(b.y - a.y) || abs(current.x - a.x) > abs(b.x - a.x);
        i++;
        /*if (Debug::isDebugOn())
        {
            Debug::addDeferRender([current,this,check](){

                                DrawCircle3D(toVector3(current),1,{},0,checkBlocks(current,false,check) ? RED : WHITE);

                                });
        }*/


    }

    Vector2 answer = past ? b : current;
    for (EntityPlanet& terr : planets) //calculate the answer completely separately, by only accounting for planets
        {
            if (terr.ptr.lock() && check(terr.type))
            {
                Vector2 intersect = lineShapeIntersect(terr.ptr.lock()->getShape(),a,b);
                if (Vector2DistanceSqr(intersect,a) < Vector2DistanceSqr(a,b))
                {
                    answer = intersect; //take the answer closer to "a"
                }
            }
        }
    return answer;

}

void Terrain::addPlanet(PhysicsBody& planet, BlockType type)
{
    planets.push_back({Globals::Game.objects.getObject(&planet),type});
}

void Terrain::render(int i, int z)
{

   //Vector3 white = Vector3{255,255,255}*std::max(0.0f,std::min(1.0f,(1 - static_cast<float>(z - Globals::Game.camera.position.z - Globals::Game.CAMERA_Z_DISP)/Globals::MAX_Z)));

   Vector3 white = Vector3(255,255,255)*pow(.5,i);

   Color balls = {white.x,white.y,white.z,255}; //can't do math with raylib colors breaking_bad_crawl_space.gif

    float shade = pow(.5,i);
    SetShaderValue(TerrainOutline,GetShaderLocation(TerrainOutline,"shade"),&shade,SHADER_UNIFORM_FLOAT);

   BeginShaderMode(TerrainOutline);

   float ratio =  Block::BLOCK_DIMEN/PIXEL_SIZE;
   DrawBillboardPro(Globals::Game.Camera.getCamera(),blocksTexture.texture,Rectangle(0,0,blocksTexture.texture.width,blocksTexture.texture.height)
                    ,Vector3(blocksTexture.texture.width*ratio/2,blocksTexture.texture.height*ratio/2,z),Vector3(0,-1,0),
                    Vector2(blocksTexture.texture.width*ratio,blocksTexture.texture.height*ratio),Vector2(blocksTexture.texture.width/2,blocksTexture.texture.height/2)*ratio,
                    0,balls);
    EndShaderMode();
   //BeginShaderMode(GravityFieldShader);
    //DrawSprite3D(gravityTexture.texture,Rectangle(MAX_TERRAIN_SIZE/2,MAX_TERRAIN_SIZE/2,MAX_TERRAIN_SIZE,MAX_TERRAIN_SIZE));
    //EndShaderMode();

}

void Terrain::drawBlocks()
{
    isDrawing = true;
    BeginTextureMode(blocksTexture);
}

void Terrain::endDrawBlocks()
{
    isDrawing = false;
    EndTextureMode();
}
