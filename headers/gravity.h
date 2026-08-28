#ifndef GRAVITY_H_INCLUDED
#define GRAVITY_H_INCLUDED

#include <vector>

#include <raylib.h>
#include <raymath.h>

struct GravityField
{

    static constexpr size_t SCALE_FACTOR = 10;
    static constexpr int WIDTH = 3000/SCALE_FACTOR; //entries per width
    static constexpr int FIELD_WIDTH = 3*SCALE_FACTOR;

    GravityField(int gravRad);

    /**
     * @brief Given a block position, updates the corresponding gravity field and all surrounding gravity fields
     * 
     * @param pos 
     */
    void addBlock(const Vector2& pos);

    void debugRender();

private:

    //represents an average amount of gravitational force. "totalPos"/total is the gravitational field excerted by this spot
    struct GravField
    {
        Vector2 totalDir;
        size_t total = 0;

        GravField operator+(const Vector2& pos)
        {
            totalDir += pos;
            total += 1;

            return *this;
        }

        void operator+=(const Vector2& pos)
        {
            *this = *this + pos;
        }
    };

    int gravityRadius = 0; //max number of blocks away a field can be and still be updated by a block addition
    std::vector<GravField> fields = std::vector<GravField>(WIDTH*WIDTH,GravField{});
};

#endif