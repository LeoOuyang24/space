
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

struct A
{
    int x; int y; int z;
    A(int x_,int y_, int z_) : x(x_),y(y_),z(z_)
    {

    }
};

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
   // ToggleFullscreen();
    rlDisableBackfaceCulling();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    std::tuple balls = {1,2,3};
    std::make_from_tuple<A>(balls);


    std::vector<PhysicsBody*> objs;

    const int minRadius = 10;
    const int maxRadius = 50;

    Vector2 planetCenter = Vector2(0,0);

    GlobalTerrain terrain;
    //terrain.generatePlanets();

   //terrain.generatePlanet(planetCenter,radius);
   /*terrain.terrain[Vector2(0,0)].reset(new Block(Vector2(0,0),RED));
    terrain.terrain[Vector2(10,0)].reset(new Block(Vector2(10,0),RED));
    terrain.terrain[Vector2(0,10)].reset(new Block(Vector2(0,10),RED));
    terrain.terrain[Vector2(10,10)].reset(new Block(Vector2(10,10),RED));*/

    //terrain.generateRightTriangle({-100,0},100,RED);
    //terrain.generateRect({100,100,100,100},RED);
   //terrain.generatePlanet(Vector2(1000,500),100);
   //terrain.generatePlanet(Vector2(1000,-500),250);

    Camera2D camera;

    camera.target = Vector2(screenWidth/2,screenHeight/2);
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    //terrain.terrain[Vector2(980,400)].reset(new Block{Vector2(980,400),Color{255,0,0,128}});


    /*terrain.forEachPos([&terrain](const Vector2& vec){
                       if (!terrain.blockExists(vec))
                       {
                           std::cout << vec.x << " " << vec.y << " " << Vector2Distance(Vector2(1000,500),Vector2(980,400)) << "\n";
                       }
                       terrain.terrain[vec]->color.a += 50;
                       },Vector2(1000,500),100,100.0f);*/
    // Main game loop

    Player::PlayerSprite = LoadTexture("sprites/guy.png");
    Player player(Vector2(0,150));


   Spawns spawns = PLANETS;
    int a = 0;

    float accum = 0;
    float tick = 0.016;
    float speed = 1;


    Vector2 endpoint = {};

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
                    camera.target += Vector2Normalize(GetMousePosition() - Vector2{screenWidth/2,screenHeight/2});

            camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        }
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(),camera);


        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            switch (spawns)
            {
            case PLANETS:
                terrain.generatePlanet(mousePos,50,Color(255,0,0,100 ));
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

            BeginMode2D(camera);

                ClearBackground(BLACK);
                player.render();


                for (int i = 0; i < objs.size(); i ++)
                {
                    objs[i]->render();
                }



                terrain.render();
            if (spawns == ENDPOINT)
            {
                DrawCircle(endpoint.x,endpoint.y,3,PURPLE);
                DrawLine(mousePos.x,mousePos.y,endpoint.x,endpoint.y,PURPLE);

                Vector2 onTerrain = terrain.lineTerrainIntersect(mousePos,endpoint).pos;
                DrawCircle(onTerrain.x,onTerrain.y,3,PURPLE);
            }


            Debug::renderDefers();

            EndMode2D();
                                  //DrawPoly({100,100},10,10,0,PURPLE);

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
