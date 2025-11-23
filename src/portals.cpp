#include "../headers/portal.h"

Portal::Portal(const Vector3& start, int r, const Vector3& dest_) : dest({dest_.x,dest_.y},dest_.z,0), Object({Vector2(start.x,start.y),start.z},
                                     std::make_tuple(r),
                                     std::make_tuple()
                                     )
 {
    portalShader = LoadShader(0,TextFormat("shaders/fragments/portal.h",330));
    texture = LoadRenderTexture(100,100);
    followGravity = false;

 }

 void Portal::collideWith(PhysicsBody& player)
 {

    if (&player == Globals::Game.player.get())
    {
        player.orient.pos = {dest.pos.x,dest.pos.y};
        Globals::Game.setLayer(dest.layer);
    }
 }

 void Portal::render()
 {
    // Object::render();
    // renderer.render({CIRCLE,dest,&collider},WHITE);

    DrawCircle3D({orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius,{0,0,0},0,RED);

    BeginShaderMode(portalShader);
        float time = GetTime();
        SetShaderValue(portalShader,GetShaderLocation(portalShader,"time"),&time,SHADER_UNIFORM_FLOAT);
        DrawBillboard(Globals::Game.camera,texture.texture,{orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius*3,WHITE);
        DrawBillboard(Globals::Game.camera,texture.texture,{dest.pos.x,dest.pos.y,Globals::Game.terrain.getZOfLayer(dest.layer)},collider.radius*3,WHITE);
    EndShaderMode();
 }

void TriggerPortalSpawn::interact(PhysicsBody& self, PhysicsBody& other)
 {
    //if active, a player has interacted with us, and either we are already unlocked or the player has the key, spawn the portal!
    if (this->active &&
        &other == Globals::Game.player.get() &&
        (this->lockVal == Key::unlocked || Key::unlocks(Globals::Game.player->keys,this->lockVal)))
    {
        Vector3 spawn = this->absolute ?
                            this->start:
                            Vector3(this->start.x + self.orient.pos.x, this->start.y + self.orient.pos.y, self.orient.layer);
       Portal* portal = new Portal(spawn,10,this->end);
       Globals::Game.addObject(*portal);
    }
 }
