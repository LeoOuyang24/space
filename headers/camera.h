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
    void setCameraFollow(bool val,int transition = 0); //toggle camera follow on/off
    /**
     * @brief set "cameraFollow" to false and set the new point to look at
     * 
     * @param point point to look at
     * @param transition how many frames you want the transition to be (0 for no transition)
     */
    void setCameraFollow(const Vector3& point, int transition = 0); //turn camera follow off, and have it follow the provided point, "transition" == true if we want a transiiton
    void setCameraFollow(const Vector2& point, int transition = 0); 
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
    void moveCamera(const Vector3& pos, int transition = 0);
    void moveCamera(const Vector2& pos, int transition = 0);
    void moveCamera(float z, int transition = 0); //set position to z

    /**
     * @brief sets the camera's TARGET to the provided position
     * 
     * @param pos, new target position
     * @param transition how many frames you want the transition to be (0 for no transition)
     */
    void lookAt(const Vector3& pos, int transition = 0);
    void lookAt(float z, int transition = 0); //set target to z

    /**
     * @brief Starts queuing camera changes. All camera state changes from here on out will be queued and played in a Sequence. "queuing" is immediately set to true. This is the only function that sets "queuing" to true
     * 
     */
    void startQueue();
    /**
     * @brief End queuing camera changes. All camera state changes from here on out will be IGNORED until the queued functions finish. "lock" is immediately set to true, "queuing" is immediately set to false. At the very end of the queued actions, lock = "false".
     * 
     */
    void stopQueue();

    /**
     * @brief Clear all queued up camera moves. Unlocks as well
     * 
     */
    void clear();

    /**
     * @brief returns true if camera no longer has any queued up movements (seq is empty)
     * 
     */
     bool isDone();


    /**
     * @brief Get the Camera object
     * 
     * @return const Camera3D& 
     */
    const Camera3D& getCamera();
    SequencePtr seq; //the sequence to add camera transitions to

private:
    /**
     * @brief Queues up a function call
     * 
     * @return const Camera3D& 
     */
    template<typename T>
    void queueUp(T func)
    {
        seq->push_back(RunThis([func,this](int){
            lock = false;
            func();
            lock = true;
            return true;
        }));
    }
    Vector2 bounds = {}; //point between 0,0 and "bounds" that the camera can not leave
    float maxCameraDisp = 0; //maximum distance from the background a camera can have

    Camera3D camera;
    bool cameraFollow = false;
    Vector3 cameraFollowPoint = {};
    bool lock = false; //true if all camera state modifications will be ignored
    bool queueing = false; //true if we are queueing actions
};

#endif // CAMERA_H