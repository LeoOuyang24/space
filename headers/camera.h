#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include "raylib.h"
#include "sequencer.h"

struct GameCamera
{
    void init(const Vector2& bounds_, float maxCameraDisp_);
    void update();
    /**
     * @brief set "cameraFollow" to a boolean value
     * 
     * @param val value to set to
     * @param transition how many frames you want the transition to be (0 for no transition)
     */
    Sequencer setCameraFollow(bool val,int transition = 0); //toggle camera follow on/off
    /**
     * @brief set "cameraFollow" to false and set the new point to look at
     * 
     * @param point point to look at
     * @param transition how many frames you want the transition to be (0 for no transition)
     */
    Sequencer setCameraFollow(const Vector3& point, int transition = 0); //turn camera follow off, and have it follow the provided point, "transition" == true if we want a transiiton
    Sequencer setCameraFollow(const Vector2& point, int transition = 0); 
    /**
     * @brief Returns cameraFollow
     * 
     * @return true 
     * @return false 
     */
    bool getCameraFollow();

    /**
     * @brief moves camera to the provided position
     * 
     * @param pos position to move to
     * @param transition how many frames you want the transition to be (0 for no transition)
     */
    Sequencer moveCamera(const Vector3& pos, int transition = 0);
    Sequencer moveCamera(const Vector2& pos, int transition = 0);
    Sequencer moveCamera(float z, int transition = 0); //set position to z

    /**
     * @brief sets the camera's TARGET to the provided position
     * 
     * @param pos, new target position
     * @param transition how many seconds you want the transition to be (0 for no transition)
     */
    Sequencer lookAt(std::function<Vector3()> func, int transition);
    Sequencer lookAt(const Vector3& pos, int transition = 0);
    Sequencer lookAt(const Vector2& pos, int transition);
    Sequencer lookAt(float z, int transition = 0); //set target to z

    /**
     * @brief Get the Camera object
     * 
     * @return const Camera3D& 
     */
    const Camera3D& getCamera();

private:
    Vector2 bounds = {}; //point between 0,0 and "bounds" that the camera can not leave
    float maxCameraDisp = 0; //maximum distance from the background a camera can have

    Camera3D camera;
    bool cameraFollow = false;
    //point to LOOK AT when camera follow is false.
    Vector3 cameraFollowPoint = {};
};

#endif // CAMERA_H