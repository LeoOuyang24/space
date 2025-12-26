#include "../headers/interface.h"
#include "../headers/game.h"
#include "../headers/player.h"
#include "../headers/raylib_helper.h"
#include "../headers/interactives.h"

void Interface::processSign()
{
    Vector2 screenDimen = {GetScreenWidth(),GetScreenHeight()};
    float wedge = .1;
    float hWedge = .3;
    auto message = sign.lock()->getMessage(messageIndex);
    Rectangle messageBox = {wedge*screenDimen.x,
                  hWedge*screenDimen.y,
                  (1 - wedge*2)*screenDimen.x,
                  (1 - hWedge*2)*screenDimen.y};
    DrawRectangle(messageBox.x,messageBox.y,messageBox.width,messageBox.height,
                  WHITE);
    float marginWidth = .05*messageBox.width;
    DrawRectangleLinesEx(messageBox,marginWidth,BROWN);

    DrawText2D(GetFontDefault(),
                fitText(GetFontDefault(),message,30,10,messageBox.width - marginWidth*2).c_str(),
               {messageBox.x + messageBox.width/2, messageBox.y + messageBox.height/2},
               30,10,BLACK,CENTER);

    auto key = GetKeyPressed();
    if (key != 0 && !justSet || messageIndex >= sign.lock()->getMessagesSize()) //if any key is pressed
    {
        setMessage(nullptr);
    }
    else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        messageIndex ++;
    }
    justSet = false;
}

void Interface::setMessage(Sign* message)
{
    justSet = message;
    sign = std::static_pointer_cast<Sign>(Globals::Game.objects.getObject(message));
    if (!sign.lock().get())
    {
        messageIndex = 0;
    }
}

void Interface::render()
{
    int i = 0;
    Vector2 screenDimen = {GetScreenWidth(),GetScreenHeight()};
    Texture2D& sprite = *Globals::Game.Sprites.getSprite("key.png");
    for (auto& it : Globals::Game.player->keys)
    {
        Vector2 point = {200 + i*100,100};
        float dimen = 50;
        DrawTexturePro(sprite,
                       Rectangle(0,0,sprite.width,sprite.height),
                       Rectangle(point.x,point.y,dimen,dimen),
                       Vector2(dimen/2,dimen/2),
                       0,it);
        i ++;
    }
    float height = Globals::Game.player->dying;
    DrawRectangle(100,screenDimen.x*.9 - height,100,height,RED);

    DrawText(std::to_string(Globals::Game.getCollects()).c_str(),.85*screenDimen.x,.9*screenDimen.y,50,WHITE);
    DrawTextureEx(*Globals::Game.Sprites.getSprite("gear.png"),{.8*screenDimen.x,.91*screenDimen.y},0,0.1,WHITE);
   // DrawBillboard(Globals::Game.camera,*Globals::Game.Sprites.getSprite("gear.png"),{.8*GetScreenWidth(),.9*GetScreenHeight(),0},50,WHITE);

    if (sign.lock().get())
    {
        processSign();
    }


}
