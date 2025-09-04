#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <tuple>

#include "objects.h"
#include "render.h"

//a bit more accurate rect collider that checks if a side is in terrain
struct PlayerCollider : public RectCollider
{
    PlayerCollider(int width, int height);
    bool isOnGround(Orient& orient, GlobalTerrain& terrain);
};

struct PlayerRenderer : public TextureRenderer
{
    void render(const Shape& shape, const Color& color);
};

struct Player : public Object<PlayerCollider,TextureRenderer>
{
    static Texture2D PlayerSprite;
    static constexpr int PLAYER_DIMEN = 20;


    float renderRotation = 0;
    Vector2 left, right;

    Player(const Vector2& pos);
    void update(GlobalTerrain&);
};

#endif // PLAYER_H_INCLUDED
