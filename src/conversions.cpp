#include "../headers/conversions.h"

std::vector<std::string> split(std::string str, char delimit)
{
    std::vector<std::string> vec;
    size_t start = 0;
    size_t ind = 0;
    while ( (ind = str.find(delimit,start)) != std::string::npos)
    {
        vec.push_back(str.substr(start,ind - start));
        start = ind + 1;
    }
    vec.push_back(str.substr(start,ind));
    return vec;
}

template<>
bool fromString(std::string str)
{
    return str == "1";
}

template<>
float fromString(std::string str)
{
    return std::stof(str);
}

template<>
int fromString(std::string str)
{
    return std::stoi(str);
}

template<>
size_t fromString(std::string str)
{
    return fromString<int>(str);
}

template<>
Vector2 fromString(std::string str)
{
    auto nums = split(str);
    return
    {
        fromString<float>(nums[0]),
        nums.size() > 0 ? fromString<float>(nums[1]) : 0
    };
}

template<>
Vector3 fromString(std::string str)
{
    auto nums = split(str);
    return
    {
        fromString<int>(nums[0]),
        nums.size() > 1 ? fromString<int>(nums[1]) : 0,
        nums.size() > 2 ? fromString<int>(nums[2]) : 0
    };
}

template<>
Color fromString(std::string str)
{
    auto nums = split(str);
    return {
        fromString<int>(nums[0]),
        nums.size() > 1 ? fromString<int>(nums[1]) : 0,
        nums.size() > 2 ? fromString<int>(nums[2]) : 0,
        nums.size() > 3 ? fromString<int>(nums[3]) : 0
    };
}

template<>
Texture2D* fromString(std::string str)
{
    Texture2D* sprite = Globals::Game.Sprites.getSprite(str);
    if (!sprite)
    {
        std::cerr << "fromString: failed to fetch sprite: " << str << "\n";
        exit(1);
    }
    return sprite;
}




template<>
std::string toString(Vector2 vec)
{
    return toString(vec.x) + "," + toString(vec.y);
}

template<>
std::string toString(Vector3 vec)
{
    return toString(vec.x) + "," + toString(vec.y) + "," + toString(vec.z);
}

template<>
std::string toString(Color col)
{
    return toString(col.r) + "," + toString(col.g) + "," + toString(col.b) + "," + toString(col.a);
}

template<>
std::string toString(Texture2D* sprite)
{
    return Globals::Game.Sprites.getSpritePath(sprite);
}
