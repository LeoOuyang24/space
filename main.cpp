
#include <unordered_map>
#include <iostream>
#include <vector>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

#include "headers/blocks.h"
#include "headers/objects.h"
#include "headers/player.h"
#include "headers/resources_math.h"
#include "headers/debug.h"
#include "headers/game.h"

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


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const Vector2 screenDimen = {1920,1080};

    InitWindow(screenDimen.x, screenDimen.y, "raylib [core] example - basic window");
   // ToggleFullscreen();
    rlDisableBackfaceCulling();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------



    std::vector<PhysicsBody*> objs;

    const int minRadius = 10;
    const int maxRadius = 50;

    Vector2 planetCenter = Vector2(0,0);

    Terrain terrain(Terrain::MAX_WIDTH,Terrain::MAX_WIDTH);

    Camera3D& camera = Globals::Game.camera;

    camera.position = Vector3(0,0,0);
    camera.target = Vector3(0,0,Globals::BACKGROUND_Z);
    //camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    //camera.rotation = 0.0f;
    //camera.zoom = 1.0f;
    camera.up = {0,-1,0};
    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;

    Player::PlayerSprite = LoadTexture("sprites/guy.png");
    Player player(Vector2(0,150));


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

    RenderTexture2D bg = LoadRenderTexture(Terrain::MAX_WIDTH*screenDimen.x/screenDimen.y,Terrain::MAX_WIDTH);
    BeginTextureMode(bg);
        ClearBackground(BLACK);
        BeginShaderMode(stars);
            DrawTexture(bg.texture,0,0,WHITE);
        EndShaderMode();
        BeginShaderMode(sun);
            DrawTexture(bg.texture,0,0,WHITE);
        EndShaderMode();
    EndTextureMode();

    int frames = 0;
    //player.force = Vector2(100,0);
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
            player.update(terrain);
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
            camera.position.z += GetMouseWheelMove()*5;
            camera.target.z += GetMouseWheelMove()*5;
        }
        //Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(),camera);
        Vector2 mousePos = screenToWorld(GetMousePosition(),camera,screenDimen,Globals::Game.currentZ);
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            switch (spawns)
            {
            case PLANETS:
                terrain.generatePlanet(mousePos,50,Color(100,255,100,255 ));
                break;
            case OBJECTS:
                {
                    Color color = {255,255,255,100};
                    if (rand()%2)
                        objs.push_back(new Object<RectCollider,ShapeRenderer<ShapeType::RECT>>({mousePos,0},color,10,10));
                    else
                        objs.push_back(new Object<CircleCollider,ShapeRenderer<ShapeType::CIRCLE>>({mousePos,0},color,10));
                    break;
                }
            case PLAYER:
                player.orient.pos = mousePos;
                break;
            case ENDPOINT:
                endpoint = mousePos;
                break;

            }
        }
        else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            terrain.remove(mousePos,50);
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

            for (int i = 0; i < objs.size(); i ++)
            {
                objs[i]->update(terrain);
                for (int j = i + 1; j < objs.size(); j ++)
                {
                    if (CheckCollision(objs[i]->getShape(), objs[j]->getShape()))
                    {
                        Vector2 force = Vector2Normalize(objs[i]->getPos() - objs[j]->getPos() )*10;
                        objs[i]->addForce(force);
                        objs[j]->addForce(force*-1);
                    }
                }
            }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            BeginMode3D(camera);


                ClearBackground(BLACK);

                SetShaderValue(sun,timeLocation,&frames,SHADER_UNIFORM_INT);

                BeginShaderMode(stars);
                    DrawTexture(bg.texture,0,0,WHITE);
                EndShaderMode();
               BeginShaderMode(sun);
                    DrawTexture(bg.texture,0,0,WHITE);
                EndShaderMode();

                //DrawSphere({mousePos.x,mousePos.y,z},10,BLUE);
                DrawBillboardRec(camera,bg.texture,Rectangle(0,0,bg.texture.width,bg.texture.height),
                                 Vector3(Terrain::MAX_WIDTH/2,Terrain::MAX_WIDTH/2,Globals::BACKGROUND_Z),
                                 Vector2(bg.texture.width,bg.texture.height),WHITE);
                //DrawBillboard(camera,Player::PlayerSprite,Vector3(-10,-10,0),1,WHITE);
            //DrawTexture(bg.texture,0,0,WHITE);

                player.render();

                for (int i = 0; i < objs.size(); i ++)
                {
                    objs[i]->render();
                }


                terrain.render(-500);
            if (spawns == ENDPOINT)
            {
                DrawCircle(endpoint.x,endpoint.y,3,PURPLE);
                DrawLine(mousePos.x,mousePos.y,endpoint.x,endpoint.y,PURPLE);

                Vector2 onTerrain = terrain.lineTerrainIntersect(mousePos,endpoint).pos;
                DrawCircle(onTerrain.x,onTerrain.y,3,PURPLE);
            }


            Debug::renderDefers();

            EndMode3D();

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
