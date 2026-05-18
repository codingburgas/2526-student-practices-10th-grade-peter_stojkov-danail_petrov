#ifndef APP_H
#define APP_H

#include "raylib.h"
#include "login.h"
#include "menu.h"
#include "../Cinema_Design/design.h"
#include "../Cinema_Design/tickets.h"

enum class AppScreen {
    LOGIN,
    MENU,
    TICKETS
};

class CinemaApp {
public:
    CinemaApp();
    ~CinemaApp();
    void Run();

private:
    const int screenWidth = 800;
    const int screenHeight = 600;

    AppScreen currentScreen;
    bool isRunning;

    LoginSystem loginSystem;
    CinemaMenu menu;
};

#endif
