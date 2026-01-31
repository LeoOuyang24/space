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

//
struct PlayerCollider : public RectCollider
{
    PlayerCollider(int width, int height);
};
struct PlayerRenderer : public TextureRenderer
{
    PlayerRenderer(Player& owner_);
    void render(const Shape& shape, const Color& color);
private:
    Player& owner;
};

//represents an object that may need to be restored if the player dies before landing
//used for things like restoring a gear that was collected followed by the player dying before landing
struct RestoreObject
{
    std::shared_ptr<PhysicsBody> ptr;
    Orient orient;
};

//variables that get reset upon RESET
struct PlayerState
{
    std::vector<RestoreObject> restoreThese;
    std::unordered_set<Key::KeyVal> keys;
    Orient orient; //orientation right before dying
};

struct Player : public Object<PlayerCollider,PlayerRenderer,Player>
{
    static Texture2D PlayerSprite;
    static constexpr int PLAYER_DIMEN = 30;

    static constexpr float PLAYER_MAX_SPEED = 5;
    static constexpr float PLAYER_RUN_MAX_SPEED = 5; //currently unused
    static constexpr float PLAYER_MAX_AIR_SPEED = 4.5;

    static constexpr float PLAYER_GROUND_ACCEL = 0.2; //added to player force every frame
    static constexpr float PLAYER_AIR_ACCEL = 0.9; //unlike the above acceleration, this is MULTIPLED every frame

    static constexpr float AIR_FRICTION = 0.99; //number to multiply to speed every frame. Bigger number = less friction
    static constexpr float GROUND_FRICTION = 0.85;

    static constexpr float PLAYER_MAX_POWER = 100;

    enum State
    {
        WALKING = 0,
        CHARGING,
        PORTALLING,
        DEAD
    };

    State state = WALKING;
    PlayerState resetState;

    float speed = 0;

    float aimAngle = 0; //aim for charging
    float power = 0; //charging power
    int dying = 0; //press and hold to die

    double freeFallTime = 0;

    bool boosted = false;

    bool facing = true;
    bool tangible = true; //player-specific cuz currently there's not really a reason other objects should be intangible

    std::weak_ptr<PhysicsBody> holding;

    std::unordered_set<Key::KeyVal> keys;
    bool isTangible();

    Player(const Vector2& pos);
    void update(Terrain&);
    void addKey(Key::KeyVal);

    void handleControls(); //all player controls are handled here

    void setState(State newState);

    void saveResetState();
    void addResetObject(PhysicsBody& body);
    void resetPlayer();

};



#endif // PLAYER_H_INCLUDED
