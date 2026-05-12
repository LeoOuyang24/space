#include "../headers/resources_math.h"
#include "../headers/camera.h"
#include "../headers/sequencer.h"
#include "../headers/game.h"
#include "../headers/objects.h"

void GameCamera::init()
{
    
    camera.projection = CAMERA_PERSPECTIVE;
   // camera.position = Vector3(0,0,Globals::Game.getCurrentZ() - Globals::CAMERA_Z_DISP);
    camera.target = Vector3(0,0,Globals::BACKGROUND_Z);
    //camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    //camera.rotation = 0.0f;
    //camera.zoom = 1.0f;
    camera.up = {0,-1,0};
    camera.fovy = 90;

    seq.reset(new Sequencer());
    Sequences::add(seq,false);
}

void GameCamera::update()
{
    if (cameraFollow && Globals::Game.getPlayer() && !Debug::isDebugOn() && seq->size() == 0)
    {
        lookAt(toVector3(Globals::Game.getPlayer()->getPos()),0);
    }
}

void GameCamera::setCameraFollow(bool val, int transition )
{
    if (queueing)
    {
        queueUp(std::bind(static_cast<void(GameCamera::*)(bool,int)>(&setCameraFollow),this,val,transition));
    }
    else if (!lock)
    {
        cameraFollow = val;

        if (val)
        {
            if (transition)
            {
                seq->push_front(RunThis([transition,this,startPos=camera.position](int times){
                    Vector3 endPos = toVector3(Globals::Game.getPlayer()->getPos());
                    Vector3 pos = lerp(startPos,{endPos.x,endPos.y,endPos.z - Globals::CAMERA_Z_DISP},sin(static_cast<float>(times)/transition*M_PI/2));
                    camera.position = pos;
                    camera.target = pos;
                    camera.target.z = pos.z + Globals::CAMERA_Z_DISP;
                    return times >= transition || Debug::isDebugOn(); 
                    }));
            }
        }
        else
        {
            moveCamera(cameraFollowPoint,transition);
        }

    }

}

void GameCamera::setCameraFollow(const Vector3& pos, int transition )
{
    if (!lock)
    {
        cameraFollowPoint = pos; //allowed to happen outside of queue, since it doesn't really change anything until we actually run things
        setCameraFollow(false,transition);
    }
}

void GameCamera::setCameraFollow(const Vector2& point, int transition)
{
    setCameraFollow({point.x,point.y,camera.position.z},transition);
}

bool GameCamera::getCameraFollow()
{
    return cameraFollow;
}

void GameCamera::moveCamera(const Vector3& pos, int transition )
{
    if (queueing)
    {
        queueUp(std::bind(static_cast<void(GameCamera::*)(const Vector3&,int)>(&moveCamera),this,pos,transition));
        return;
    }
    else if (!lock)
    {
        float disp = Globals::CAMERA_Z_DISP*tan(camera.fovy/2*DEG2RAD); //distance from the edge of the screen

        //clamps camera to level area
        Vector2 clampedPos = {
            Clamp(pos.x,disp,Terrain::MAX_TERRAIN_SIZE - disp),
            Clamp(pos.y,disp,Terrain::MAX_TERRAIN_SIZE - disp)
        };


        if (transition > 0 && seq) //incrementally assign
        {
            seq->push_front(RunThis([disp,transition,this,startPos=camera.position,endPos=Vector3(clampedPos.x,clampedPos.y,pos.z)](int times){
                Vector3 pos = lerp(startPos,endPos,sin(static_cast<float>(times)/transition*M_PI/2));
                camera.position = pos;
                camera.target = pos;
                camera.target.z = pos.z + Globals::CAMERA_Z_DISP;
                return times >= transition || Debug::isDebugOn(); 
            }));

        }
        else
        {
            assignVector(camera.position,clampedPos);
            assignVector(camera.target,clampedPos);
            camera.position.z = pos.z ;
            camera.target.z = pos.z + Globals::CAMERA_Z_DISP;
        }
    }
}

void GameCamera::moveCamera(const Vector2& pos, int transition )
{
    moveCamera({pos.x,pos.y,camera.position.z},transition);
}

void GameCamera::moveCamera(float z, int transition )
{
    moveCamera(Vector3(camera.position.x,camera.position.y,z), transition);
}

void GameCamera::lookAt(float z, int transition )
{
    moveCamera(z - Globals::CAMERA_Z_DISP, transition);
}

void GameCamera::lookAt(const Vector3& pos, int transition )
{
    moveCamera({pos.x,pos.y,pos.z - Globals::CAMERA_Z_DISP},transition);
}

void GameCamera::startQueue()
{
    if (!lock)
    {
        queueing = true;
    }
}

void GameCamera::stopQueue()
{
    if (!lock)
    {
        lock = true;
        queueing = false;
        if (seq.get())
        {
            seq->push_back(RunThis([this](int){ lock = false; return true;}));
        }
    }
}

void GameCamera::clear()
{
    if (seq)
    {
        lock = false;
        seq->clear();
    }
}

bool GameCamera::isDone()
{
    return seq->size() == 0;
}

const Camera3D& GameCamera::getCamera()
{
    return camera;
}

