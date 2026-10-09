#include "../headers/physics.h"
#include "../headers/shape.h"
#include "../headers/blocks.h"

Vector2 Forces::getNormalForceMultiplier(const Shape& shape, const Vector2& totalForce, const Terrain& terrain)
{
    Shape horiz = shape;
    Shape vert = shape;

    horiz.orient.pos.x += totalForce.x;
    vert.orient.pos.y += totalForce.y;

    return {terrain.blockExists(horiz) ? -1 : 1, terrain.blockExists(vert) ? -1 : 1};

}