#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <tuple>
#include <unordered_set>

#include "objects.h"
#include "colliders.h"
#include "render.h"
#include "item.h"


template<>
struct std::hash<Key::KeyVal>
{
    size_t operator()(const Key::KeyVal& color) const;
};

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

//variables that get reset upon RESET
struct PlayerState
{
    std::unordered_set<Key::KeyVal> keys;
    Orient orient;
};

class Item;
struct Player : public Object<PlayerCollider,PlayerRenderer>
{
    static Texture2D PlayerSprite;
    static constexpr int PLAYER_DIMEN = 20;

    static constexpr float PLAYER_MAX_SPEED = 2;
    static constexpr float PLAYER_RUN_MAX_SPEED = 3;
    static constexpr float PLAYER_MAX_AIR_SPEED = 1;

    static constexpr float PLAYER_GROUND_ACCEL = 0.1;
    static constexpr float PLAYER_AIR_ACCEL = 0.05;

    static constexpr float PLAYER_MAX_POWER = 50;

    enum State
    {
        WALKING = 0,
        CHARGING
    };

    State state = WALKING;
    PlayerState resetState;



    float speed = 0;

    float aimAngle = 0;
    float power = 0;

    bool facing = true;

    std::unordered_set<Key::KeyVal> keys;
    std::weak_ptr<Item> holding;

    Player(const Vector2& pos);
    void update(Terrain&);
    void setHolding(Item& obj);
    Item* getHolding();
    void addKey(Key::KeyVal);

    void handleControls(); //all player controls are handled here

};

#endif // PLAYER_H_INCLUDED
