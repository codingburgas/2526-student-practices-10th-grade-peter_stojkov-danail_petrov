#pragma once
#include "raylib.h"
#include <string>
#include <vector>

enum class BookingChannel {
    Online,
    WalkIn
};

struct ShowtimeOption {
    std::string cinema;
    std::string city;
    std::string hall;
    std::string time;
    Rectangle bounds;
};

class ShowtimesScreen {
public:
    ShowtimesScreen(int screenWidth, int screenHeight);

    void SetMovie(const std::string& title);
    void Update(bool& goBack, bool& showSelected);
    void Draw();

    const std::string& GetSelectedShowLabel() const;
    BookingChannel GetBookingChannel() const;

private:
    void LoadDemoShows();

    int screenWidth;
    int screenHeight;

    std::string movieTitle;
    std::string selectedShowLabel;
    BookingChannel bookingChannel;
    std::vector<ShowtimeOption> shows;

    Rectangle onlineBtn;
    Rectangle walkInBtn;
};
