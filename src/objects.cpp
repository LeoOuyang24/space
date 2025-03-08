#include "../headers/objects.h"

void Object::render()
{
    DrawPoly(pos,8,radius,0,WHITE);
}
