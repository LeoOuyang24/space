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

template<>
struct Factory<Sign>
{
    static constexpr std::string ObjectName = "sign";
    using Base = FactoryBase<Sign,
                                access<Sign,&Sign::message>,
                                access<Sign,&Sign::orient,&Orient::pos>,
                                access<Sign,&Sign::orient,&Orient::layer>>;
};

#endif // INTERACTIVES_H_INCLUDED
