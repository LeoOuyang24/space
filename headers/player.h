#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <tuple>

#include "objects.h"
#include "render.h"

class Player;
//a bit more accurate rect collider that checks if a side is in terrain
//also considered "not on ground" if user is jumped
struct PlayerCollider : public RectCollider
{
    PlayerCollider(int width, int height, Player& owner_);
    bool isOnGround(Orient& orient, Terrain& terrain);
private:
    Player& owner;
};

struct PlayerRenderer : public TextureRenderer
{
    PlayerRenderer(Player& owner_);
    void render(const Shape& shape, const Color& color);
private:
    Player& owner;
};

struct Player : public Object<PlayerCollider,PlayerRenderer>
{
    static Texture2D PlayerSprite;
    static constexpr int PLAYER_DIMEN = 20;

    static constexpr float PLAYER_MAX_SPEED = 2;
    static constexpr float PLAYER_RUN_MAX_SPEED = 3;

    static constexpr float PLAYER_GROUND_ACCEL = 0.1;
    static constexpr float PLAYER_AIR_ACCEL = 0.01;
    float speed = 0;

    bool facing = true;


    Player(const Vector2& pos);
    void update(Terrain&);
};

#endif // PLAYER_H_INCLUDED
