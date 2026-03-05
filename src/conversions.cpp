#include <cstring>
#include <algorithm>

#include "../headers/conversions.h"
#include "../headers/enemy.h"
#include "../headers/portal.h"
#include "../headers/blocks.h"

std::string_view SplitString::operator[](size_t index) const
{
    return index < nums.size() - 1 ? str.substr(nums[index], (nums[index + 1] - nums[index] - 1)) : "";
}

size_t SplitString::size() const
{
    return nums.size() - 1;
}

//splits a string
SplitString::SplitString(std::string_view str, char delimit)
{
    size_t start = 0;
    size_t ind = 0;
    nums = {0};
    while ( (ind = str.find(delimit,start)) != std::string::npos)
    {
        nums.push_back(ind + 1);
        start = ind + 1;
    }
    if (nums[nums.size()-1] != str.size()) //true if the string doesn't end with a delimit, in which case we simply add position after the end of the string (+1 because normally the next index is the position of the next string)
    {                                           //if this is false, that means the string DOES end with a delimit, in which case the last position was already added in the while loop
        nums.push_back(str.size() + 1);
    }
    this->str = str;
}

template<>
bool fromString(std::string_view str)
{
    return str == "1";
}

template<>
float fromString(std::string_view str)
{
    return str.size() > 0 ? std::atof(str.data()) : 0;
}

template<>
int fromString(std::string_view str)
{
    return str.size() > 0 ? std::atoi(str.data()) : 0;
}

template<>
size_t fromString(std::string_view str)
{
    return str.size() > 0 ? fromString<int>(str) : 0;
}

template<>
double fromString(std::string_view str)
{
    return str.size() > 0 ? std::stod(str.data()) : 0;
}

template<>
Vector2 fromString(std::string_view str)
{
    auto nums = SplitString(str);
    return
    {
        fromString<float>(nums[0]),
        fromString<float>(nums[1])
    };
}

template<>
Vector3 fromString(std::string_view str)
{
    auto nums = SplitString(str);
    return
    {
        fromString<int>(nums[0]),
        fromString<int>(nums[1]),
        fromString<int>(nums[2])
    };
}

template<>
Color fromString(std::string_view str)
{
    auto nums = SplitString(str);
    return {
        fromString<int>(nums[0]),
        fromString<int>(nums[1]),
        fromString<int>(nums[2]),
        fromString<int>(nums[3])
    };
}

template<>
Texture2D fromString(std::string_view str)
{
    Texture2D sprite = Globals::Game.Sprites.getSprite(str.data());
    return sprite;
}

template<>
PortalCondition* fromString(std::string_view str)
{
    SplitString split(str);
    if (split[0] == "gear")
    {
        return new TokenLocked(fromString<size_t>(split[1]));
    }
    return nullptr;
}

template<>
OnTrigger* fromString(std::string_view str)
{
    SplitString split(str,'|');

    if (split[0] == "trigger_spawn_planets")
    {
        return new TriggerSpawnPlanets(split);
    }
    return nullptr;
}

template<>
std::string fromString(std::string_view str)
{
    std::string answer = "";
    for (int i = 0; i < str.size(); i ++)
    {
        if (str[i] == '\\' && i < str.size() - 1)
        {
            switch (str[i+1])
            {
            case 'n':
                answer += '\n';
                break;
            case 't':
                answer += '\t';
                break;
            default:
                answer += '\\';
                i--;
            }
            i ++;
        }
        else
        {
            answer += str[i];
        }
    }
    return answer;

}

template<>
std::vector<std::string> fromString(std::string_view str)
{
    SplitString split(str,'|');
    std::vector<std::string> answer;
    split.forEach([&answer](const std::string_view& view){
                  answer.emplace_back(fromString<std::string>(view));
                  });
    return answer;
}

template<>
LaserBeamEnemy::RotateFunc fromString(std::string_view str)
{
    if (strncmp(str.data(),"SINE",strlen("SINE")) == 0)
    {
        return LaserBeamEnemy::RotateFunc::SINE;
    }
    return LaserBeamEnemy::RotateFunc::CONSTANT;

}

