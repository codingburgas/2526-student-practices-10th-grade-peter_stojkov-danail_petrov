#pragma once
#include "raylib.h"
#include "screens/menu.h"
#include "screens/login.h"
#include "screens/movies.h"

#include "screens/payment.h"
#include "screens/showtimes.h"
#include "screens/tickets.h"

enum class AppState {
    Login,
    Menu,
    Movies,
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
    ShowtimesScreen showtimes;
    TicketsScreen tickets;
    PaymentScreen payment;

    AppState currentState;
};
