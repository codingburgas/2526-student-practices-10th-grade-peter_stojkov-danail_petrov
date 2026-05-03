#include "raylib.h"
#include "../Cinema_Design/design.h"
#include "../Cinema_Design/tickets.h"

enum Screen {
    MENU = 0,
    TICKETS = 1
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Cinema Menu Test");

    int currentScreen = MENU;

    Rectangle btnTickets = { screenWidth / 2 - 100, 250, 200, 50 };
    Rectangle btnMovies = { screenWidth / 2 - 100, 320, 200, 50 };
    Rectangle btnExit = { screenWidth / 2 - 100, 390, 200, 50 };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        bool hoverTickets = CheckCollisionPointRec(mouse, btnTickets);
        bool hoverMovies = CheckCollisionPointRec(mouse, btnMovies);
        bool hoverExit = CheckCollisionPointRec(mouse, btnExit);

        if (hoverTickets || hoverMovies || hoverExit)
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        else
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (hoverTickets && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = TICKETS;
        }

        if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScreen == TICKETS) {
            DrawTicketsPage();

            if (IsKeyPressed(KEY_BACKSPACE)) {
                currentScreen = MENU;
            }

            EndDrawing();
            continue;
        }

        DrawText("CINEMA MENU", screenWidth / 2 - MeasureText("CINEMA MENU", 40) / 2, 120, 40, MAROON);

        DrawRectangleRec(btnTickets, hoverTickets ? LIGHTGRAY : GRAY);
        DrawText("TICKETS", btnTickets.x + 55, btnTickets.y + 15, 20, WHITE);

        DrawRectangleRec(btnMovies, hoverMovies ? LIGHTGRAY : GRAY);
        DrawText("MOVIES", btnMovies.x + 60, btnMovies.y + 15, 20, WHITE);

        DrawRectangleRec(btnExit, hoverExit ? RED : MAROON);
        DrawText("EXIT", btnExit.x + 75, btnExit.y + 15, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
