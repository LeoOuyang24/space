#include <iostream>
#include <algorithm>

#include "../headers/player.h"

#include "../headers/debug.h"
#include "../headers/game.h"

#include "../headers/item.h"
#include "../headers/sequencer.h"
#include "../headers/raylib_helper.h"

size_t std::hash<Key::KeyVal>::operator()(const Key::KeyVal& val) const
{
    //basically colors are hashed based on their hex value
    return val.r  << 24 + val.g << 16 + val.b << 8 + val.a;
}

Texture2D Player::PlayerSprite;

PlayerCollider::PlayerCollider(int width, int height) : RectCollider{width,height}
{

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
    facing = owner.facing;

    switch (owner.state)
    {
    case Player::State::CHARGING:
        {
            shape2.orient.rotation += owner.aimAngle;
            DrawLine3D({shape2.orient.pos.x,shape2.orient.pos.y,Globals::Game.getCurrentZ()},
                       {shape2.orient.pos.x + cos(shape2.orient.rotation - M_PI/2)*100,shape2.orient.pos.y + sin(shape2.orient.rotation - M_PI/2)*100,Globals::Game.getCurrentZ()},
                       RED
                       );
            break;
        }
    case Player::State::PORTALLING:
        {
            DrawSphere(toVector3(shape2.orient.pos),GetDimen(shape2).x/2.0,YELLOW);
            break;
        }
    }

    if ( owner.freeFallTime != 0 && GetTime() - owner.freeFallTime > 5)
    {
        suggestButtonPress(owner.getShape(),"Z");
    }

    if (owner.state != Player::State::PORTALLING)
    {
        sprite = Globals::Game.Sprites.getSprite(Vector2LengthSqr(owner.forces.getForce(Forces::BOOSTING)) < 1 ? "guy.png" : "guy_boosting.png");
            TextureRenderer::render(shape2,color);
    }

}

bool Player::isTangible()
{
    return state != PORTALLING;
}

Player::Player(const Vector2& pos_) : Object({pos_},std::make_tuple(PLAYER_DIMEN,PLAYER_DIMEN),std::make_tuple(std::ref(*this)))
{
    renderer.setSprite(Globals::Game.Sprites.getSprite("guy.png"));
}

void Player::update(Terrain& terrain)
{
    //if (!onGround)
    Object::applyForces(terrain);

    //tint = onGround ? WHITE : freeFall ? BLUE : RED;


    float oldRotation = orient.rotation;
    Object::adjustAngle(terrain);
   /* if (onGround && !wasOnGround && abs(orient.rotation - oldRotation) > 3*M_PI/4)
    {
        //flip angle upon landing
        facing = !facing;
    }*/

    if (onGround && !wasOnGround)
    {
        freeFallTime = -1;
    }

    handleControls();

    //Vector2 forwardNorm = orient.getFacing(); //perpendicular to normal vector that moves us forward on flat ground with rotation 0
    //orient.pos += forwardNorm*speed;

    if (onGround )
    {
        boosted = false;
        freeFallTime = 0;
        saveResetState();
        stayOnGround(terrain);
    }
    else if (wasOnGround)
    {
        freeFallTime = GetTime();
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

            }
            if (IsKeyPressed(KEY_SPACE) && onGround)
            {
                Vector2 jump = running ?
                                    orient.getNormal()*-10 + orient.getFacing()*12*(facing*2 - 1) :
                                    orient.getNormal()*-14;
                forces.addForce(jump,Forces::JUMP);
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !onGround && !boosted)
            {
                forces.addFriction(0);
                forces.addForce(Vector2Normalize(screenToWorld(GetMousePosition(),
                                              Globals::Game.getCamera(),
                                              {GetScreenWidth(),GetScreenHeight()},
                                              Globals::Game.getCurrentZ()) - getPos())*3,Forces::BOOSTING);
                boosted = true;
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
    setState(state == PORTALLING ? PORTALLING : WALKING);
    //setState((IsKeyDown(KEY_LEFT_SHIFT) && onGround) ? CHARGING : WALKING);
    if ((!leftRight || !onGround)|| state == CHARGING)
    {
        speed = trunc(speed*(onGround ? GROUND_FRICTION : AIR_FRICTION),3); //apply friction
    }
    //orient.pos += orient.getFacing()*speed;

    forces.setForce(orient.getFacing()*speed,Forces::MOVE);

    if (IsKeyDown(KEY_Z))
    {
        dying += 1;
        if (dying >= 100)
        {
            setDead(true);
            dying = 0;
        }
    }
    else
    {
        dying = 0;
    }
   // std::cout << forces.getForce(Forces::MOVE) << "\n";
}

void Player::setState(State newState)
{
    state = newState;
}

void Player::saveResetState()
{
    resetState.restoreThese.clear();
    resetState.orient = getOrient();
}

void Player::addResetObject(PhysicsBody& body)
{
    resetState.restoreThese.push_back({Globals::Game.objects.getObject(&body),body.getOrient()});
}

void Player::resetPlayer()
{
    forces.addFriction(0);
    std::for_each(resetState.restoreThese.begin(),resetState.restoreThese.end(),[](const RestoreObject& restore){

                  if (restore.ptr.get())
                  {
                        restore.ptr->setDead(false);
                        restore.ptr->setOrient(restore.orient);
                        restore.ptr->onRestore();
                        Globals::Game.addObject(restore.ptr);
                  }

                  });
    resetState.restoreThese.clear();
    orient = resetState.orient;
    setDead(false);

}
