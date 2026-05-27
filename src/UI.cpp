
#include "../headers/resources_math.h"
#include "../headers/raylib_helper.h"


#include "../headers/UI.h"

#include "../headers/objects.h"
#include "../headers/audio.h"


Button::Button(const Vector2& vec, ClickFunc func,std::string_view view) : minDimens(vec),onClick(func),message(view)
{

}

void Button::render(const Rectangle& box)
{
    Rectangle topLeft = {box.x - box.width/2,box.y - box.height/2,box.width,box.height};
    DrawRectangle(topLeft.x,topLeft.y,topLeft.width,topLeft.height,
        CheckCollisionPointRec(GetMousePosition(),topLeft) ? 
            GRAY : 
            WHITE);
    DrawRectangleLinesEx(topLeft,5,BLACK);

    DrawText2D(GetFontDefault(),message.c_str(),{box.x,box.y},30,5,BLACK,TextAlign::CENTER);
}

Vector2 Button::getMinDimens()
{
    return minDimens;
}

void Button::update(const Rectangle& box)
{
    if (CheckCollisionPointRec(GetMousePosition(),{box.x - box.width/2,box.y - box.height/2,box.width,box.height}) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        onClick();
    }
}



MainMenu::MainMenu() : play(
                            Globals::screenDimen*0.2,
                            [](){
                                
                                Globals::Game.setState(GameState::PLAYING);

                            },"START"),
                        quit(
                             Globals::screenDimen*0.2,
                             [](){exit(0);},"QUIT")
{

}

void MainMenu::process(const Rectangle& rect)
{
    Rectangle playRect = {0.5*rect.width,0.55*rect.height,play.getMinDimens().x,play.getMinDimens().y};
    play.process(playRect);

    Rectangle quitRect = {0.5*rect.width,0.8*rect.height,quit.getMinDimens().x,quit.getMinDimens().y};
    quit.process(quitRect);
}

WorldMap::WorldNode::WorldNode(const Vector2& pos, CurrentWorld current) : center(pos), world(current)
{

}


void WorldMap::WorldNode::render(const Rectangle& rect)
{
    DrawEllipseGradient({rect.x,rect.y,WorldMap::NODE_Z},rect.width/2,rect.height/2,WHITE,Color(color.r,color.g,color.b,0));
    if (world == Globals::Game.getCurWorld() && parent)
    {
        DrawSprite3D(Globals::Game.Sprites.getSprite("lifepod.png"),parent->camera.getCamera(),{rect.x,rect.y,100,100},WorldMap::NODE_Z);
    }
    else if (isHovered(rect) && world != Globals::Game.getCurWorld())
    {
        for (int i = 0; i < 360; i ++)
        {
            float radians = i*DEG2RAD;
            Vector2 start = Vector2(rect.x,rect.y) + Vector2(cos(radians),sin(radians))*ellipseRadius(radians,rect.width/2,rect.height/2);
            Vector2 end = Vector2(rect.x,rect.y) + Vector2(cos(radians + DEG2RAD),sin(radians + DEG2RAD))*ellipseRadius(radians + DEG2RAD,rect.width/2,rect.height/2);
            DrawLine3D(Vector3(start.x,start.y,WorldMap::NODE_Z),Vector3(end.x,end.y,WorldMap::NODE_Z),RED,10);
        }
    }
}

void WorldMap::WorldNode::update(const Rectangle& rect)
{
    if (isClicked(rect))
    {
        Globals::Game.setCurWorldThreaded(world);
        Globals::Game.setState(GameState::PLAYING);
        Globals::Game.interface.setMenu(Menus::NONE);
    }
}

bool WorldMap::WorldNode::isHovered(const Rectangle& rect)
{
    return (parent && 
        CheckCollisionPointRec(screenToWorld(GetMousePosition(),parent->camera.getCamera(),WorldMap::NODE_Z),
        {rect.x - rect.width/2,rect.y - rect.height/2,rect.width,rect.height}));
}

