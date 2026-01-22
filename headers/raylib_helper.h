#ifndef RAYLIB_HELPER_H_INCLUDED
#define RAYLIB_HELPER_H_INCLUDED

//raylib functions that should exist but don't
#include <iostream>

#include "raylib.h"

struct AnimeInfo
{
    float speed = 0; //frame per physics tick. For example, a value of 1 would move forward one frame per tick, basically 60 FPS
    int horizFrames = 1;
    int vertFrames = 1;
};

//x,y of "pos" is the center of the object, not the top left for all of these Draw functions
void DrawSprite3D(const Texture2D& sprite, const Rectangle& pos, float rotation = 0, Color tint = WHITE);
void DrawAnime(const Texture2D& sprite, double start, const AnimeInfo& info, const Rectangle& pos, float rotation = 0, Color tint = WHITE );
void DrawAnime3D(const Texture2D& sprite, double start, const AnimeInfo& info, const Rectangle& pos, float z, float rotation = 0, Color tint = WHITE );

//no idea where to put this LUL
//moves camera to look at "pos"
void moveCamera(Camera3D& camera, const Vector2& pos);

void moveCamera(Camera3D& camera, const Vector3& pos);

enum TextAlign
{
    LEFT = 0, //given (x,y), text is drawn starting at x, with y being the middle of the height
    CENTER, //given (x,y), text is drawn assuming x is the halfway point
    RIGHT //given (x,y), text is drawn assuming x is the end
};

//return a copy of the string formatted to next exceed "maxWidth"
//for simplicity, special white space characters (\n,\t,etc) are treated as regular characters
//don't include them for this to work properly
std::string fitText(Font font, std::string_view str, float fontSize, float fontSpacing, float maxWidth);

//draw text in 2d with alignment.
//Optionally provide a rectangle to fit the text in, which will shrink the text if the provided fontSize is too big
//(ignored if width and height are 0)
void DrawText2D(Font font, const char *text, Vector2 position, float fontSize,
                float fontSpacing, Color tint,
                TextAlign align = LEFT);

void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint);
//draw text in 3d, and return the left-most point at the very end. This return value can then be used to draw like a texture or something after the text
Vector3 DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint, TextAlign align = LEFT);

#endif // RAYLIB_HELPER_H_INCLUDED
