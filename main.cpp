
#include <unordered_map>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>


#include "headers/blocks.h"
#include "headers/objects.h"
#include <raymath.h>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

const int planetRadius = 100;

Vector2 getRandDistFrom(Vector2 center, int distanceFrom = 500)
{
    float distance = rand()%distanceFrom + planetRadius*1.1;
    float radians = M_PI/180*(rand()%360);
    Vector2 pos = center + Vector2(cos(radians),sin(radians))*distance;
    return pos;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    ToggleFullscreen();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Vector2 center = Vector2(screenWidth/2,screenHeight/2);

    std::vector<Object*> objs;

    const int minRadius = 10;
    const int maxRadius = 50;
    for (int i = 0; i < 5000; i ++)
    {
        Object* obj = new Object{Vector2(rand()%1000-500,rand()%1000 - 500),rand()%10 + 1};
        objs.push_back(obj);
       obj->force = Vector2(100,0);
    }

    GlobalTerrain terrain;
    //terrain.generatePlanets();

   terrain.generatePlanet(Vector2(0,0),200);
   terrain.generatePlanet(Vector2(1000,500),100);
   terrain.generatePlanet(Vector2(1000,-500),250);


    Camera2D camera;

    camera.target = Vector2(0,0);
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    // Main game loop

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        float deltaTime = GetFrameTime();
        //std::cout << 1/deltaTime << "\n";
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        if (IsKeyDown(KEY_A))
        {
            camera.target.x -= 100*deltaTime;
        }
        if (IsKeyDown(KEY_D))
        {
            camera.target.x += 100*deltaTime;
        }
        if (IsKeyDown(KEY_W))
        {
            camera.target.y -= 100*deltaTime;
        }
        if (IsKeyDown(KEY_S))
        {
            camera.target.y += 100*deltaTime;
        }
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(),camera);
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            terrain.remove(mousePos,50);
        }
        else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            terrain.generatePlanet(mousePos,50);
        }



        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            BeginMode2D(camera);

                ClearBackground(BLACK);


                /*Vector2 testCenter = Vector2(0,0);
                int testRadius = 1000;
                int amount = 0;

                Rectangle cameraRect = {camera.target.x - camera.offset.x/camera.zoom,camera.target.y - camera.offset.y/camera.zoom, camera.offset.x*2/camera.zoom,camera.offset.y*2/camera.zoom};
                terrain.forEachPosTest([&testCenter,testRadius,&amount](const Vector2& pos){
                                                                       amount ++;

                                        DrawCircle(pos.x,pos.y,5,Color(0,255,0,100));
                                   },testCenter,testRadius);

                int newAmount = 0;
                int noIf = 0;
                terrain.forEachPosPerim([&testCenter,testRadius, &newAmount,&noIf,&cameraRect](const Vector2& pos){
                                            newAmount ++;
                                            DrawCircle(pos.x,pos.y,5,Color(255,0,0,100));

                                                                   },testCenter,testRadius);*/
               // std::cout << amount << " " << newAmount << " " << noIf << "\n";
                terrain.render();
              for (Object* obj : objs)
                {

                    int searchRad = 300;
                    bool nearby = false;

                    terrain.forEachPosPerim([&nearby,&terrain](const Vector2& pos){
                            if (terrain.blockExists(pos))
                            {
                                nearby = true;
                            }
                                            },obj->pos,searchRad);
                    if (nearby)
                    {
                        terrain.forEachPos([obj,&terrain](const Vector2& pos){
                            if (terrain.blockExists(pos))
                            {
                                float mag = (obj->radius)/pow(Vector2Length(pos - obj->pos),2);
                                obj->force += (pos - obj->pos)*mag;
                            }
                                   },obj->pos,searchRad);
                    }

            float mag = Vector2Length(obj->force);
            if (mag >= 1000.0f)
            {
                obj->force = Vector2Normalize(obj->force)*1000.0f;
            }
            //std::cout << mag << "\n";
            obj->force *= .99;

            obj->pos += obj->force*deltaTime;
                    obj->render();
                   // DrawCircleLines(obj->pos.x,obj->pos.y,300,RED);
                }
                DrawPoly(mousePos,8,10,0,RED);

            EndMode2D();
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
