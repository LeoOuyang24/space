#include "../headers/portal.h"
#include "../headers/sequencer.h"

#include "rlgl.h"

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
               fontSize,10,0,false,unlocked() ? BLACK : RED,CENTER);

    pos.x += fontSize/2;
    pos.y += fontSize/2;
    DrawBillboard(Globals::Game.getCamera(),Globals::Game.Sprites.getSprite("gear.png"),pos,fontSize,WHITE);
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
    followGravity = false;
    cond.reset(new TokenLocked(5));

}

Portal::Portal(const Vector2& pos, LayerType layer, const Vector2& destPos_, LayerType layerDisp_) : Portal()
{
    cond.reset();
    setOrient({pos,layer});
    destPos = destPos_;
    layerDisp = layerDisp_;
}

bool Portal::unlocked()
{
    return !cond.get() || cond->unlocked();
}

 void Portal::interactWith(PhysicsBody& obj)
 {
    if (unlocked())
    {
        Player* player = static_cast<Player*>(&obj);
        player->setState(Player::State::PORTALLING);

        player->setState(Player::State::PORTALLING);

        Sequences::add(false,[pos = getPos(), player,startPos = player->getPos(),duration=30.0f](int x){
                    player->setPos(lerp(startPos,pos,sin(std::min(x,(int)duration/2)/duration*M_PI))); //in 15 seconds, move the player to the center
                    return x >= duration; //wait 30 seconds
                     })
            ->add(Globals::Game.Camera.setCameraFollow(Vector3{destPos.x,destPos.y,Globals::Game.terrain.getZOfLayer(orient.layer + layerDisp)},120))
            .parallel([player](int){player->setPos(Globals::Game.getCamera().target); return true;})
                .add([player,destPos=this->destPos,destLayer=orient.layer + layerDisp](int){
                    Globals::Game.Camera.setCameraFollow(true);
                    Globals::Game.setLayer(destLayer);
                    player->setState(Player::State::WALKING);
                    player->setPos(destPos);
                    player->orient.setZ();
                    return true;
                });
    }
 }


 void Portal::render()
 {
    BeginShaderMode(PortalShader);
        float time = GetTime();
        Vector4 tint = unlocked() ? Vector4{1,1,0,0} : Vector4{0.5,0.5,0.5,0};
        SetShaderValue(PortalShader,GetShaderLocation(PortalShader,"time"),&time,SHADER_UNIFORM_FLOAT);
        SetShaderValue(PortalShader,GetShaderLocation(PortalShader,"tint"),&tint,SHADER_UNIFORM_VEC4);

        DrawBlankSprite(toVector3(orient.pos),{collider.radius*2,collider.radius*2},0);

    EndShaderMode();

       if (cond.get())
        {
            cond->render(getShape());
        }
 }
