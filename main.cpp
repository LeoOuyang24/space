
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
    auto timeLocation = GetShaderLocation(sun,"time");

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
            Vector3 color = {rand()%100/100.0,rand()%100/100.0,rand()%100/100.0};
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

    Globals::Game.init();
    Portal::PortalShader = LoadShader(0,TextFormat("shaders/fragments/portal.h",GLSL_VERSION));

    const Camera3D& camera = Globals::Game.getCamera();
    //Globals::Game.loadLevel("levels/load_this.txt");

    //World world1 = {{"levels/layer0.txt","levels/layer1.txt","levels/layer2.txt"}};
    //Globals::Game.loadLevel("levels/block.txt");

    Globals::Game.addWorld("worlds/world1");
    Globals::Game.setCurWorld(0);

    Globals::Game.moveCamera(Vector3{Terrain::MAX_TERRAIN_SIZE*0.5,Terrain::MAX_TERRAIN_SIZE*0.5,Globals::BACKGROUND_Z*0.9});


    float accum = 0;
    float tick = 1/60.0f;
    float speed = 1;

    //exportBackground();

    int frames = 0;

    Terrain::GravityFieldShader = LoadShader(0,TextFormat("shaders/fragments/terrain.h",GLSL_VERSION));
    Texture& blocks = Globals::Game.getCurrentTerrain()->blocksTexture.texture;
    Vector2 pixelSizes = {1.0/blocks.width,1.0/blocks.height};
    SetShaderValue(Terrain::GravityFieldShader,GetShaderLocation(Terrain::GravityFieldShader,"pixelSizes"),&pixelSizes,SHADER_UNIFORM_VEC2);

    SoundLibrary::loadBGM("music/world3.wav");
    SoundLibrary::toggleBGM(false);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //SoundLibrary::update();

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
        frames = 0;

        if (GetMouseWheelMove())
        {

            //camera.zoom += ((float)GetMouseWheelMove()*0.05f);
            float move = GetMouseWheelMove()*5;
            //camera.position.z += move;
            //camera.target.z += move;
            Globals::Game.moveCamera(Globals::Game.getCamera().position + Vector3(0,0,move));
        }
        if (!Debug::isDebugOn() && Globals::Game.player && Globals::Game.getCameraFollow())
        {
            Globals::Game.moveCamera(Globals::Game.player->orient.pos);
        }
        if constexpr (Globals::DEBUG)
            Debug::handleInput();
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();


            Rectangle r1 = {450,450,200,50};
            Rectangle r2 = {550,550,100,50};

            float rot1 = GetTime()*0.5;
            float rot2 = GetTime()*0.25;

            bool collided = CheckCollisionRecsRotated(r1,r2,rot1,rot2);


            BeginMode3D(camera);


                ClearBackground(BLACK);

                /*BeginShaderMode(stars);
                    DrawTexture(bg.texture,0,0,WHITE);
                EndShaderMode();
               BeginShaderMode(sun);
                    DrawTexture(bg.texture,0,0,WHITE);
                EndShaderMode();*/

                Texture2D bg  = Globals::Game.getBG();//Globals::Game.Sprites.getSprite("bg_scatter.png");
                DrawBillboardRec(camera,bg,Rectangle(0,0,bg.width,bg.height),
                                 Vector3(Terrain::MAX_TERRAIN_SIZE/2,Terrain::MAX_TERRAIN_SIZE/2,Globals::BACKGROUND_Z),
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