template<>
std::string toString(BlockType& type) //this has to be declared before from string since we use it there
{
    switch (type)
    {
    case AIR:
        return "AIR";
    case SOLID:
        return "SOLID";
    case LAVA:
        return "LAVA";
    case ANTI:
        return "ANTI";
    }
    return "AIR";
}
template<>
BlockType fromString(std::string_view str)
{
    //iterate through each possible block type and see which string matches
    //It's a bit of a weird implementation, but it does mean that we don't have to define the string variants twice
    for (BlockType type = AIR; type < BLOCK_TYPES; type=static_cast<BlockType>(static_cast<uint8_t>(type) + 1))
    {
        if (toString(type) == str)
        {
            return type;
        }
    }
    return AIR;
}

template<>
MovingTerrain::MoveFunc fromString(std::string_view str)
{
    //basically return two functions, one is how we move, the other is how to serialize
    SplitString split(str,'|');
    auto toString = [cereal=std::string(str.data())]() -> std::string {return cereal;}; //toString function each of these guys will have, which is just returning the string that was used to deserialize them
    if (split[0] == "LINE") //LINE|<distance>|<angle in degrees>
    {
        return { //linear movement, can be diagonal based on 3rd parameter
            [distance = fromString<float>(split[1]),
            degrees = fromString<float>(split[2])]
                (const Orient& o,const Vector2& starting,uint16_t frame,float speed) -> Vector2
                    {
                        if (distance > 0)
                            return starting + Vector2(cos(degrees*DEG2RAD),sin(degrees*DEG2RAD))*sin(frame*speed/10.0f/distance)*distance;
                        return starting;
                    },
            toString
        };
    }
    else if (split[0] == "CIRCLE") //CIRCLE|<radius>
    {
        return {
            [radius=fromString<float>(split[1])](const Orient& o,const Vector2& starting,uint16_t frame,float speed){
                float angle = frame/1000.0f*speed;
                return starting + Vector2(cos(angle),sin(angle))*radius;
            },
            toString
        };
    }
    return {};
}

template<>
std::string toString(Vector2& vec)
{
    return toString(vec.x) + "," + toString(vec.y);
}

template<>
std::string toString(Vector3& vec)
{
    return toString(vec.x) + "," + toString(vec.y) + "," + toString(vec.z);
}

template<>
std::string toString(Color& col)
{
    return toString(col.r) + "," + toString(col.g) + "," + toString(col.b) + "," + toString(col.a);
}

template<>
std::string toString(Texture2D& sprite)
{
    return Globals::Game.Sprites.getSpritePath(sprite);
}

template<>
std::string toString(PortalCondition* cond)
{
    if (cond)
    {
        return cond->toString();
    }
    return "null";
}

template<>
std::string toString(OnTrigger* trigger)
{
    if (trigger)
    {
        return trigger->to_string();
    }
    return "null";
}

template<typename T>
std::string toString(T* ptr);


template<>
std::string toString(std::string& str)
{
    return str;
}
template<>
std::string toString(std::vector<std::string>& vec)
{
    std::string answer = vec.size() > 0 ? vec[0] : ""; //adding the first element allows us to prepend the | delimitter, which prevents trailing delimitters
    if (vec.size() > 0)
    {
           std::for_each(vec.begin() + 1,vec.end(),[&answer](const std::string& str){
                  answer += '|' + str;

                  });
    }



    return answer;
}

template<>
std::string toString(LaserBeamEnemy::RotateFunc& func)
{
    switch (func)
    {
        case LaserBeamEnemy::RotateFunc::CONSTANT:
            return "CONSTANT";
        case LaserBeamEnemy::RotateFunc::SINE:
            return "SINE";
        default:
            return "CONSTANT";
    }
}

template<>
std::string toString(MovingTerrain::MoveFunc& moveFunc)
{
    if (moveFunc.toString)
        return moveFunc.toString();
    return "null";
}