bool WorldMap::WorldNode::isClicked(const Rectangle& rect)
{
    return isHovered(rect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

WorldMap::WorldMap()
{
    bg = LoadRenderTexture(2000,2000);

    nodes[0] = WorldNode({bg.texture.width*0.6,bg.texture.height*0.7},0);
    nodes[1] = WorldNode({bg.texture.width/2, bg.texture.height/2},1);

    nodes[0].parent = this;
    nodes[1].parent = this;

    nodes[0].color = Color{0,255,255,0};
    nodes[1].color = Color{255,150,0,0};

    BeginTextureMode(bg);
        ClearBackground(BLACK);
        for (int i = 0; i < 1000; i ++)
        {
            Vector2 center = Vector2(rand()%(static_cast<int>(bg.texture.width)),rand()%static_cast<int>(bg.texture.height));
            int radius = rand()%2 + 1;
        
            DrawCircle(center.x,center.y,radius,WHITE);
            
        }
        
        for (size_t i = 0; i < nodes.size() - 1; i ++)
        {
            //DrawCircle(nodes[i].center.x,nodes[i].center.y,100,RED);
            DrawLine(
                nodes[i].center.x,bg.texture.height - nodes[i].center.y,
                nodes[i+1].center.x,bg.texture.height - nodes[i+1].center.y,
                WHITE);
        }

    EndTextureMode();

    camera.init({bg.texture.width,bg.texture.height},abs(CAMERA_Z));
    camera.setCameraFollow(false);
    camera.moveCamera({nodes[0].center.x,nodes[0].center.y,CAMERA_Z});

}

void WorldMap::render(const Rectangle& rect)
{
    //bg.texture = Globals::Game.getBG();
    //DrawRectangle(rect.x - rect.width/2,rect.y - rect.height/2,rect.width,rect.height,BLACK);
        //DrawTexture(bg.texture,rect.x -  rect.width/2,rect.y - rect.height/2,WHITE);
    DrawBillboardPro(camera.getCamera(),bg.texture,
        {0,0,bg.texture.width,bg.texture.height},
        {bg.texture.width/2,bg.texture.height/2,BG_Z},
        camera.getCamera().up,
        {bg.texture.width,bg.texture.height},
        {bg.texture.width/2,bg.texture.height/2},
        0,WHITE);
}

void WorldMap::update(const Rectangle& rect)
{
    if (IsKeyPressed(KEY_E))
    {
        //Globals::Game.interface.setMenu(NONE);
    }
    if (GetMouseWheelMove())
    {

        //camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        float move = GetMouseWheelMove()*10;
        //camera.position.z += move;
        //camera.target.z += move;
        //camera.moveCamera(camera.getCamera().position + Vector3(0,0,move));
    }
    Vector2 screenDimen = GetScreenDimen();
    if (GetMousePosition().x >= 0.9*screenDimen.x)
    {
        camera.moveCamera(camera.getCamera().position + Vector3(10,0,0));
    }
    else if (GetMousePosition().x <= 0.1*screenDimen.x)
    {
        camera.moveCamera(camera.getCamera().position + Vector3(-10,0,0));
    }

    if (GetMousePosition().y >= 0.9*screenDimen.y)
    {
        camera.moveCamera(camera.getCamera().position + Vector3(0,10,0));
    }
    else if (GetMousePosition().y <= 0.1*screenDimen.y)
    {
        camera.moveCamera(camera.getCamera().position + Vector3(0,-10,0));
    }
    if (IsKeyPressed(KEY_C))
    {
        std::cout << camera.getCamera().position << "\n";
    }
}

void WorldMap::process(const Rectangle& rect)
{
    update(rect);
    for (size_t i = 0; i < nodes.size(); i ++)
    {
        nodes[i].update({nodes[i].center.x,nodes[i].center.y,400,300});
    }
    BeginMode3D(camera.getCamera());
        render(rect);
        for (size_t i = 0; i < nodes.size(); i ++)
        {
            nodes[i].render({nodes[i].center.x,nodes[i].center.y,400,300});
        }
    EndMode3D();

}