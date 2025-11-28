
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

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
#include "headers/collideTriggers.h"
#include "headers/item.h"
#include "headers/editor.h"




#include <rlgl.h>
#include <raymath.h>


#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif




bool active = false;
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const Vector2 screenDimen = {900,900};

    InitWindow(screenDimen.x, screenDimen.y, "raylib [core] example - basic window");
    rlDisableBackfaceCulling();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second


    Globals::Game.init();

    Camera3D& camera = Globals::Game.camera;

    Globals::Game.loadLevel("levels/layer0.txt");
    Globals::Game.setLayer(0);

    float accum = 0;
    float tick = 0.016;
    float speed = 1;


    Shader stars= LoadShader(0, TextFormat("shaders/fragments/stars.h", GLSL_VERSION));
    Shader sun = LoadShader(0,TextFormat("shaders/fragments/sun.h",GLSL_VERSION));

    SetShaderValue(stars, GetShaderLocation(stars,"screenDimen"), &screenDimen, SHADER_UNIFORM_VEC2);
    Vector4 sunCenter = {1,1,1,1};
    Vector4 sunEdge = {0,0.8,1,0.0};
    SetShaderValue(sun,GetShaderLocation(sun,"centerColor"),&sunCenter,SHADER_UNIFORM_VEC4);
    SetShaderValue(sun,GetShaderLocation(sun,"borderColor"),&sunEdge,SHADER_UNIFORM_VEC4);
    auto timeLocation = GetShaderLocation(sun,"time");

    //RenderTexture2D bg = LoadRenderTexture(5000*screenDimen.x/screenDimen.y,5000);//LoadRenderTexture(Terrain::MAX_WIDTH*screenDimen.x/screenDimen.y*2,Terrain::MAX_WIDTH*2);
    /*BeginTextureMode(bg);
        ClearBackground(BLACK);
        BeginShaderMode(stars);
            DrawTexture(bg.texture,0,0,WHITE);
        EndShaderMode();
        BeginShaderMode(sun);
            DrawTexture(bg.texture,0,0,WHITE);
        EndShaderMode();
    EndTextureMode();

    ExportImage(LoadImageFromTexture(bg.texture),"sprites/bg.png");*/

    int frames = 0;

    Terrain::GravityFieldShader = LoadShader(0,TextFormat("shaders/fragments/terrain.h",GLSL_VERSION));
    Texture& blocks = Globals::Game.getCurrentTerrain()->blocksTexture.texture;
    Vector2 pixelSizes = {1.0/blocks.width,1.0/blocks.height};
    SetShaderValue(Terrain::GravityFieldShader,GetShaderLocation(Terrain::GravityFieldShader,"pixelSizes"),&pixelSizes,SHADER_UNIFORM_VEC2);


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        float deltaTime = GetFrameTime();
        if ( IsKeyPressed(KEY_RIGHT_BRACKET) || !Debug::isPaused())
        {
        accum += deltaTime;
        Debug::clearRenderDefers();
        }

        while (accum >= tick/speed )
        {
            //player.update(*Globals::Game.getCurrentTerrain());
            Globals::Game.terrain.update(Globals::Game.getCurrentLayer());
            Sequences::runPhysics();

            accum -= tick/speed;
            frames ++;

        }

        if (GetMouseWheelMove())
        {

            //camera.zoom += ((float)GetMouseWheelMove()*0.05f);
            float move = GetMouseWheelMove()*5;
            camera.position.z += move;
            camera.target.z += move;
        }
        //Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(),camera);


        if (GetMousePosition().x >= 0.9*screenDimen.x)
        {
            camera.position.x += 10;
            camera.target.x += 10;
        }
        else if (GetMousePosition().x <= 0.1*screenDimen.x)
        {
            camera.position.x -= 10;
            camera.target.x -= 10;
        }

        if (GetMousePosition().y >= 0.9*screenDimen.y)
        {
            camera.position.y += 10;
            camera.target.y += 10;
        }
        else if (GetMousePosition().y <= 0.1*screenDimen.y)
        {
            camera.position.y -= 10;
            camera.target.y -= 10;
        }

        Debug::handleInput();



        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            BeginMode3D(camera);


                ClearBackground(BLACK);

                /*BeginShaderMode(stars);
                    DrawTexture(bg.texture,0,0,WHITE);
                EndShaderMode();
               BeginShaderMode(sun);
                    DrawTexture(bg.texture,0,0,WHITE);
                EndShaderMode();*/

                Texture2D& bg = *Globals::Game.Sprites.getSprite("bg.png");
                DrawBillboardRec(camera,bg,Rectangle(0,0,bg.width,bg.height),
                                 Vector3(Terrain::MAX_WIDTH/2,Terrain::MAX_WIDTH/2,Globals::BACKGROUND_Z),
                                 Vector2(bg.width,bg.height),WHITE);
                Globals::Game.terrain.render();

            Sequences::runRenders();
            Debug::renderDefers();

            EndMode3D();

            Globals::Game.interface.render();
            Debug::drawInterface();

            DrawFPS(10, 10);

        EndDrawing();


        //----------------------------------------------------------------------------------
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
