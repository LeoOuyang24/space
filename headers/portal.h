#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include "objects.h"
#include "shape.h"
#include "render.h"
#include "player.h"


struct Portal : public Object<CircleCollider,ShapeRenderer<CIRCLE>>
{
    Orient dest;
    Shader portalShader;
    Portal(int x, int y,int z, int radius, const Vector3& dest_);
    void update()
    {

    }
    void collideWith(PhysicsBody& other);
    void render();
};


#endif // PORTAL_H_INCLUDED
