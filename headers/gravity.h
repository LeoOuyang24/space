#ifndef GRAVITY_H_INCLUDED
#define GRAVITY_H_INCLUDED

#include <vector>

#include <raylib.h>
#include <raymath.h>

struct GravityField
{

    GravityField( size_t width_, size_t fieldWidth_, int gravityRadius_);

    /**
     * @brief Given a block position, updates the corresponding gravity field and all surrounding gravity fields
     * 
     * @param pos
     * @param remove true if you want to remove a block instead (the logic is the exact same so I didn't bother making a new function) 
     */
    void addBlock(const Vector2& pos, bool remove = false, float magnitude = 1.0f);


    /**
     * @brief Returns the gravity field at the given position
     * 
     * @param pos 
     * @return Vector2 
     */
    Vector2 getFieldAtPos(const Vector2& pos);

    void debugRender();

private:

    //represents an average amount of gravitational force. "totalPos"/total is the gravitational field excerted by this spot
    struct GravField
    {
        Vector2 totalDir;
        size_t total = 0;
        float distance = 10000;

        GravField operator+(const Vector2& pos);

        GravField operator-(const Vector2& pos);

        void operator+=(const Vector2& pos);

        void operator-=(const Vector2& pos);
    };

    const int gravityRadius; //max number of blocks away a field can be and still be updated by a block addition, is an int to prevent some overflow issues
    const size_t width; //the number of fields in a row
    const size_t fieldWidth;//the width of each gravityField
    std::vector<GravField> fields;
};

#endif