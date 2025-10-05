#include <iostream>

#include "../headers/player.h"

#include "../headers/debug.h"
#include "../headers/game.h"

#include "../headers/item.h"

Texture2D Player::PlayerSprite;


PlayerCollider::PlayerCollider(int width, int height, Player& owner_) : RectCollider{width,height}, owner(owner_)
{

}

bool PlayerCollider::isOnGround(Orient& o, Terrain& t)
{
    Vector2 last = o.pos;
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left

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

float PlayerCollider::getLandingAngle(Orient& o, Terrain& terrain)
{
    Vector2 prevCorner = o.pos + Vector2Rotate(Vector2(-width/2,-height/2),o.rotation); //top left


    Vector2 ground = {}; //vector towrads the ground, calculated by adding all intersections
    for (int i = 0; i < 4; i ++) //top right, bot right, bot left, top left
    {
        int index =(i + 1)%4;
        Vector2 corner = o.pos + Vector2Rotate(Vector2(width/2,height/2)*Vector2(index/2*2 - 1,((index%3) != 0)*2 - 1),o.rotation);

        PossiblePoint intersect = terrain.lineIntersectWithTerrain(prevCorner,corner);

        if (intersect.exists)
        {
            ground += o.pos - intersect.pos;
        }

        prevCorner = corner;
    }

    if (Vector2Equals(ground,{0,0}))
        {
            return o.rotation;
        }

    return atan2(ground.y,ground.x) + M_PI/2;
    }

PlayerRenderer::PlayerRenderer(Player& owner_) : owner(owner_)
{

}

void PlayerRenderer::render(const Shape& shape,const Color& color)
{
    /*if (sprite)
    {
        Vector2 dimen = GetDimen(shape);
        DrawBillboardPro(Globals::Game.camera,*sprite,Rectangle(0,0,sprite->width*flip,sprite->height),
                         Vector3(shape.orient.pos.x,shape.orient.pos.y,Globals::Game.terrain.getZOfLayer(shape.orient.layer)),Vector3(0,-1,0),dimen,
                         dimen*0.5,shape.orient.rotation*RAD2DEG*-1,color);

    }*/
    TextureRenderer::render(shape,color);
}

Player::Player(const Vector2& pos_) : Object({pos_},std::make_tuple(PLAYER_DIMEN,PLAYER_DIMEN,std::ref(*this)),std::make_tuple(std::ref(*this)))
{
    renderer.setSprite(PlayerSprite);
}

void Player::update(Terrain& terrain)
{
    //if (!onGround)
    bool wasOnGround = onGround;
    Object::update(terrain);

    tint = onGround ? WHITE : RED;
    Vector2 normal = orient.getNormal();


    //if on ground, adjust our angle based on the angle of the terrain
    if (onGround)
    {
        if (!wasOnGround) //just landed
        {
            orient.rotation = collider.getLandingAngle(orient,terrain);
        }
        else //otherwise adjust angle based on terrain angle
        {
            Vector2 botLeft = orient.pos +Vector2Rotate(Vector2(-collider.width/2,collider.height/2),orient.rotation);
            Vector2 botRight = orient.pos + Vector2Rotate(Vector2(collider.width/2,collider.height/2),orient.rotation);

            botLeft = terrain.lineTerrainIntersect(botLeft - normal, botLeft).pos;
            botRight = terrain.lineTerrainIntersect(botRight - normal,botRight).pos;

            float newAngle = trunc(atan2(botRight.y - botLeft.y, botRight.x - botLeft.x),3);

            if (trunc(abs(newAngle - orient.rotation),2) > .001)
            {
                orient.rotation = newAngle;
            }
        }
    }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        {
            this->facing = IsKeyDown(KEY_D);

            float accel = (onGround ? PLAYER_GROUND_ACCEL : PLAYER_AIR_ACCEL);
            float accelAmount = speed == 0 ? accel*2 : std::max(accel*0.5f,abs(accel*speed));
            float maxSpeed = IsKeyDown(KEY_LEFT_SHIFT) ? PLAYER_RUN_MAX_SPEED : PLAYER_MAX_SPEED;
            speed += accelAmount*(facing*2 - 1);


            speed = std::min(abs(speed),maxSpeed)*((speed > 0)*2 - 1); //prevent speed from exceeding maximum

            this->renderer.facing = facing;
        }
        else
        {
            speed = trunc(speed*0.85,3);
        }

        Vector2 forwardNorm = orient.getFacing(); //perpendicular to normal vector that moves us forward on flat ground with rotation 0
        orient.pos += forwardNorm*speed;

    if (onGround )
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            forces.addForce(orient.getNormal()*-8,Forces::JUMP);
        }

        Vector2 bruh = terrain.lineTerrainIntersect(orient.pos,orient.pos + orient.getNormal()*collider.height/2).pos; //- normal*(collider.height)/2;
        Vector2 newPos = bruh - orient.getNormal()*(collider.height/2  - 1);

        orient.pos = newPos;
    }

    if (Item* held = getHolding())
    {
        held->orient.pos = orient.pos + orient.getFacing()*(GetDimen(held->getShape()).x/2 + collider.width/2)*(facing*2 - 1);
        held->orient.layer = orient.layer;
        held->orient.rotation = orient.rotation;
    }
}

void Player::setHolding(Item& body)
{
    auto shared = Globals::Game.objects.getObject(body);
    if (shared.get())
    {
        holding = std::static_pointer_cast<Item>(shared);
    }
}

Item* Player::getHolding()
{
    return (holding.lock().get());
}
