#include "login.h"

LoginSystem::LoginSystem() {
    currentScreen = LoginState::SCREEN_MAIN;
    passwordInput = "";
    passwordWrong = false;
    userLoggedIn = false;
    shouldClose = false;

    btnAdmin = { 250, 200, 300, 50 };
    btnUser = { 250, 280, 300, 50 };
    btnExit = { 250, 360, 300, 50 };
    btnBack = { 20, 20, 100, 40 };

    btnSchedule = { 250, 200, 300, 50 };
    btnReserve = { 250, 280, 300, 50 };
    btnBuy = { 250, 360, 300, 50 };
}

bool LoginSystem::IsButtonClicked(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void LoginSystem::Update() {
    switch (currentScreen) {
    case LoginState::SCREEN_MAIN:
        if (IsButtonClicked(btnAdmin)) {
            currentScreen = LoginState::SCREEN_ADMIN_LOGIN;
            passwordInput = "";
            passwordWrong = false;
        }
        if (IsButtonClicked(btnUser)) {
            userLoggedIn = true;
        }
        if (IsButtonClicked(btnExit)) {
            shouldClose = true;
        }
        break;

    case LoginState::SCREEN_ADMIN_LOGIN:
        if (IsButtonClicked(btnBack)) currentScreen = LoginState::SCREEN_MAIN;

        int key;
        key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (passwordInput.length() < 15)) {
                passwordInput += (char)key;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !passwordInput.empty()) {
            passwordInput.pop_back();
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (passwordInput == "admin123") {
                currentScreen = LoginState::SCREEN_ADMIN_MENU;
            }
            else {
                passwordWrong = true;
                passwordInput = "";
            }
        }
        break;

    case LoginState::SCREEN_ADMIN_MENU:
        if (IsButtonClicked(btnBack)) currentScreen = LoginState::SCREEN_MAIN;
        break;

    case LoginState::SCREEN_USER_MENU:
        if (IsButtonClicked(btnBack)) currentScreen = LoginState::SCREEN_MAIN;
        break;
    }
}

void LoginSystem::Draw() {
    if (currentScreen != LoginState::SCREEN_MAIN) {
        DrawRectangleRec(btnBack, LIGHTGRAY);
        DrawText("Back", btnBack.x + 32, btnBack.y + 12, 16, BLACK);
    }

    switch (currentScreen) {
    case LoginState::SCREEN_MAIN:
        DrawText("WELCOME TO THE CINEMA", 220, 100, 30, MAROON);

        DrawRectangleRec(btnAdmin, DARKGRAY);
        DrawText("Login as Administrator", btnAdmin.x + 50, btnAdmin.y + 15, 18, WHITE);

        DrawRectangleRec(btnUser, DARKGRAY);
        DrawText("Login as User", btnUser.x + 90, btnUser.y + 15, 18, WHITE);

        DrawRectangleRec(btnExit, RED);
        DrawText("Exit", btnExit.x + 130, btnExit.y + 15, 18, WHITE);
        break;

    case LoginState::SCREEN_ADMIN_LOGIN:
        DrawText("ADMINISTRATOR LOGIN", 250, 150, 24, DARKGRAY);
        DrawText("Enter password and press Enter:", 255, 220, 18, GRAY);

        DrawRectangle(250, 270, 300, 40, LIGHTGRAY);
        DrawRectangleLines(250, 270, 300, 40, DARKGRAY);
        DrawText(std::string(passwordInput.length(), '*').c_str(), 260, 280, 20, BLACK);

        if (passwordWrong) {
            DrawText("Incorrect password!", 315, 330, 18, RED);
        }
        break;

    case LoginState::SCREEN_ADMIN_MENU:
        DrawText("ADMINISTRATOR PANEL", 240, 100, 26, MAROON);
        DrawText("1. Add Movie", 250, 200, 20, BLACK);
        DrawText("2. Edit Schedule", 250, 250, 20, BLACK);
        DrawText("3. View Reserved Seats", 250, 300, 20, BLACK);
        break;

    case LoginState::SCREEN_USER_MENU:
        DrawText("USER MENU", 330, 100, 26, MAROON);

        DrawRectangleRec(btnSchedule, LIME);
        DrawText("1. View Movie Schedule", btnSchedule.x + 45, btnSchedule.y + 15, 18, BLACK);

        DrawRectangleRec(btnReserve, LIGHTGRAY);
        DrawText("2. Reserve a Seat", btnReserve.x + 70, btnReserve.y + 15, 18, DARKGRAY);

        DrawRectangleRec(btnBuy, LIGHTGRAY);
        DrawText("3. Buy a Ticket", btnBuy.x + 80, btnBuy.y + 15, 18, DARKGRAY);
        break;
    }
}
