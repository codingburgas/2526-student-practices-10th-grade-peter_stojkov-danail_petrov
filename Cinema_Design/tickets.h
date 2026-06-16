#pragma once
#include "raylib.h"

struct Seat {
    int row;
    int col;
    bool isReserved;
    bool isSelected;
    Rectangle bounds;
};

void InitTicketsPage();
void UpdateTicketsPage(int& currentScreen);
void DrawTicketsPage();
