#include "../headers/portal.h"
#include "../headers/sequencer.h"

Shader Portal::PortalShader;

bool TokenLocked::unlocked()
{
    return Globals::Game.getCollects() >= requirement;
}

void TokenLocked::render(Shape shape)
{
    std::string str = std::to_string(requirement);
    int fontSize = 50;
    Vector3 pos =

    DrawText3D(GetFontDefault(),
               str.c_str(),
               Globals::Game.terrain.orientToVec3(shape.orient),
               fontSize,10,0,false,unlocked() ? WHITE : RED,CENTER);

    pos.x += fontSize/2;
    pos.y += fontSize/2;
    DrawBillboard(Globals::Game.camera,*Globals::Game.Sprites.getSprite("gear.png"),pos,fontSize,WHITE);
}

std::string TokenLocked::toString()
{
    return "gear," + std::to_string(requirement);
}

Portal::Portal() :  Object({Vector2(0,0),0},
                                     std::make_tuple(100),
                                     std::make_tuple()
                                     )
{
    texture = LoadRenderTexture(100,100);
    followGravity = false;
    cond.reset(new TokenLocked(5));

}

bool Portal::unlocked()
{
    return !cond.get() || cond->unlocked();
}

 void Portal::interactWith(PhysicsBody& player)
 {
    if (unlocked())
    {
        static_cast<Player*>(&player)->setState(Player::State::PORTALLING);
       // RunThis r = RunThis::Func([](int){return true;});
        //player.orient.pos = {dest.pos.x,dest.pos.y};
        Sequences::add(true,
                       [&player,pos=this->orient.pos](int x){
                        player.setPos(pos);
                       return x >= 30; //wait 30 frames (~0.5 second)

                       },
                       [dest=this->destPos,start = player.getPos()](int x){
                        Player* player = static_cast<Player*>(Globals::Game.getPlayer());
                        player->setPos(start + (dest - start)*.01f*x);
                        return Vector2Equals(player->getPos(),dest) || x >= 100; //fail safe, this can only run 100 times
                       },
                        [this](int)
                        {
                            static_cast<Player*>(Globals::Game.getPlayer())->setState(Player::State::WALKING);
                            Globals::Game.setLayer(orient.layer + layerDisp);
                            return true;
                        }
                       );
    }
 }


 void Portal::render()
 {
    DrawCircle3D({orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius,{0,0,0},0,RED);

    BeginShaderMode(PortalShader);
        float time = GetTime();
        Vector4 tint = unlocked() ? Vector4{1,1,0,0} : Vector4{0.5,0.5,0.5,0};
        SetShaderValue(PortalShader,GetShaderLocation(PortalShader,"time"),&time,SHADER_UNIFORM_FLOAT);
        SetShaderValue(PortalShader,GetShaderLocation(PortalShader,"tint"),&tint,SHADER_UNIFORM_VEC4);
        DrawBillboard(Globals::Game.camera,texture.texture,{
                      orient.pos.x,orient.pos.y,
                      Globals::Game.terrain.getZOfLayer(orient.layer)},
                      collider.radius*3,WHITE);
        //DrawBillboard(Globals::Game.camera,texture.texture,{dest.pos.x,dest.pos.y,Globals::Game.terrain.getZOfLayer(dest.layer)},collider.radius*3,WHITE);


    EndShaderMode();

       if (cond.get())
        {
            cond->render(getShape());
        }
 }

