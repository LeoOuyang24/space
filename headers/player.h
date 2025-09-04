#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <tuple>

#include "objects.h"
#include "render.h"

//literally the same thing as a rect collider but we store which points collided with terrain
struct PlayerCollider : public RectCollider
{
    bool inTerrain = false;

    PlayerCollider(int width, int height);
    bool isOnGround(Orient& orient, GlobalTerrain& terrain); //just returns inTerrain

     //returns true and an angle if we are in terrain, or false and 0 if not.
     // use the bool to set inTerrain so we don't have to call this calculation every time
     //also makes it so that we don't have to redo this calculation in Object::update to check if we are in the ground and again to update our angle
    std::pair<bool,float>checkIfInTerrain(Orient& orient, GlobalTerrain& terrain);
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
