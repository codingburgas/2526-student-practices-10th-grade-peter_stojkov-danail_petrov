#pragma once
#include "raylib.h"
#include <string>
#include <vector>

struct BookingCard {
    std::string sourceLine;
    std::string timestamp;
    std::string show;
    std::string seats;
    std::string seatCount;
    std::string totalAmount;
    std::string paymentMethod;
    std::string channel;
    std::string status;
};

class MyBookingsScreen {
public:
    MyBookingsScreen(int screenWidth, int screenHeight);

    void SetUsername(const std::string& username);
    void Update(bool& goBack);
    void Draw();

private:
    void LoadBookings();
    void DeleteBooking(const std::string& sourceLine);

    int screenWidth;
    int screenHeight;
    std::string username;
    std::vector<BookingCard> bookings;
    float scrollOffset;
};
