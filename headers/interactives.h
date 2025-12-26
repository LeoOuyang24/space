#ifndef INTERACTIVES_H_INCLUDED
#define INTERACTIVES_H_INCLUDED

#include "objects.h"
#include "render.h"
#include "factory.h"

struct Sign : public Object<RectCollider,TextureRenderer,Sign>
{
    static constexpr Vector2 SIGN_DIMEN = {40,40};
    static constexpr std::string SIGN_SPRITE_PATH = "sign.png";
    std::vector<std::string> message;
    Sign();

    void interactWith(PhysicsBody& other);
    std::string getMessage(size_t index) const;
    size_t getMessagesSize();
};

struct Rover : public Object<RectCollider,TextureRenderer,Rover>
{
    static constexpr Vector2 ROVER_DIMEN = {110,110};
    static constexpr std::string ROVER_SPRITE_PATH = "rover.png";
    Rover();
    void interactWith(PhysicsBody& other);
};

template<>
struct Factory<Sign>
{
    static constexpr std::string ObjectName = "sign";
    using Base = FactoryBase<Sign,
                                access<Sign,&Sign::message>,
                                access<Sign,&Sign::orient,&Orient::pos>>;
};

template<>
struct Factory<Rover>
{
    static constexpr std::string ObjectName = "rover";
    using Base = FactoryBase<Rover,
                                access<Rover,&Rover::orient,&Orient::pos>>;
};

#endif // INTERACTIVES_H_INCLUDED
