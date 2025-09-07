#include <iostream>

#include "../headers/player.h"

#include "../headers/debug.h"

Texture2D Player::PlayerSprite;


PlayerCollider::PlayerCollider(int width, int height) : RectCollider{width,height}
{

}

bool PlayerCollider::isOnGround(Orient& o, GlobalTerrain& t)
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

PlayerRenderer::PlayerRenderer(Player& owner_) : owner(owner_)
{

}

void PlayerRenderer::render(const Shape& shape,const Color& color)
{
    if (sprite)
    {
        RectCollider* rect = static_cast<RectCollider*>(shape.collider);
        int flip = (owner.facing)*2 - 1;
        DrawTexturePro(*sprite,{0,0,sprite->width*flip,sprite->height},
                       {shape.orient.pos.x,shape.orient.pos.y,rect->width,rect->height},
                       {rect->width/2,rect->height/2},
                       shape.orient.rotation*RAD2DEG,
                       color
                       );
    }
}

Player::Player(const Vector2& pos_) : Object(pos_,std::make_tuple(PLAYER_DIMEN,PLAYER_DIMEN),std::make_tuple(std::ref(*this)))
{
    renderer.setSprite(PlayerSprite);
}


void Player::update(GlobalTerrain& terrain)
{
        Object::update(terrain);

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
            orient.rotation = atan2(botRight.y - botLeft.y, botRight.x - botLeft.x);

        }
        Debug::addDeferRender([angle=orient.rotation,pos=orient.pos](){
          DrawLine(pos.x,pos.y,pos.x + cos(angle)*100, pos.y+sin(angle)*100,WHITE);
          DrawText((std::string("ANGLE: ") + std::to_string(angle)).c_str(),pos.x,pos.y - 10,5,WHITE);
          //DrawText(std::to_string(old).c_str(),pos.x - 10, pos.y - 20, 5, WHITE);
          });
    }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        {
            this->facing = IsKeyDown(KEY_D);

            float accel = (onGround ? PLAYER_GROUND_ACCEL : PLAYER_AIR_ACCEL);
            float accelAmount = speed == 0 ? accel*2 : std::max(accel*0.5f,abs(accel*speed));
            float maxSpeed = IsKeyDown(KEY_LEFT_SHIFT) ? PLAYER_RUN_MAX_SPEED : PLAYER_MAX_SPEED;
            speed += accelAmount*(facing*2 - 1);


            speed = std::min(abs(speed),maxSpeed)*((speed > 0)*2 - 1); //prevent speed from exceeding maximum
        }
        else
        {
            speed = trunc(speed*0.9,3);
        }

        Debug::addDeferRender([speed=this->speed,pos=orient.pos](){
                              DrawText(std::to_string(speed).c_str(),pos.x,pos.y - 20,10,WHITE);
                              });

        Vector2 forwardNorm = orient.getFacing(); //perpendicular to normal vector that moves us forward on flat ground with rotation 0
        orient.pos += forwardNorm*speed;

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
