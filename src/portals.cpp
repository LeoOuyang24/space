#include "../headers/portal.h"

UnlockCondition::UnlockCondition(PortalCondition* cond, Portal& owner) : InteractComponent(
                                                                        [this,&owner]
                                                                       (PhysicsBody& self, PhysicsBody& other){

                                                               if (!this->condition.get() || this->condition->unlocked())
                                                               {
                                                                self.orient.pos = {owner.dest.pos.x,owner.dest.pos.y};
                                                                Globals::Game.setLayer(owner.dest.layer);
                                                               }

                                                                                           }), condition(cond)
{

}

Shader Portal::PortalShader;

Portal::Portal() :  Object({Vector2(0,0),0},
                                     std::make_tuple(100),
                                     std::make_tuple()
                                     )
{
    texture = LoadRenderTexture(100,100);
    followGravity = false;
}
Portal::Portal(const Vector3& start, int r, const Vector3& dest_) : Portal()
 {
    assignVector(dest.pos,dest_);
    dest.layer = dest_.z;

    assignVector(orient.pos,start);
    orient.layer = start.z;
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

    BeginShaderMode(PortalShader);
        float time = GetTime();
        SetShaderValue(PortalShader,GetShaderLocation(PortalShader,"time"),&time,SHADER_UNIFORM_FLOAT);
        DrawBillboard(Globals::Game.camera,texture.texture,{orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius*3,WHITE);
        DrawBillboard(Globals::Game.camera,texture.texture,{dest.pos.x,dest.pos.y,Globals::Game.terrain.getZOfLayer(dest.layer)},collider.radius*3,WHITE);
    EndShaderMode();
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
