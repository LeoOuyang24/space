#include "../headers/resources_math.h"
#include "../headers/camera.h"
#include "../headers/sequencer.h"
#include "../headers/game.h"
#include "../headers/objects.h"

void GameCamera::init(const Vector2& bounds_, float maxCameraDisp_)
{
    
    camera.projection = CAMERA_PERSPECTIVE;
   // camera.position = Vector3(0,0,Globals::Game.getCurrentZ() - Globals::CAMERA_Z_DISP);
    camera.target = Vector3(0,0,Globals::BACKGROUND_Z);
    //camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    //camera.rotation = 0.0f;
    //camera.zoom = 1.0f;
    camera.up = {0,-1,0};
    camera.fovy = 90;

    bounds = bounds_;
    maxCameraDisp = maxCameraDisp_;
}

void GameCamera::update()
{
    if (cameraFollow && Globals::Game.getPlayer() && !Debug::isDebugOn())
    {
        lookAt(toVector3(Globals::Game.getPlayer()->getPos()),0);
    }
}

Sequencer GameCamera::setCameraFollow(bool val, int transition )
{
    //if no transition, we can just set the value and be done
    if (transition == 0)
    {
        cameraFollow = val;
        return Sequencer();
    }

    Sequencer seq;

    //if going from false to true, we only want to set the value AFTER transition is done
    //the reason is because GameCamera::update will set our position every frame based on the value
    //of "cameraFollow". So we have to be careful for when we set the variable
    if (val) 
    {
        seq = lookAt([](){return toVector3(Globals::Game.getPlayer()->getPos());},transition)
            .add([this](int){ this->cameraFollow = true; return true;});
    }
    else //else if going from true to false, we can set the value immediately then do the transition.
    {
        cameraFollow = val;
        seq = lookAt(cameraFollowPoint,transition);
    }
    return seq;
}

Sequencer GameCamera::setCameraFollow(const Vector3& pos, int transition )
{
    cameraFollowPoint = pos; 
    return setCameraFollow(false,transition);
}

Sequencer GameCamera::setCameraFollow(const Vector2& point, int transition)
{
    return setCameraFollow({point.x,point.y,camera.target.z},transition);
}

bool GameCamera::getCameraFollow()
{
    return cameraFollow;
}

Sequencer GameCamera::moveCamera(const Vector3& pos, int transition )
{
    return lookAt({pos.x,pos.y,pos.z + maxCameraDisp},transition);
}

Sequencer GameCamera::moveCamera(const Vector2& pos, int transition )
{
    return moveCamera({pos.x,pos.y,camera.position.z},transition);
}

Sequencer GameCamera::moveCamera(float z, int transition )
{
    return moveCamera(Vector3(camera.position.x,camera.position.y,z), transition);
}

Sequencer GameCamera::lookAt(std::function<Vector3()> func, int transition)
{

    if (transition > 0) //incrementally assign
    {
        return Sequencer(InitFunc([func,transition,this](const Vector3& cameraPos, int times){
            Vector3 endPos = func();
            float disp = maxCameraDisp*tan(camera.fovy/2*DEG2RAD); //distance from the edge of the screen

            //clamps camera to level area
            endPos  = {
                Clamp(endPos.x,disp,bounds.x - disp),
                Clamp(endPos.y,disp,bounds.y - disp),
                endPos.z
            };
            Vector3 pos = lerp(cameraPos,endPos,sin(static_cast<float>(times)/transition*M_PI/2));
            camera.target = pos;
            camera.position = camera.target - Vector3(0,0,maxCameraDisp);
            return times >= transition || Debug::isDebugOn(); 
        },
        [this](){return this->getCamera().target;}));
    }
    else
    {
        Vector3 endPos = func();
        float disp = maxCameraDisp*tan(camera.fovy/2*DEG2RAD); //distance from the edge of the screen

        //clamps camera to level area
        endPos  = {
            Clamp(endPos.x,disp,bounds.x - disp),
            Clamp(endPos.y,disp,bounds.y - disp),
            endPos.z
        };
        camera.target = endPos;
        camera.position = camera.target - Vector3(0,0,maxCameraDisp);
        return Sequencer();
    }  
}

Sequencer GameCamera::lookAt(float z, int transition )
{
    return lookAt({camera.target.x,camera.target.y,z},transition);
}

Sequencer GameCamera::lookAt(const Vector2& pos, int transition)
{
    return lookAt({pos.x,pos.y,camera.target.z},transition);
}

Sequencer GameCamera::lookAt(const Vector3& pos, int transition )
{
    return lookAt([pos](){return pos;},transition);
}

const Camera3D& GameCamera::getCamera()
{
    return camera;
}

