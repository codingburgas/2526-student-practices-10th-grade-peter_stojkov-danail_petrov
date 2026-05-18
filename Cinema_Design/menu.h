#ifndef MENU_H
#define MENU_H

#include "raylib.h"

enum class MenuScreen {
    MAIN_MENU = 0,
    TICKETS_PAGE = 1,
    EXIT_APP = -1
};

class CinemaMenu {
public:
    CinemaMenu(int screenWidth, int screenHeight);
    void Update(MenuScreen& currentScreen);
    void Draw();

private:
    Rectangle btnTickets;
    Rectangle btnMovies;
    Rectangle btnExit;
    bool hoverTickets;
    bool hoverMovies;
    bool hoverExit;
};

#endif
