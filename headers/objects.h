#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <raylib.h>

struct Object
{
    Vector2 pos;
    int radius;

    Vector2 force = {0,0};

    void render();
};

#endif // OBJECTS_H_INCLUDED
