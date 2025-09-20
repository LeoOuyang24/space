#include "../headers/portal.h"

Portal::Portal(int x, int y, int z, int r, const Vector3& dest_) : dest({dest_.x,dest_.y},0,dest_.z), Object(Vector2(x,y),
                                     std::make_tuple(r),
                                     std::make_tuple()
                                     )
 {
    orient.layer = z;

    portalShader = LoadShader(0,TextFormat("shaders/fragments/portal.h",330));


 }

 void Portal::collideWith(PhysicsBody& player)
 {
    player.orient.pos = {dest.pos.x,dest.pos.y};
    player.orient.layer = dest.layer;//Globals::Game.terrain.getZOfLayer(dest.z);
    if (&player == Globals::Game.player.get())
    {
        Globals::Game.setLayer(dest.layer);
    }
 }

 void Portal::render()
 {
    // Object::render();
    // renderer.render({CIRCLE,dest,&collider},WHITE);
    RenderTexture2D balls = LoadRenderTexture(100,100);

    DrawCircle3D({orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius,{0,0,0},0,RED);

    BeginShaderMode(portalShader);
        float time = GetTime();
        SetShaderValue(portalShader,GetShaderLocation(portalShader,"time"),&time,SHADER_UNIFORM_FLOAT);
        //DrawSphere({orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius,WHITE);
        DrawBillboard(Globals::Game.camera,balls.texture,{orient.pos.x,orient.pos.y,Globals::Game.terrain.getZOfLayer(orient.layer)},collider.radius*3,WHITE);
        DrawBillboard(Globals::Game.camera,balls.texture,{dest.pos.x,dest.pos.y,Globals::Game.terrain.getZOfLayer(dest.layer)},collider.radius*3,WHITE);

        //DrawTexture(balls.texture,orient.pos.x,orient.pos.y,WHITE);
        //DrawCircle(orient.pos.x,orient.pos.y,10,WHITE);
    EndShaderMode();
 }
