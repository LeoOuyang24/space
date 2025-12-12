#include <iostream>

#include "../headers/player.h"

#include "../headers/debug.h"
#include "../headers/game.h"

#include "../headers/item.h"

size_t std::hash<Key::KeyVal>::operator()(const Key::KeyVal& val) const
{
    //basically colors are hashed based on their hex value
    return val.r  << 24 + val.g << 16 + val.b << 8 + val.a;
}

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
    Shape shape2 = shape;

    if (owner.state == Player::State::CHARGING)
    {
        shape2.orient.rotation += owner.aimAngle;
        DrawLine3D({shape2.orient.pos.x,shape2.orient.pos.y,Globals::Game.getCurrentZ()},
                   {shape2.orient.pos.x + cos(shape2.orient.rotation - M_PI/2)*100,shape2.orient.pos.y + sin(shape2.orient.rotation - M_PI/2)*100,Globals::Game.getCurrentZ()},
                   RED
                   );
    }
    TextureRenderer::render(shape2,color);

}

Player::Player(const Vector2& pos_) : Object({pos_},std::make_tuple(PLAYER_DIMEN,PLAYER_DIMEN,std::ref(*this)),std::make_tuple(std::ref(*this)))
{
    renderer.setSprite(*Globals::Game.Sprites.getSprite("guy.png"));
}

void Player::update(Terrain& terrain)
{
    //if (!onGround)
    Object::applyForces(terrain);

    tint = onGround ? WHITE : freeFall ? BLUE : RED;
    Vector2 normal = orient.getNormal();

    Object::adjustAngle(terrain);

    handleControls();

    //Vector2 forwardNorm = orient.getFacing(); //perpendicular to normal vector that moves us forward on flat ground with rotation 0
    //orient.pos += forwardNorm*speed;

    if (onGround )
    {
        stayOnGround(terrain);
    }

    if (terrain.isBlockType(orient.pos,LAVA))
    {
        setDead(true);
    }

}

void Player::addKey(Key::KeyVal val)
{
    keys.insert(val);
}

void Player::handleControls()
{
    bool leftRight = (IsKeyDown(KEY_A) || IsKeyDown(KEY_D));
    if (leftRight)
    {
        facing = IsKeyDown(KEY_D);
    }

    switch (state)
    {
    case WALKING:
        {
            power = 0;
            bool running = IsKeyDown(KEY_LEFT_CONTROL);
            if (leftRight)
            {
                float accel = (onGround ? PLAYER_GROUND_ACCEL : PLAYER_AIR_ACCEL);
                float maxSpeed = !onGround ? PLAYER_MAX_AIR_SPEED :
                                         running ?
                                            PLAYER_RUN_MAX_SPEED :
                                            PLAYER_MAX_SPEED;
                speed += accel*(facing*2 - 1);

                speed = std::min(abs(speed),maxSpeed)*((speed > 0)*2 - 1); //prevent speed from exceeding maximum

                this->renderer.facing = facing;
            }
            if (IsKeyPressed(KEY_SPACE) && onGround)
            {
                Vector2 jump = running ?
                                    orient.getNormal()*-8 + orient.getFacing()*8*(facing*2 - 1) :
                                    orient.getNormal()*-12;
                forces.addForce(jump,Forces::JUMP);
            }
        }
        break;
    case CHARGING:
        {
            if (IsKeyDown(KEY_SPACE))
            {
                power = Clamp(power + 1,0,PLAYER_MAX_POWER);
            }
            else if (IsKeyReleased(KEY_SPACE))
            {
                orient.rotation += aimAngle;
                resetState.orient = orient;
                resetState.keys = keys;
                forces.addForce(orient.getNormal()*-12*(1 + power/PLAYER_MAX_POWER),Forces::JUMP);
                power = 0;
                aimAngle = 0;
            }
            else
            {
                power = 0;
            }
            if (leftRight)
            {
                aimAngle = Clamp(aimAngle + 0.01*(this->facing*2 - 1),- M_PI/4, M_PI/4);
            }

        }
    break;
    }
    state = (IsKeyDown(KEY_LEFT_SHIFT) && onGround) ? CHARGING : WALKING;
    if ((!leftRight || !onGround)|| state == CHARGING)
    {
        speed = trunc(speed*(onGround ? GROUND_FRICTION : AIR_FRICTION),3); //apply friction
    }
    //orient.pos += orient.getFacing()*speed;

    forces.setForce(orient.getFacing()*speed,Forces::MOVE);
   // std::cout << forces.getForce(Forces::MOVE) << "\n";


}
