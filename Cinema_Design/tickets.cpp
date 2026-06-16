#include "raylib.h"
#include "tickets.h"
#include <vector>
#include <string>

using namespace std;

const int ROWS = 8;
const int COLS = 12;
vector<vector<Seat>> cinemaHall(ROWS, vector<Seat>(COLS));
int selectedCount = 0;
float ticketPrice = 12.50f;
bool isInitialized = false;

void InitTicketsPage() {
    selectedCount = 0;
    float startX = (800.0f - (COLS * (35.0f + 8.0f) - 8.0f)) / 2.0f;
    float startY = 180.0f;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            cinemaHall[r][c].row = r + 1;
            cinemaHall[r][c].col = c + 1;
            cinemaHall[r][c].isSelected = false;
            cinemaHall[r][c].isReserved = ((r + c) % 5 == 0 || (r * c) % 7 == 0);
            cinemaHall[r][c].bounds = { startX + c * (35.0f + 8.0f), startY + r * (35.0f + 8.0f), 35.0f, 35.0f };
        }
    }
    isInitialized = true;
}

void UpdateTicketsPage(int& currentScreen) {
    if (!isInitialized) InitTicketsPage();

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (CheckCollisionPointRec(mousePos, cinemaHall[r][c].bounds)) {
                    if (!cinemaHall[r][c].isReserved) {
                        if (cinemaHall[r][c].isSelected) {
                            cinemaHall[r][c].isSelected = false;
                            selectedCount--;
                        }
                        else {
                            cinemaHall[r][c].isSelected = true;
                            selectedCount++;
                        }
                    }
                }
            }
        }

        Rectangle confirmBtn = { 400.0f - 75.0f, 530.0f, 150.0f, 40.0f };
        if (CheckCollisionPointRec(mousePos, confirmBtn) && selectedCount > 0) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (cinemaHall[r][c].isSelected) {
                        cinemaHall[r][c].isSelected = false;
                        cinemaHall[r][c].isReserved = true;
                    }
                }
            }
            selectedCount = 0;
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        currentScreen = 0;
    }
}

void DrawTicketsPage() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    DrawText("TICKETS PAGE", screenWidth / 2 - MeasureText("TICKETS PAGE", 40) / 2, 40, 40, DARKBLUE);
    DrawText("Press BACKSPACE to return", 10, screenHeight - 30, 20, GRAY);

    DrawRectangle(150, 110, 500, 12, LIGHTGRAY);
    DrawText("SCREEN", screenWidth / 2 - MeasureText("SCREEN", 14) / 2, 130, 14, GRAY);

    float startX = (800.0f - (COLS * (35.0f + 8.0f) - 8.0f)) / 2.0f;
    float startY = 180.0f;

    for (int r = 0; r < ROWS; r++) {
        string rowNum = to_string(r + 1);
        int textX = (int)(startX - 25.0f);
        int textY = (int)(startY + r * (35.0f + 8.0f) + 8.0f);
        DrawText(rowNum.c_str(), textX, textY, 16, GRAY);

        for (int c = 0; c < COLS; c++) {
            Color seatColor = LIME;
            if (cinemaHall[r][c].isReserved) seatColor = RED;
            else if (cinemaHall[r][c].isSelected) seatColor = ORANGE;

            DrawRectangleRec(cinemaHall[r][c].bounds, seatColor);
            DrawRectangleLinesEx(cinemaHall[r][c].bounds, 1.5f, BLACK);
        }
    }

    float totalCost = (float)selectedCount * ticketPrice;
    string statsText = "Tickets: " + to_string(selectedCount) + " | Total: $" + to_string(totalCost);
    DrawText(statsText.c_str(), screenWidth / 2 - MeasureText(statsText.c_str(), 18) / 2, 490, 18, BLACK);

    Rectangle confirmBtn = { 400.0f - 75.0f, 530.0f, 150.0f, 40.0f };
    Color btnColor = (selectedCount > 0) ? Color{ 143, 19, 21, 255 } : GRAY;

    DrawRectangleRec(confirmBtn, btnColor);
    DrawRectangleLinesEx(confirmBtn, 1.0f, BLACK);

    int btnTextX = (int)(confirmBtn.x + (confirmBtn.width - MeasureText("CONFIRM", 16)) / 2.0f);
    int btnTextY = (int)(confirmBtn.y + 12.0f);
    DrawText("CONFIRM", btnTextX, btnTextY, 16, WHITE);
}
