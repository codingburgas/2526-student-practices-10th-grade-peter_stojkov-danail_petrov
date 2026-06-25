#pragma once
#include "raylib.h"
#include "screens/menu.h"
#include "screens/login.h"
#include "screens/movies.h"
#include "screens/movie_details.h"
#include "screens/my_bookings.h"

#include "screens/payment.h"
#include "screens/showtimes.h"
#include "screens/tickets.h"

enum class AppState {
    Login,
    Menu,
    Movies,
    MyBookings,
    MovieDetails,
    Showtimes,
    Tickets,
    Payment
};

class CinemaApp {
public:
    CinemaApp(int w, int h);
    void Run();

private:
    int width;
    int height;

    LoginScreen login;
    CinemaMenu menu;
    MoviesScreen movies;
    MyBookingsScreen myBookings;
    MovieDetailsScreen movieDetails;
    ShowtimesScreen showtimes;
    TicketsScreen tickets;
    PaymentScreen payment;

    AppState currentState;
};
