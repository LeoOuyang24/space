#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <iostream>
#include <memory>
#include <functional>

#include "UI.h"

class Sign;

enum Menus
{
    NONE, //none, the game is running
    MAIN_MENU,
    WORLD_MAP,
    MENU_AMOUNT //amount of menus
};

class Interface
{

    //is set to true when signMessage is first set, which basically just means that the sign UI can't go down the same frame its activated
    bool justSet = false;
    std::weak_ptr<Sign> sign;
    size_t messageIndex = 0;

    Menus currentMenu = Menus::MAIN_MENU;
    MainMenu menu;

    std::array<std::unique_ptr<UIElement>,static_cast<size_t>(Menus::MENU_AMOUNT)-1> menus;

    void processSign();//handle rendering sign message
public:
    void init();
    void setMessage(Sign* message);
    void setMenu(Menus menu);
    void process();
};

#endif // INTERFACE_H_INCLUDED
