#include "../headers/raylib_helper.h"
#include "../headers/conversions.h"
#include "rlgl.h"

#include <iostream>

void moveCamera(Camera3D& camera, const Vector2& pos)
{

    float disp = Globals::CAMERA_Z_DISP*tan(camera.fovy/2*DEG2RAD); //distance from the edge of the screen

    //clamps camera to level area
    Vector2 clampedPos = {
        Clamp(pos.x,disp,Terrain::MAX_TERRAIN_SIZE - disp),
        Clamp(pos.y,disp,Terrain::MAX_TERRAIN_SIZE - disp)
    };
    assignVector(camera.position,clampedPos);
    assignVector(camera.target,clampedPos);

}

void moveCamera(Camera3D& camera, const Vector3& pos)
{
    moveCamera(camera,Vector2{pos.x,pos.y});

    camera.target.z = pos.z;
    camera.position.z = pos.z - Globals::CAMERA_Z_DISP;
}

std::string fitText(Font font, std::string_view text, float fontSize, float fontSpacing, float maxWidth)
{
    SplitString split(text,' '); //split strings into words
    std::string str = std::string(split[0]); //start with the very first word already added.
                                //yes this does mean that words that take up "maxWidth" will exceed "maxWidth" length, not much we can do

    size_t strStart = 0; //start of our current line
    for (int i = 1; i < split.size(); i ++)
    {
        size_t start = str.size(); //this is the index of the space before new word
        str += ' ';
        str += split[i]; //add the word and a space before it

        //calculate if our currnet line is too long
        float width = MeasureTextEx(font,&str[strStart],fontSize,fontSpacing).x;
        if (width >= maxWidth) //if the width of our new word exceeds "maxWidth"...
        {
            str[start] = '\n'; //...replace the space we added earlier with a newline instead
            strStart = start + 1; //index of our next line
        }
    }
    return str;
}

void DrawText2D(Font font, const char *text, Vector2 position, float fontSize, float fontSpacing, Color tint, TextAlign align)
{

    SplitString split(text,'\n');

    float height =  MeasureTextEx(font,text,fontSize,fontSpacing).y;

    position.y -= height/2;

    for (int i = 0; i < split.size(); i ++)
    {
        const char* c_str = std::string(split[i]).c_str();//no easy way to convert std::string_view to c_string that doesn't go over is my joker ark

        //dimension of this line (we really only care about horizontal)
        float width = MeasureTextEx(font,c_str,fontSize,fontSpacing).x;
        Vector2 adjusted = {position.x - width/2*align,position.y + height/split.size()*i};
        DrawTextEx(font,c_str,adjusted,fontSize,fontSpacing,tint);
    }

}

//shamelessly, almost 100% copy pasted from https://www.raylib.com/examples/text/loader.html?name=text_3d_drawing\
//is modified to draw along x-y plane as opposed to x-z,

// Draw codepoint at specified position in 3D space
void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;

    // Character destination rectangle on screen
    // NOTE: We consider charsPadding on drawing
    position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding)*scale;
    position.y += (float)(font.glyphs[index].offsetY - font.glyphPadding)*scale;

    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding, font.recs[index].height + 2.0f*font.glyphPadding };

    float width = (float)(font.recs[index].width + 2.0f*font.glyphPadding)*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.glyphPadding)*scale;

    if (font.texture.id > 0)
    {
        const float x = 0.0f;
        const float y = 0.0f;
        const float z = 0.0f;

        // normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
        const float tx = srcRec.x/font.texture.width;
        const float ty = srcRec.y/font.texture.height;
        const float tw = (srcRec.x+srcRec.width)/font.texture.width;
        const float th = (srcRec.y+srcRec.height)/font.texture.height;

        rlCheckRenderBatchLimit(4 + 4*backface);
        rlSetTexture(font.texture.id);

        rlPushMatrix();
            rlTranslatef(position.x, position.y, position.z);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);

                // Front Face
                rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
                rlTexCoord2f(tx, ty); rlVertex3f(x,         y,          z);              // Top Left Of The Texture and Quad
                rlTexCoord2f(tx, th); rlVertex3f(x,         y + height, z);     // Bottom Left Of The Texture and Quad
                rlTexCoord2f(tw, th); rlVertex3f(x + width, y + height, z);     // Bottom Right Of The Texture and Quad
                rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

                if (backface)
                {
                    // Back Face
                    rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
                    rlTexCoord2f(tx, ty); rlVertex3f(x,         y,          z);          // Top Right Of The Texture and Quad
                    rlTexCoord2f(tw, ty); rlVertex3f(x + width, y,          z);          // Top Left Of The Texture and Quad
                    rlTexCoord2f(tw, th); rlVertex3f(x + width, y + height, z); // Bottom Left Of The Texture and Quad
                    rlTexCoord2f(tx, th); rlVertex3f(x,         y + height, z); // Bottom Right Of The Texture and Quad
                }
            rlEnd();
        rlPopMatrix();

        rlSetTexture(0);
    }
}
#include <iostream>
//also shamelessly copied, with alignments and return value added in
// Draw a 2D text in 3D space
Vector3 DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing,
                float lineSpacing, bool backface, Color tint, TextAlign align)
{
    int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;               // Offset X to next character to draw

    float scale = fontSize/(float)font.baseSize;

    Vector3 startPos = position;
    Vector2 dimen = MeasureTextEx(font,text,fontSize,fontSpacing);
    startPos.y -= dimen.y/2; //automatically center vertically

    if (align != LEFT)
    {
        startPos.x -= dimen.x/2*((align == RIGHT) + 1);
    }
    for (int i = 0; i < length;)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += fontSize + lineSpacing;
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint3D(font, codepoint, (Vector3){ startPos.x + textOffsetX, startPos.y  + textOffsetY, position.z }, fontSize, backface, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)font.recs[index].width*scale + fontSpacing;
            else textOffsetX += (float)font.glyphs[index].advanceX*scale + fontSpacing;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
    return { startPos.x + textOffsetX, startPos.y  + textOffsetY, position.z };
}
