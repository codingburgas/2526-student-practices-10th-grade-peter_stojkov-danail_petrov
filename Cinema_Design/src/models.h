#pragma once
#include <string>
#include <vector>

enum class SeatType {
    Silver,
    Gold,
    Platinum
};

enum class PaymentMethod {
    CreditCard,
    Cash
};

enum class BookingStatus {
    Pending,
    Confirmed,
    Cancelled
};

struct Seat {
    int id;
    int row;
    int col;
    SeatType type;
    double price;
};

struct ShowSeat {
    int id;
    int seatId;
    int showId;
    bool isReserved;
};

struct Movie {
    int id;
    std::string title;
    std::string language;
    std::string genre;
    std::string releaseDate;
    int duration;
};

struct Hall {
    int id;
    int cinemaId;
    std::string name;
    std::vector<Seat> seats;
};

struct Cinema {
    int id;
    std::string name;
    std::string city;
};

struct Show {
    int id;
    int movieId;
    int hallId;
    std::string showTime;
};

struct User {
    int id;
    std::string username;
    std::string password;
    bool isAdmin;
};

struct Booking {
    int id;
    int userId;
    int showId;
    std::vector<int> showSeatIds;
    PaymentMethod paymentMethod;
    BookingStatus status;
    double totalPrice;
};
