#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

//UI Stuff

#include <functional>
#include <iostream>

#include <raylib.h>

#include "camera.h"
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
        bool isHovered(const Rectangle&); //true if mouse is hovering over    
        bool isClicked(const Rectangle&); //true if mouse is hovering over AND left mouse button is clicked

        WorldMap* parent = nullptr;
        Vector2 center;
        CurrentWorld world = 0;
        Color color;
    };
    std::array<WorldNode,2> nodes;
public:

    static constexpr float BG_Z = 0; //z at which the background should be rendered at
    static constexpr float NODE_Z = -10; //z at which the nodes should be rendered at
    static constexpr float CAMERA_Z = -500; //z at which the camera is at

    RenderTexture bg;
    GameCamera camera;

    WorldMap();
    void render(const Rectangle&);
    void update(const Rectangle&);
    void process(const Rectangle&);
};

#endif // UI_H_INCLUDED
