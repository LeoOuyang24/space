
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>


#include "headers/sprites.h"
#include "headers/factory.h"

#include "headers/blocks.h"
#include "headers/objects.h"
#include "headers/player.h"
#include "headers/resources_math.h"
#include "headers/debug.h"
#include "headers/game.h"
#include "headers/terrain.h"
#include "headers/sequencer.h"

#include "headers/portal.h"
#include "headers/item.h"
#include "headers/audio.h"

#include <rlgl.h>
#include <raymath.h>

#define PLATFORM_DESKTOP

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

void exportBackground()
{
    const Vector2 screenDimen = Globals::screenDimen;


    Shader stars= LoadShader(0, TextFormat("shaders/fragments/stars.h", GLSL_VERSION));
    Shader sun = LoadShader(0,TextFormat("shaders/fragments/sun.h",GLSL_VERSION));

    SetShaderValue(stars, GetShaderLocation(stars,"screenDimen"), &screenDimen, SHADER_UNIFORM_VEC2);
    Vector4 sunCenter = {1,1,1,1};
    Vector4 sunEdge = {1,0.5,0,0.0};
    SetShaderValue(sun,GetShaderLocation(sun,"centerColor"),&sunCenter,SHADER_UNIFORM_VEC4);
    SetShaderValue(sun,GetShaderLocation(sun,"borderColor"),&sunEdge,SHADER_UNIFORM_VEC4);
    //auto timeLocation = GetShaderLocation(sun,"time");



    RenderTexture2D bg = LoadRenderTexture(8000*2,8000);//LoadRenderTexture(Terrain::MAX_WIDTH*screenDimen.x/screenDimen.y*2,Terrain::MAX_WIDTH*2);

    BeginTextureMode(bg);
        ClearBackground(BLACK);

        for (int i = 0; i < 10000;i ++)
        {
            float randed = rand()%10000/10000.0*100;
            float angle = rand()%10000/10000.0*360*M_PI/180;
            int a = bg.texture.width;
            int b = bg.texture.height;
            float maxRadius = (a*b)/(sqrt(pow(b*cos(angle),2) + pow(a*sin(angle),2))); //radius of an ellipse at a given angle
            float dist = maxRadius/2*Lerp(0,1,randed/(randed+5));
            Vector2 center = {bg.texture.width/2 + cos(angle)*dist,bg.texture.height/2 + sin(angle)*dist};
            //Vector3 color = {rand()%100/100.0,rand()%100/100.0,rand()%100/100.0};
            DrawCircleGradient(center.x,center.y,5,WHITE,Color(rand()%255,rand()%255,rand()%255,0));
        }
        BeginShaderMode(sun);
            DrawTexture(bg.texture,0,0,WHITE);
        EndShaderMode();
        //DrawCircleGradient(bg.texture.width/2,bg.texture.height/2,bg.texture.width/3,WHITE,Color(0,255,255,0));
        //DrawCircle(bg.texture.width/2,bg.texture.height/2,bg.texture.width/100,WHITE);
    EndTextureMode();

    ExportImage(LoadImageFromTexture(bg.texture),"sprites/bg_scatter2.png");
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const Vector2 screenDimen = Globals::screenDimen;
    SetConfigFlags( FLAG_VSYNC_HINT);
    InitWindow(screenDimen.x, screenDimen.y, "raylib [core] example - basic window");
    InitAudioDevice();

    if constexpr (!Globals::DEBUG)
        ToggleBorderlessWindowed();

    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    rlDisableBackfaceCulling();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    ClassDeserializer::init();
    Globals::Game.init();

    Portal::PortalShader = LoadShader(0,TextFormat("shaders/fragments/portal.h",GLSL_VERSION));

    Globals::Game.addWorld("worlds/world0");
    Globals::Game.addWorld("worlds/world1");

    Globals::Game.Camera.moveCamera(Vector3{Terrain::MAX_TERRAIN_SIZE*0.5,Terrain::MAX_TERRAIN_SIZE*0.5,Globals::BACKGROUND_Z*0.9});


    Terrain::TerrainOutline = LoadShader(0,TextFormat("shaders/fragments/terrain_outline.h",GLSL_VERSION));
    SetShaderValue(Terrain::TerrainOutline,GetShaderLocation(Terrain::TerrainOutline,"pixelSizes"),&Terrain::PIXEL_SIZE,SHADER_UNIFORM_VEC2);
    SetShaderValue(Terrain::TerrainOutline,GetShaderLocation(Terrain::TerrainOutline,"outline_thickness"),&Block::BLOCK_DIMEN,SHADER_UNIFORM_VEC2);

    GlobalShaders::EllipseGradientShader = LoadShader(0,TextFormat("shaders/fragments/oval.h",GLSL_VERSION));

    SoundLibrary::loadBGM("music/world0_together.wav");
    //SoundLibrary::loadBGM("music/world1/world1.wav");
    SoundLibrary::toggleBGM(false);

    Globals::Game.setCurWorldThreaded(0);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        Globals::Game.update();
            // Draw
            //----------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(BLACK);
                Globals::Game.render();
                DrawFPS(10, 10);
            EndDrawing();
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
