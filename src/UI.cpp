
#include "../headers/resources_math.h"
#include "../headers/raylib_helper.h"


#include "../headers/UI.h"

#include "../headers/objects.h"


Button::Button(const Vector2& center, const Vector2& dimen, ClickFunc func,std::string_view view) : box({center.x - dimen.x/2,
                                                                                                        center.y - dimen.y/2,
                                                                                                        dimen.x,dimen.y}),onClick(func),message(view)
{

}

void Button::render()
{
    DrawRectangle(box.x,box.y,box.width,box.height,CheckCollisionPointRec(GetMousePosition(),box) ? GRAY : WHITE);
    DrawRectangleLinesEx(box,5,BLACK);

    DrawText2D(GetFontDefault(),message.c_str(),{box.x + box.width/2,box.y + box.height/2},30,5,BLACK,TextAlign::CENTER);
}

void Button::update()
{
    if (CheckCollisionPointRec(GetMousePosition(),box) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        onClick();
    }
}

MainMenu::MainMenu() : play({0.5*Globals::screenDimen.x,0.55*Globals::screenDimen.y},
                            Globals::screenDimen*0.2,
                            [](){
                            Globals::Game.interface.setMenu(NONE);
                            Sequences::add(true,[](int x){
                                            constexpr float its = 100.0f;
                                            float z = lerp(Globals::Game.camera.position.z,Globals::START_Z - Globals::CAMERA_Z_DISP,1/its);
                                            Globals::Game.camera.position.z = z;
                                           return x > 300;

                                           },
                                           [](int x){

                                           moveCamera(Globals::Game.camera,lerp({3000,3000},Globals::Game.getPlayer()->getPos(),x/100.0f));
                                           return x > 100;

                                           },

                                           [](int){
                                            Globals::Game.setCameraFollow(true);
                                           Globals::Game.setLayer(0);
                                           return true;

                                           });

                            },"START"),
                        quit({0.5*Globals::screenDimen.x,0.8*Globals::screenDimen.y},
                             Globals::screenDimen*0.2,
                             [](){exit(0);},"QUIT")
{

}

void MainMenu::update()
{
    play.update();
    play.render();

    quit.update();
    quit.render();
}
