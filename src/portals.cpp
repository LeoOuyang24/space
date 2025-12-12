#include "../headers/portal.h"

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
Portal::Portal(const Vector3& start, int r, const Vector3& dest_) : Portal()
 {
    assignVector(dest.pos,dest_);
    dest.layer = dest_.z;

    assignVector(orient.pos,start);
    orient.layer = start.z;

}

bool Portal::unlocked()
{
    return !cond.get() || cond->unlocked();
}

 void Portal::interactWith(PhysicsBody& player)
 {
    if (unlocked())
    {
        player.orient.pos = {dest.pos.x,dest.pos.y};
        Globals::Game.setLayer(dest.layer);
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

void TriggerPortalSpawn::interact(PhysicsBody& self, PhysicsBody& other)
 {
    //if active, a player has interacted with us, and either we are already unlocked or the player has the key, spawn the portal!
    if (this->active &&
        &other == Globals::Game.player.get() &&
        (this->lockVal == Key::UNLOCKED || Key::unlocks(Globals::Game.player->keys,this->lockVal)))
    {
        Vector3 spawn = this->absolute ?
                            this->start:
                            Vector3(this->start.x + self.orient.pos.x, this->start.y + self.orient.pos.y, self.orient.layer);
       Portal* portal = new Portal(spawn,10,this->end);
       Globals::Game.addObject(*portal);
    }
 }
