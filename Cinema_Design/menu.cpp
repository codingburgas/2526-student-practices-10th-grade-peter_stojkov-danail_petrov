#include "menu.h"

CinemaMenu::CinemaMenu(int screenWidth, int screenHeight) {
    btnTickets = { (float)screenWidth / 2 - 100, 250, 200, 50 };
    btnMovies = { (float)screenWidth / 2 - 100, 320, 200, 50 };
    btnExit = { (float)screenWidth / 2 - 100, 390, 200, 50 };
    hoverTickets = false;
    hoverMovies = false;
    hoverExit = false;
}

void CinemaMenu::Update(MenuScreen& currentScreen) {
    Vector2 mouse = GetMousePosition();

    hoverTickets = CheckCollisionPointRec(mouse, btnTickets);
    hoverMovies = CheckCollisionPointRec(mouse, btnMovies);
    hoverExit = CheckCollisionPointRec(mouse, btnExit);

    if (hoverTickets || hoverMovies || hoverExit)
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    else
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    if (hoverTickets && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentScreen = MenuScreen::TICKETS_PAGE;
    }

    if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentScreen = MenuScreen::EXIT_APP;
    }
}

void CinemaMenu::Draw() {
    int screenWidth = GetScreenWidth();
    DrawText("CINEMA MENU", screenWidth / 2 - MeasureText("CINEMA MENU", 40) / 2, 120, 40, MAROON);

    DrawRectangleRec(btnTickets, hoverTickets ? LIGHTGRAY : GRAY);
    DrawText("TICKETS", btnTickets.x + 55, btnTickets.y + 15, 20, WHITE);

    DrawRectangleRec(btnMovies, hoverMovies ? LIGHTGRAY : GRAY);
    DrawText("MOVIES", btnMovies.x + 60, btnMovies.y + 15, 20, WHITE);

    DrawRectangleRec(btnExit, hoverExit ? RED : MAROON);
    DrawText("EXIT", btnExit.x + 75, btnExit.y + 15, 20, WHITE);
}
