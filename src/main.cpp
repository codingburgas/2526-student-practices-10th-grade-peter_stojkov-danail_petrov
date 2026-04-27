#include "raylib.h"
#include "../Cinema_Design/design.h"

#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Cinema Paradise - Main Menu");

    Rectangle btnTickets = { screenWidth / 2 - 100, 250, 200, 50 };
    Rectangle btnMovies = { screenWidth / 2 - 100, 320, 200, 50 };
    Rectangle btnExit = { screenWidth / 2 - 100, 390, 200, 50 };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, btnTickets) ||
            CheckCollisionPointRec(mousePoint, btnMovies) ||
            CheckCollisionPointRec(mousePoint, btnExit)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        if (CheckCollisionPointRec(mousePoint, btnExit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("CINEMA PARADISE", screenWidth / 2 - MeasureText("CINEMA PARADISE", 40) / 2, 100, 40, MAROON);
        DrawText("Please select an option:", screenWidth / 2 - MeasureText("Please select an option:", 20) / 2, 160, 20, DARKGRAY);

        DrawRectangleRec(btnTickets, CheckCollisionPointRec(mousePoint, btnTickets) ? LIGHTGRAY : GRAY);
        DrawText("TICKETS", btnTickets.x + 55, btnTickets.y + 15, 20, WHITE);

        DrawRectangleRec(btnMovies, CheckCollisionPointRec(mousePoint, btnMovies) ? LIGHTGRAY : GRAY);
        DrawText("MOVIES", btnMovies.x + 60, btnMovies.y + 15, 20, WHITE);

        DrawRectangleRec(btnExit, CheckCollisionPointRec(mousePoint, btnExit) ? RED : MAROON);
        DrawText("EXIT", btnExit.x + 75, btnExit.y + 15, 20, WHITE);

        DrawText("v1.0", 10, screenHeight - 20, 10, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
