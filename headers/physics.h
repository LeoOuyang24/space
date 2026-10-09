#ifndef PHYSICS_H
#define PHYSICS_H

#include <cstdint>
#include <array>

#include "raylib.h"

struct Shape;
struct Terrain;

struct Forces
{
    enum ForceSource : uint8_t
    {
        GRAVITY = 0,
        JUMP,
        MOVE,
        ENEMY, //misc for any forces applied by enemies
        THROW,
        BOUNCE, //forces from when going out of bounds
        SWINGING,
        BOOSTING, //from boosting, player specific
        FORCE_SOURCE_SIZE //number of force sources, should always be the last member
    };

    static constexpr float MAX_FORCE_MAG = 20;
    static constexpr float ON_GROUND_FORCE = 1; //objects with a force less than this will stick to the ground
    std::array<Vector2,FORCE_SOURCE_SIZE> forces{}; //mapping force source to a force
    Vector2 totalForce = {0,0}; //total forces

    void setForce(const Vector2& force, Forces::ForceSource source);
    void addForce( Vector2 force, ForceSource source);
    void addFriction(const Vector2& friction);
    void addFriction(float friction);
    void addFriction(float friction, ForceSource source);
    Vector2 getTotalForce();

    Vector2 getForce(ForceSource source) const //read-only, get force from a source
    {
        return (source >= forces.size()) ? Vector2{0,0} : forces[source];
    }

    static Vector2 getNormalForceMultiplier(const Shape& shape, const Vector2& totalForce, const Terrain& terrain);
};

#endif // PHYSICS_H