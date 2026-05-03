#include "raylib.h"
#include "tickets.h"

void DrawTicketsPage() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    DrawText("TICKETS PAGE", screenWidth / 2 - MeasureText("TICKETS PAGE", 40) / 2, 100, 40, DARKBLUE);
    DrawText("Films: ......", 200, 200, 20, BLACK);

    DrawText("Press BACKSPACE to return", 10, screenHeight - 30, 20, GRAY);
}
