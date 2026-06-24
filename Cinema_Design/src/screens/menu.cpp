#include "menu.h"

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TEXT_WHITE     = { 248, 250, 252, 255 };
static Color TEXT_DIM      = { 148, 163, 184, 255 };
static Color CYAN_ACC      = { 0, 240, 255, 255 };
static Color PINK_ACC      = { 255, 42, 109, 255 };
static Color PINK_HOVER    = { 255, 80, 130, 255 };

CinemaMenu::CinemaMenu(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    float cx = w / 2.0f;
    btnMovies  = { cx - 130, 240, 260, 55 };
    btnExit    = { cx - 130, 320, 260, 55 };

    hoverMovies = false;
    hoverExit = false;
}

void CinemaMenu::Update(MenuResult& result) {
    result = MenuResult::NONE;

    Vector2 mouse = GetMousePosition();

    hoverMovies = CheckCollisionPointRec(mouse, btnMovies);
    hoverExit = CheckCollisionPointRec(mouse, btnExit);

    if (hoverMovies && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        result = MenuResult::OPEN_MOVIES;

    if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        result = MenuResult::EXIT_APP;
}

void CinemaMenu::Draw() {
    int cx = screenWidth / 2;

    const char* title = "CINEMA";
    int tw = MeasureText(title, 50);
    DrawText(title, cx - tw / 2 + 2, 82, 50, Color{0, 0, 0, 80});
    DrawText(title, cx - tw / 2, 80, 50, CYAN_ACC);

    const char* sub = "DASHBOARD";
    int sw = MeasureText(sub, 20);
    DrawText(sub, cx - sw / 2, 140, 20, TEXT_DIM);

    DrawLine(cx - 100, 175, cx + 100, 175, BORDER_COLOR);

    DrawRectangleRounded(btnMovies, 0.4f, 8, hoverMovies ? PINK_HOVER : PINK_ACC);
    const char* moviesText = "BROWSE MOVIES";
    int mw = MeasureText(moviesText, 22);
    DrawText(moviesText, (int)(btnMovies.x + (btnMovies.width - mw) / 2), (int)(btnMovies.y + 17), 22, DARK_BG);

    DrawRectangleRounded(btnExit, 0.4f, 8, hoverExit ? Color{80, 30, 30, 255} : CARD_COLOR);
    DrawRectangleRoundedLines(btnExit, 0.4f, 8, hoverExit ? Color{255, 100, 100, 255} : BORDER_COLOR);
    const char* exitText = "EXIT";
    int ew = MeasureText(exitText, 22);
    DrawText(exitText, (int)(btnExit.x + (btnExit.width - ew) / 2), (int)(btnExit.y + 17), 22, TEXT_WHITE);
}
