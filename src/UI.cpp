
#include "../headers/resources_math.h"
#include "../headers/raylib_helper.h"


#include "../headers/UI.h"

#include "../headers/objects.h"
#include "../headers/audio.h"


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
