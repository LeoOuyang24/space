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

    if (owner.holding.lock().get())
    {
        Vector2 mousePos = screenToWorld(GetMousePosition(),Globals::Game.getCamera(),Globals::Game.getCurrentZ());
        DrawLine3D(toVector3(shape2.orient.pos),
                   toVector3(shape2.orient.pos + Vector2Normalize(mousePos - shape2.orient.pos)*owner.power*10),RED);
    }

    if (owner.state != Player::State::PORTALLING && owner.state != Player::State::DEAD)
    {
        sprite = Globals::Game.Sprites.getSprite(Vector2LengthSqr(owner.forces.getForce(Forces::BOOSTING)) < 1 ? "guy.png" : "guy_boosting.png");
            TextureRenderer::render(shape2,color);
    }

    /*Rectangle rect = owner.getOrient().getRect(GetDimen(owner.getShape()));
    Vector2 center = owner.getPos();
    DrawSphere(toVector3(rotatePoint({rect.x,rect.y},center,owner.getOrient().rotation)),5,RED);
    DrawSphere(toVector3(rotatePoint({rect.x + rect.width,rect.y},center,owner.getOrient().rotation)),5,RED);
    DrawSphere(toVector3(rotatePoint({rect.x,rect.y + rect.height},center,owner.getOrient().rotation)),5,RED);
    DrawSphere(toVector3(rotatePoint({rect.x + rect.width,rect.y + rect.height},center,owner.getOrient().rotation)),5,RED);*/

}

bool Player::isTangible()
{
    return tangible && state != PORTALLING;
}

Player::Player(const Vector2& pos_) : Object({pos_},std::make_tuple(PLAYER_DIMEN,PLAYER_DIMEN),std::make_tuple(std::ref(*this)))
{
    renderer.setSprite(Globals::Game.Sprites.getSprite("guy.png"));
}

void Player::update(Terrain& terrain)
{
    //if (!onGround)
    if (state != DEAD)
    {
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

        if (PhysicsBody* ptr = holding.lock().get())
        {
            Orient o = ptr->getOrient();
            o.pos = (getPos() + orient.getNormal()*(-collider.height/2 - GetDimen(ptr->getShape()).y/2));
            o.rotation = orient.rotation;

            ptr->setOrient(o);
        }
    }

}

void Player::addKey(Key::KeyVal val)
{
    keys.insert(val);
}

void Player::handleControls()
{
    bool leftRight = (IsKeyDown(KEY_A) || IsKeyDown(KEY_D));

    switch (state)
    {
    case WALKING:
        {
            //power = 0;
            if (leftRight)
            {
                float accel = (onGround ? PLAYER_GROUND_ACCEL : PLAYER_AIR_ACCEL);
                float maxSpeed = !onGround ? PLAYER_MAX_AIR_SPEED :
                                            PLAYER_MAX_SPEED;
                if (onGround) //update facing, but only on ground
                {
                    //on ground, we can turn on a dime
                    facing = IsKeyDown(KEY_D);
                    speed = (abs(speed) + accel)*(2*facing - 1);
                }
                else if (!onGround && (IsKeyDown(KEY_D) != facing))
                {
                    //if in the air and moving backwards against how we are facing, we can decelerate using PLAYER_AIR_ACCEL
                    speed *= accel;
                }

                speed = Clamp(speed,-maxSpeed,maxSpeed);//prevent speed from exceeding maximum

            }
            if (IsKeyPressed(KEY_SPACE) && onGround)
            {
                Vector2 jump = IsKeyDown(KEY_LEFT_CONTROL) ?
                                    orient.getNormal()*-10 + orient.getFacing()*12*(facing*2 - 1) :
                                    orient.getNormal()*-14;
                forces.addForce(jump,Forces::JUMP);
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !onGround && !boosted)
            {


                forces.addFriction(0);
                forces.addForce(Vector2Normalize(screenToWorld(GetMousePosition(),
                                              Globals::Game.getCamera(),
                                              Globals::Game.getCurrentZ()) - getPos())*5,Forces::BOOSTING);
                /*Vector2 boostForce =Vector2Normalize(screenToWorld(GetMousePosition(),
                                              Globals::Game.getCamera(),
                                              Globals::Game.getCurrentZ()) - getPos())*10;*/
                //forces.setForce(boostForce,Forces::GRAVITY);
                /*Sequences::add(true,[boostForce,this](int frames){
                               setPos(getPos() + boostForce);

                               return frames >= 10;
                               });*/

                boosted = true;

                Sequences::add(false,[pos = orient.pos + orient.getNormal()*GetDimen(getShape()).y,rot=orient.rotation](int count){
                               DrawSprite3D(Globals::Game.Sprites.getSprite("mid-air-boost.png"),
                                              Rectangle(pos.x,pos.y,40,20),rot,Color(255,255,255,255-255*count/10.0));

                                    return count >= 10;
                               });

            }
            if (PhysicsBody* body = holding.lock().get())
            {
                Vector2 mousePos = screenToWorld(GetMousePosition(),Globals::Game.getCamera(),Globals::Game.getCurrentZ());
                body->getForces().addFriction(0);
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    power = std::min(power + 1,100.0f);

                }
                else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && power > 0)
                {
                    //Debug::togglePaused();
                    static_cast<Barrel*>(body)->held = GetTime();
                    float angle = atan2(mousePos.y - orient.pos.y,mousePos.x - orient.pos.x);
                    body->getForces().addForce(Vector2Normalize(mousePos - orient.pos)*(power)*0.5,Forces::MOVE);
                    holding.reset();
                    power = 0;
                }
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
    //setState((IsKeyDown(KEY_LEFT_SHIFT) && onGround) ? CHARGING : WALKING);
    if (((!leftRight || !onGround) || state == CHARGING))
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
        dying = std::max(dying - std::max(2.0,dying*0.1),0.0);
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
    tangible = false;
    setDead(false);
    setState(DEAD);

    Sequences::add(false,[start=GetTime(),rect=Rectangle(getPos().x, getPos().y, 500, 500)](int frames){
                   const Anime* a = Globals::Game.Sprites.getAnime("death.png");
                   if (a)
                   {

                        DrawAnime3D(a->spritesheet,start,a->info,rect,Globals::Game.getCurrentZ());
                        return frames > a->info.horizFrames*a->info.vertFrames/a->info.speed;
                   }
                    return frames >= 60;
                   },[this](int frames){

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

                    holding.reset(); //drop what we're holding
                    setState(WALKING);
                    tangible = true;
                   return true;
                   });




}

