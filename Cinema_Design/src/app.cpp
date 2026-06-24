#include "app.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>

static std::string CsvEscape(const std::string& value) {
    bool needsQuotes = value.find_first_of(",\"\n") != std::string::npos;
    if (!needsQuotes) {
        return value;
    }

    std::string escaped = "\"";
    for (char ch : value) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped += ch;
        }
    }
    escaped += '"';
    return escaped;
}

static std::string CurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char buffer[32]{};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return buffer;
}

static void AppendBookingCsv(
    const std::string& username,
    const std::string& show,
    const std::string& seats,
    int seatCount,
    float totalAmount,
    const std::string& paymentMethod,
    const std::string& channel
) {
    const char* path = "data/bookings.csv";
    std::filesystem::create_directories("data");
    std::ifstream existing(path);
    bool needsHeader = !existing.good() || existing.peek() == std::ifstream::traits_type::eof();
    existing.close();

    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return;
    }

    if (needsHeader) {
        file << "timestamp,username,show,seats,seat_count,total_amount,payment_method,channel,status\n";
    }

    std::ostringstream amount;
    amount << totalAmount;

    file << CsvEscape(CurrentTimestamp()) << ','
         << CsvEscape(username) << ','
         << CsvEscape(show) << ','
         << CsvEscape(seats) << ','
         << seatCount << ','
         << amount.str() << ','
         << CsvEscape(paymentMethod) << ','
         << CsvEscape(channel) << ','
         << "confirmed\n";
}

static void AppendNotificationCsv(
    const std::string& type,
    const std::string& username,
    const std::string& message
) {
    const char* path = "data/notifications.csv";
    std::filesystem::create_directories("data");
    std::ifstream existing(path);
    bool needsHeader = !existing.good() || existing.peek() == std::ifstream::traits_type::eof();
    existing.close();

    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return;
    }

    if (needsHeader) {
        file << "timestamp,type,username,message\n";
    }

    file << CsvEscape(CurrentTimestamp()) << ','
         << CsvEscape(type) << ','
         << CsvEscape(username) << ','
         << CsvEscape(message) << '\n';
}

CinemaApp::CinemaApp(int w, int h)
    : width(w), height(h),
    login(w, h),
    menu(w, h),
    movies(w, h),
    showtimes(w, h),
    tickets(w, h),
    payment(w, h),
    currentState(AppState::Login)
{
}

void CinemaApp::Run() {
    InitWindow(width, height, "Cinema Booking System");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Color{ 15, 23, 42, 255 });

        switch (currentState) {
            case AppState::Login: {
                bool loggedIn = false;
                login.Update(loggedIn);
                login.Draw();
                if (loggedIn) {
                    currentState = AppState::Menu;
                }
                break;
            }
            case AppState::Menu: {
                MenuResult result = MenuResult::NONE;
                menu.Update(result);
                menu.Draw();
                if (result == MenuResult::OPEN_MOVIES) {
                    currentState = AppState::Movies;
                } else if (result == MenuResult::EXIT_APP) {
                    CloseWindow();
                    return; // Exit Run()
                }
                break;
            }
            case AppState::Movies: {
                bool goBack = false;
                bool movieSelected = false;
                movies.Update(goBack, movieSelected);
                movies.Draw();
                if (goBack) {
                    currentState = AppState::Menu;
                } else if (movieSelected) {
                    showtimes.SetMovie(movies.GetSelectedMovieTitle());
                    currentState = AppState::Showtimes;
                }
                break;
            }
            case AppState::Showtimes: {
                bool goBack = false;
                bool showSelected = false;
                showtimes.Update(goBack, showSelected);
                showtimes.Draw();
                if (goBack) {
                    currentState = AppState::Movies;
                } else if (showSelected) {
                    tickets.LoadDemoHall(showtimes.GetSelectedShowLabel());
                    currentState = AppState::Tickets;
                }
                break;
            }
            case AppState::Tickets: {
                bool goBack = false;
                bool confirmed = false;
                tickets.Update(goBack, confirmed);
                tickets.Draw();
                if (goBack) {
                    currentState = AppState::Showtimes;
                } else if (confirmed) {
                    payment.SetTotalAmount(tickets.GetTotalAmount());
                    payment.SetBookingChannel(showtimes.GetBookingChannel());
                    currentState = AppState::Payment;
                }
                break;
            }
            case AppState::Payment: {
                PaymentResult res = PaymentResult::NONE;
                payment.Update(res);
                payment.Draw();
                if (res == PaymentResult::PAID_CREDIT_CARD || res == PaymentResult::PAID_CASH) {
                    AppendBookingCsv(
                        login.GetUsername(),
                        tickets.GetShowLabel(),
                        tickets.GetLastBookingSeats(),
                        tickets.GetLastBookingSeatCount(),
                        tickets.GetTotalAmount(),
                        res == PaymentResult::PAID_CREDIT_CARD ? "credit_card" : "cash",
                        showtimes.GetBookingChannel() == BookingChannel::Online ? "online" : "walk_in"
                    );
                    AppendNotificationCsv(
                        "booking_made",
                        login.GetUsername(),
                        "Booking confirmed for " + tickets.GetShowLabel()
                    );
                } else if (res == PaymentResult::DONE) {
                    currentState = AppState::Menu;
                } else if (res == PaymentResult::CANCEL) {
                    AppendNotificationCsv(
                        "booking_canceled",
                        login.GetUsername(),
                        "Booking canceled before payment for " + tickets.GetShowLabel()
                    );
                    currentState = AppState::Menu;
                }
                break;
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
