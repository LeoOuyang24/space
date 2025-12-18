#include <cstring>
#include <algorithm>

#include "../headers/conversions.h"

#include "../headers/portal.h"

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
    if (nums[nums.size() -1] != str.size()) [[likely]] //false if the string ends with a delimit, in which case we can skip this
    {
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
Texture2D* fromString(std::string_view str)
{
    Texture2D* sprite = Globals::Game.Sprites.getSprite(str.data());
    if (!sprite)
    {
        std::cerr << "fromString: failed to fetch sprite: " << str << "\n";
        exit(1);
    }
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
std::string toString(Texture2D*& sprite)
{
    return Globals::Game.Sprites.getSpritePath(sprite);
}

std::string toString(PortalCondition* cond)
{
    if (cond)
    {
        return cond->toString();
    }
    return "null";
}

template<>
std::string toString(std::unique_ptr<PortalCondition>& ptr)
{
    return toString(ptr.get());
}

template<>
std::string toString(std::string& str)
{
    return str;
}
template<>
std::string toString(std::vector<std::string>& vec)
{
    std::string answer = vec.size() > 0 ? vec[0] : ""; //allows us to prepend the | delimitter, which prevents trailing delimitters
    std::for_each(vec.begin() + 1,vec.end(),[&answer](const std::string& str){
                  answer += '|' + str;

                  });


    return answer;
}
