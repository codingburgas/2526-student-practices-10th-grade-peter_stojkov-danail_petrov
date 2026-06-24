#pragma once
#include "raylib.h"
#include <string>
#include <vector>

struct UISeat {
    int row;
    int col;
    std::string type;
    float price;
    bool isReserved;
    bool isSelected;
    Rectangle bounds;
};

class TicketsScreen {
public:
    TicketsScreen(int screenWidth, int screenHeight);

    void LoadDemoHall(const std::string& showLabel = "");
    void Update(bool& goBack, bool& confirmed);
    void Draw();
    float GetTotalAmount() const;
    const std::string& GetLastBookingSeats() const;
    int GetLastBookingSeatCount() const;
    const std::string& GetShowLabel() const;

private:
    int screenWidth;
    int screenHeight;

    int rows;
    int cols;
    std::string showLabel;

    std::vector<UISeat> seats;
    int selectedCount;
    std::string lastBookingSeats;
    int lastBookingSeatCount;
    float lastBookingTotal;

    Rectangle confirmBtn;
};
