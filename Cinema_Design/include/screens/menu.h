#pragma once
#include "raylib.h"

enum class MenuResult {
    NONE,
    OPEN_MOVIES,
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
    Rectangle btnExit;

    bool hoverMovies;
    bool hoverExit;
};
