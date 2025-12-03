#include "../headers/interface.h"
#include "../headers/game.h"
#include "../headers/player.h"

void Interface::render()
{
    int i = 0;
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
    float height = Globals::Game.player->power/Player::PLAYER_MAX_POWER*100;
    DrawRectangle(100,GetScreenHeight()*.9 - height,100,height,RED);

    DrawText(std::to_string(Globals::Game.getCollects()).c_str(),.85*GetScreenWidth(),.9*GetScreenHeight(),50,WHITE);
    DrawTextureEx(*Globals::Game.Sprites.getSprite("gear.png"),{.8*GetScreenWidth(),.91*GetScreenHeight()},0,0.1,WHITE);
   // DrawBillboard(Globals::Game.camera,*Globals::Game.Sprites.getSprite("gear.png"),{.8*GetScreenWidth(),.9*GetScreenHeight(),0},50,WHITE);
}
