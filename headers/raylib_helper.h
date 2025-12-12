#ifndef RAYLIB_HELPER_H_INCLUDED
#define RAYLIB_HELPER_H_INCLUDED

//raylib functions that should exist but don't

#include "raylib.h"

enum TextAlign
{
    LEFT, //given (x,y), text is drawn starting at x, with y being the middle of the height
    CENTER, //given (x,y), text is drawn assuming x is the halfway point
    RIGHT //given (x,y), text is drawn assuming x is the end
};

void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint);
//draw text in 3d, and return the left-most point at the very end. This return value can then be used to draw like a texture or something after the text
Vector3 DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint, TextAlign align = LEFT);

#endif // RAYLIB_HELPER_H_INCLUDED
