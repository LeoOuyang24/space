
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
                            Globals::Game.interface.setMenu(NONE);
                            Globals::Game.setLayer(0);
                            SoundLibrary::toggleBGM(true);
                            Globals::Game.Camera.startQueue();
                                Globals::Game.Camera.setCameraFollow(false,0);
                                Globals::Game.Camera.setCameraFollow(toVector2(Globals::Game.Camera.getCamera().position));
                                Globals::Game.Camera.lookAt(Globals::Game.terrain.getZOfLayer(0),300);
                                Globals::Game.Camera.setCameraFollow(true,100);
                                //Globals::Game.Camera.lookAt(toVector3(Globals::Game.getPlayer()->getPos()),100);
                            Globals::Game.Camera.stopQueue();

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
    //DrawRectangle(rect.x,rect.y,rect.width*.9,rect.height*.9,RED);
    DrawCircleGradient(rect.x,rect.y,rect.width/2,WHITE,Color(255,255,255,0));
}

void WorldMap::WorldNode::update(const Rectangle& rect)
{
    if (CheckCollisionPointRec(GetMousePosition(),rect)) //hovering
    {
        DrawRectangle(rect.x,rect.y,rect.width,rect.height,WHITE);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Globals::Game.setCurWorldThreaded(world);
            Globals::Game.interface.setMenu(Menus::NONE);
        }
    }
}


WorldMap::WorldMap()
{
    bg = LoadRenderTexture(GetScreenWidth(),GetScreenHeight());

    nodes[0] = WorldNode({bg.texture.width*0.6,bg.texture.height*0.7},0);
    nodes[1] = WorldNode({nodes[0].center.x - 200,nodes[0].center.y},1);
    nodes[2] = WorldNode({bg.texture.width/2, bg.texture.height/2},1);
    nodes[3] = WorldNode({nodes[1].center.x,bg.texture.height - nodes[1].center.y},1); 
    nodes[4] = WorldNode({nodes[0].center.x,nodes[3].center.y},1);

    BeginTextureMode(bg);
        ClearBackground(BLACK);
        for (int i = 0; i < 100; i ++)
        {
            Vector2 center = Vector2(rand()%(static_cast<int>(bg.texture.width)),rand()%static_cast<int>(bg.texture.height));
            DrawCircleGradient(
                center.x,
                center.y,
                rand()%3 + 1,
                Color{rand()%255,rand()%255,rand()%255,255},
                Color{0,0,0,255});
        }
        
        for (size_t i = 0; i < nodes.size() - 1; i ++)
        {
            DrawLine(
                nodes[i].center.x,nodes[i].center.y,
                nodes[i+1].center.x,nodes[i+1].center.y,
                WHITE);
        }

    EndTextureMode();
}

void WorldMap::render(const Rectangle& rect)
{
    //DrawRectangle(rect.x - rect.width/2,rect.y - rect.height/2,rect.width,rect.height,BLACK);
    DrawTexture(bg.texture,rect.x -  rect.width/2,rect.y - rect.height/2,WHITE);
}

void WorldMap::update(const Rectangle& rect)
{
    if (IsKeyPressed(KEY_E))
    {
        Globals::Game.interface.setMenu(NONE);
    }
}

void WorldMap::process(const Rectangle& rect)
{
    UIElement::process(rect);

    for (size_t i = 0; i < nodes.size(); i ++)
    {
        nodes[i].process({nodes[i].center.x,nodes[i].center.y,20,20});
    }
}