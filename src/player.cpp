#include <iostream>

#include "../headers/player.h"

#include "../headers/debug.h"

Texture2D Player::PlayerSprite;


PlayerCollider::PlayerCollider(int width, int height) : RectCollider{width,height}
{

}

bool PlayerCollider::isOnGround(Orient&, GlobalTerrain&)
{
    float angle = 0; //atan2 can not return 4, so this represents if there was no angle change
    Vector2 last = o.pos;
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left

    Vector2 ground = {}; //vector towrads the ground, calculated by adding all intersections
    for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left
    {
        int index =(i + 1)%4;
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(index/2*2 - 1,((index%3) != 0)*2 - 1),o.rotation);

        PossiblePoint intersect = t.lineIntersectWithTerrain(prevCorner,corner);

      /*  Debug::addDeferRender([prevCorner,corner,intersect](){
                                      DrawLineEx(prevCorner, corner,2, intersect.exists ? PURPLE : WHITE);

                              });*/

        if (intersect.exists)
        {
            return true;
        }

        prevCorner = corner;
    }
    return false;

}

void PlayerRenderer::render(const Shape& shape,const Color& color)
{
    TextureRenderer::render(shape, color);
}

Player::Player(const Vector2& pos_) : Object({pos_},WHITE,PLAYER_DIMEN,PLAYER_DIMEN)
{
    renderer.setSprite(PlayerSprite);
}


void Player::update(GlobalTerrain& terrain)
{
        Object::update(terrain);

       // bool shouldUpdate = (pear.first && !onGround) || (IsKeyDown(KEY_A) || IsKeyDown(KEY_D));

        tint = onGround ? WHITE : RED;
        Vector2 normal = orient.getNormal();

    //if on ground, adjust our angle based on the angle of the terrain
    if (onGround)
    {
        Vector2 botLeft = orient.pos +Vector2Rotate(Vector2(-collider.width/2,collider.height/2),orient.rotation);
        Vector2 botRight = orient.pos + Vector2Rotate(Vector2(collider.width/2,collider.height/2),orient.rotation);

        botLeft = terrain.lineTerrainIntersect(botLeft - normal, botLeft).pos;
        botRight = terrain.lineTerrainIntersect(botRight - normal,botRight).pos;

        float newAngle = atan2(botRight.y - botLeft.y, botRight.x - botLeft.x);

        if (trunc(abs(newAngle - orient.rotation),2) > .001)
        {
            Debug::addDeferRender([angle=newAngle,old=orient.rotation,pos=orient.pos](){
          DrawLine(pos.x,pos.y,pos.x + cos(angle)*100, pos.y+sin(angle)*100,WHITE);
          DrawText(std::to_string(angle - old).c_str(),pos.x - 10,pos.y - 10,5,WHITE);
          //DrawText(std::to_string(old).c_str(),pos.x - 10, pos.y - 20, 5, WHITE);
          });

            orient.rotation = atan2(botRight.y - botLeft.y, botRight.x - botLeft.x);

        }
    }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        {
            Vector2 facing = orient.getFacing(); //if on flat ground, this vector points to the right

            Vector2 forwardNorm = facing; //perpendicular to normal vector that moves us forward on flat ground with rotation 0
            Vector2 backwardNorm = facing*-1;

            Vector2 move = {0,0};
            if (IsKeyDown(KEY_D))
            {
                move = forwardNorm;
            }
            if (IsKeyDown(KEY_A))
            {
                move = backwardNorm;
            }

            orient.pos += move;
        }
        if (onGround )
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                forces.addForce(orient.getNormal()*-6,Forces::JUMP);
            }

            Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + orient.getNormal()*collider.height/2).pos; //- normal*(collider.height)/2;
            Vector2 newPos = bruh - orient.getNormal()*(collider.height/2  - 1);

            orient.pos = newPos;

        }
        //Vector2 newA =

        //pos += move;

    }
