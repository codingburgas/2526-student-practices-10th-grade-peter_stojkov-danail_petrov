#include "app.h"

CinemaApp::CinemaApp() : menu(screenWidth, screenHeight) {
    InitWindow(screenWidth, screenHeight, "Cinema Booking Application");
    SetTargetFPS(60);
    currentScreen = AppScreen::MENU;
    isRunning = true;
}

CinemaApp::~CinemaApp() {
    CloseWindow();
}

void CinemaApp::Run() {
    while (isRunning && !WindowShouldClose()) {

        if (currentScreen == AppScreen::LOGIN) {
            loginSystem.Update();

            if (WindowShouldClose()) {
                isRunning = false;
                continue;
            }
        }
        else if (currentScreen == AppScreen::MENU) {
            MenuScreen menuChoice = MenuScreen::MAIN_MENU;
            menu.Update(menuChoice);

            if (menuChoice == MenuScreen::TICKETS_PAGE) {
                currentScreen = AppScreen::TICKETS;
            }
            else if (menuChoice == MenuScreen::EXIT_APP) {
                isRunning = false;
                continue;
            }
        }
        else if (currentScreen == AppScreen::TICKETS) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                currentScreen = AppScreen::MENU;
            }
        }

        if (isRunning) {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScreen == AppScreen::LOGIN) {
                loginSystem.Draw();
            }
            else if (currentScreen == AppScreen::MENU) {
                menu.Draw();
            }
            else if (currentScreen == AppScreen::TICKETS) {
                DrawTicketsPage();
            }

            EndDrawing();
        }
    }
}
