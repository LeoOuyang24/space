#include "../headers/portal.h"

Portal::Portal(int x, int y, int z, int r, const Vector3& dest_) : dest({dest_.x,dest_.y},dest_.z,0), Object({Vector2(x,y),z},
                                     std::make_tuple(r),
                                     std::make_tuple()
                                     )
 {
    portalShader = LoadShader(0,TextFormat("shaders/fragments/portal.h",330));
    texture = LoadRenderTexture(100,100);

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

 InteractComponent::InteractAction TriggerPortalSpawn::createInteractFunc(TriggerPortalSpawn& self, const Vector2& disp)
 {
     return [&self,disp](PhysicsBody& owner, PhysicsBody& other)
    {
        if (self.active && self.ptr.get() &&
            &other == Globals::Game.player.get() &&
            ((self.lockVal == Key::unlocked && !Globals::Game.player->getHolding()) || (Globals::Game.player->getHolding() && Key::unlocks(Globals::Game.player->getHolding()->getKey(),self.lockVal))))
        {
            self.active = false;
            self.ptr->orient.pos = owner.orient.pos + disp;
            Globals::Game.addObject(self.ptr);
        }
    };
 }
