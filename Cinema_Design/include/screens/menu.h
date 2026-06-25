#pragma once
#include "raylib.h"

enum class MenuResult {
    NONE,
    OPEN_MOVIES,
    OPEN_MY_BOOKINGS,
    LOG_OUT,
    EXIT_APP
};

class CinemaMenu {
public:
    CinemaMenu(int screenWidth, int screenHeight);
    void Update(MenuResult& result);
    void Draw();

private:
    int screenWidth;
    int screenHeight;

    Rectangle btnMovies;
    Rectangle btnMyBookings;
    Rectangle btnExit;
    Rectangle btnTheme;
    Rectangle btnLanguage;
    Rectangle btnLogout;

    bool hoverMovies;
    bool hoverMyBookings;
    bool hoverExit;
    bool hoverTheme;
    bool hoverLanguage;
    bool hoverLogout;
};
