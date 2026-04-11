#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

//UI Stuff

#include <functional>
#include <iostream>

#include <raylib.h>

//currently unused;
struct ButtonTheme
{
    Color background = WHITE;
    Color outline = BLACK;
};

//very hard coded button rn, not very flexible
class Button
{
    typedef std::function<void()> ClickFunc;
    Rectangle box;
    ClickFunc onClick;
    std::string message = "";
    ButtonTheme theme;
public:
    Button(const Vector2&, const Vector2&, ClickFunc,std::string_view);
    void render();
    void update();
};

class MainMenu
{
    Button play;
    Button quit;
public:
    MainMenu();
    void render();
    void update();
};

enum Menus
{
    NONE,
    MAIN_MENU
};

#endif // UI_H_INCLUDED
