#include "../headers/interactives.h"

Sign::Sign() : Object({},
                      std::make_tuple(SIGN_DIMEN.x,SIGN_DIMEN.y),
                      std::make_tuple(Globals::Game.Sprites.getSprite(SIGN_SPRITE_PATH)))
{

}

void Sign::interactWith(PhysicsBody& other)
{

    Globals::Game.interface.setMessage(this);
}

std::string Sign::getMessage(size_t index) const
{
    return index < message.size() ? message[index] : "";
}

size_t Sign::getMessagesSize()
{
    return message.size();
}

Rover::Rover() : Object({},std::make_tuple(ROVER_DIMEN.x,ROVER_DIMEN.y),std::make_tuple(Globals::Game.Sprites.getSprite(ROVER_SPRITE_PATH)))
{

}

void Rover::interactWith(PhysicsBody& other)
{

}
