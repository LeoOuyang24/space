#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

//UI Stuff

#include <functional>
#include <iostream>

#include <raylib.h>

#include "terrain.h"

//abstract class for any UI Element
struct UIElement
{
    /**
     * @brief render onto provided rectangle
     * 
     */
    virtual void render (const Rectangle&)
    {

    }
    /**
     * @brief Optional update function to handle logic like button presses
     * 
     */
    virtual void update(const Rectangle&) 
    {

    }
    /**
     * @brief Passes the render center and dimensions to render and update. Dimensions will likely be at least "getMinDimens()"
     * 
     */
    virtual void process(const Rectangle& rect)
    {
        update(rect);
        render(rect);
    }

    /**
     * @brief Returns the minimum size this element can be
     * 
     * @return Vector2 
     */
    virtual Vector2 getMinDimens() 
    {
        return {0,0};
    }
};


//currently unused;
struct ButtonTheme
{
    Color background = WHITE;
    Color outline = BLACK;
};

//very hard coded button rn, not very flexible
class Button : public UIElement
{
    typedef std::function<void()> ClickFunc;
    Vector2 minDimens = {};
    ClickFunc onClick;
    std::string message = "";
    ButtonTheme theme;
public:
    Button(const Vector2& ,ClickFunc,std::string_view);
    void render(const Rectangle&);
    Vector2 getMinDimens();
    void update(const Rectangle&);
};

class MainMenu : public UIElement
{
    Button play;
    Button quit;
public:
    MainMenu();
    void process(const Rectangle& rect);
};

class WorldMap : public UIElement
{
    struct WorldNode : public UIElement
    {
        WorldNode(const Vector2& pos, CurrentWorld world);
        WorldNode(){};
        void render(const Rectangle&);
        void update(const Rectangle&);
    public:
        Vector2 center;
        CurrentWorld world = 0;
    };
    std::array<WorldNode,5> nodes;
public:
    RenderTexture bg;

    WorldMap();
    void render(const Rectangle&);
    void update(const Rectangle&);
    void process(const Rectangle&);
};

#endif // UI_H_INCLUDED
