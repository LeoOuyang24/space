
#include <unordered_map>
#include <iostream>
#include <vector>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

#include "headers/sprites.h"

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

#include <rlgl.h>
#include <raymath.h>


#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif


enum Spawns
{
    PLANETS,
    OBJECTS,
    PLAYER,
    ENDPOINT,
    SIZE
};


bool active = false;
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const Vector2 screenDimen = {900,900};

    InitWindow(screenDimen.x, screenDimen.y, "raylib [core] example - basic window");
   // ToggleFullscreen();
    rlDisableBackfaceCulling();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------



    std::vector<PhysicsBody*> objs;

    const int minRadius = 10;
    const int maxRadius = 50;

    Globals::Game.init();
    //Terrain terrain(Terrain::MAX_WIDTH,Terrain::MAX_WIDTH);

    /*Globals::Game.terrain.pushBackTerrain();

    for (int i = 1; i <2; i ++)
    {
        Globals::Game.terrain.pushBackTerrain();
        Globals::Game.terrain.getTerrain(i)->generatePlanets();
    }*/




    //Globals::Game.terrain.getTerrain(1)->generatePlanet({500,500},100,RED);

    Camera3D& camera = Globals::Game.camera;


    Player& player = *Globals::Game.player;

    Texture2D keySprite = LoadTexture("sprites/key.png");
    Texture2D lockedSprite = LoadTexture("sprites/locked.png");

    //std::get<HoldThis>(tup);

    //auto* key = new Object<RectCollider,TextureRenderer,HoldThis>({0,200},std::make_tuple(10,10),{},std::make_tuple(HoldThis()));
    auto* key = new Key(RED,{0,200},{20,20},keySprite);
    auto* key2 = new Key(YELLOW,{2439,2600},{20,20},keySprite);

    auto* locked =  new PortalSpawner({{1600,2800},0},std::make_tuple(20),std::make_tuple(lockedSprite),
                                                                            createArgs<TriggerPortalSpawn>(true,Vector2(0,-100),0,Vector3(600,500,1),100,RED));

    Globals::Game.addObject(*(key));
    Globals::Game.addObject(*locked);
    Globals::Game.addObject(*key2);



   // std::cout << "actual: " <<&(locked->collideTrigger) << "\n";

   //locked->followGravity = false;
   //key->followGravity = false;

   Spawns spawns = PLANETS;
    int a = 0;

    float accum = 0;
    float tick = 0.016;
    float speed = 1;


    Vector2 endpoint = {};
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
    //player.force = Vector2(100,0);

    Globals::Game.Sprites.addSprite("key.png","bg.png");

    Terrain::GravityFieldShader = LoadShader(0,TextFormat("shaders/fragments/terrain.h",GLSL_VERSION));
    Texture& blocks = Globals::Game.getCurrentTerrain()->blocksTexture.texture;
    Vector2 pixelSizes = {1.0/blocks.width,1.0/blocks.height};
    SetShaderValue(Terrain::GravityFieldShader,GetShaderLocation(Terrain::GravityFieldShader,"pixelSizes"),&pixelSizes,SHADER_UNIFORM_VEC2);


    Debug::togglePaused();

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        float deltaTime = GetFrameTime();
        if (!Debug::isPaused() || IsKeyPressed(KEY_RIGHT_BRACKET))
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
        //std::cout << 1/deltaTime << "\n";
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        if (IsKeyPressed(KEY_ONE))
        {
            spawns = static_cast<Spawns>(((int)spawns + 1)%Spawns::SIZE);
            a = 255;
        }

        if (GetMouseWheelMove())
        {

            //camera.zoom += ((float)GetMouseWheelMove()*0.05f);
            float move = GetMouseWheelMove()*5;
            camera.position.z += move;
            camera.target.z += move;
        }
        //Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(),camera);
        Vector2 mousePos = screenToWorld(GetMousePosition(),camera,screenDimen,Globals::Game.getCurrentZ());
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            switch (spawns)
            {
            case PLANETS:
                Globals::Game.getCurrentTerrain()->generatePlanet(mousePos,50,Color(100,255,100,255 ));
                break;
            case OBJECTS:
                {
                    Color color = {255,255,255,100};
                    if (rand()%2)
                        Globals::Game.addObject(*(new Object<RectCollider,ShapeRenderer<ShapeType::RECT>>({mousePos,Globals::Game.getCurrentLayer()},color,10,10)));
                    else
                        Globals::Game.addObject(*(new Object<CircleCollider,ShapeRenderer<ShapeType::CIRCLE>>({mousePos,Globals::Game.getCurrentLayer()},color,10)));
                    break;
                }
            case PLAYER:
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                {
                    player.orient.pos = mousePos;
                }
                break;
            case ENDPOINT:
                endpoint = mousePos;
                break;

            }
        }
        else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            Globals::Game.getCurrentTerrain()->remove(mousePos,50);
        }

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

            if (spawns == ENDPOINT)
            {
                float z = Globals::Game.getCurrentZ();
                DrawCircle3D(Vector3(endpoint.x,endpoint.y,z),3,{0,1,0},0,PURPLE);
                DrawLine3D(Vector3(mousePos.x,mousePos.y,z),Vector3(endpoint.x,endpoint.y,z),PURPLE);

                Vector2 onTerrain = Globals::Game.getCurrentTerrain()->lineTerrainIntersect(mousePos,endpoint).pos;
                DrawCircle3D(Vector3(onTerrain.x,onTerrain.y,z),3,{0,1,0},0,PURPLE);
            }

            Sequences::runRenders();
            Debug::renderDefers();

            EndMode3D();

            Globals::Game.interface.render();

            DrawText(spawns == PLANETS ? "planets" : spawns == OBJECTS ?  "objects"  : spawns == ENDPOINT ? "endpoint" : "player" ,10,50,30,WHITE);

            DrawFPS(10, 10);

        EndDrawing();

        if (IsKeyPressed(KEY_BACKSLASH))
        {
            Debug::togglePaused();
        }
        //----------------------------------------------------------------------------------
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
