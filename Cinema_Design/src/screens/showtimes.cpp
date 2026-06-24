#include "showtimes.h"
#include <string>

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TITLE_COLOR   = { 248, 250, 252, 255 };
static Color SUBTEXT_COLOR = { 148, 163, 184, 255 };
static Color ACCENT_COLOR  = { 0, 240, 255, 255 };
static Color PINK_ACC      = { 255, 42, 109, 255 };
static Color CARD_HOVER    = { 40, 55, 80, 255 };

ShowtimesScreen::ShowtimesScreen(int w, int h)
    : screenWidth(w), screenHeight(h), bookingChannel(BookingChannel::Online)
{
    onlineBtn = { 50, 96, 170, 42 };
    walkInBtn = { 235, 96, 170, 42 };
}

void ShowtimesScreen::SetMovie(const std::string& title) {
    movieTitle = title;
    selectedShowLabel.clear();
    bookingChannel = BookingChannel::Online;
    LoadDemoShows();
}

void ShowtimesScreen::LoadDemoShows() {
    shows.clear();

    std::string baseMovie = movieTitle.empty() ? "Movie" : movieTitle;
    shows = {
        { "Cinema City Paradise", "Sofia", "Hall 1", "Today 18:30", {} },
        { "Arena Mall", "Sofia", "Hall 3", "Today 20:45", {} },
        { "Kino Arena", "Plovdiv", "Hall 2", "Tomorrow 19:15", {} },
        { "Cinema City", "Varna", "Hall 4", "Tomorrow 21:00", {} }
    };

    float y = 165.0f;
    for (auto& show : shows) {
        show.bounds = { 50.0f, y, (float)screenWidth - 100.0f, 78.0f };
        y += 94.0f;
    }
}

void ShowtimesScreen::Update(bool& goBack, bool& showSelected) {
    goBack = false;
    showSelected = false;

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, onlineBtn)) {
            bookingChannel = BookingChannel::Online;
        } else if (CheckCollisionPointRec(mouse, walkInBtn)) {
            bookingChannel = BookingChannel::WalkIn;
        }

        for (const auto& show : shows) {
            if (CheckCollisionPointRec(mouse, show.bounds)) {
                selectedShowLabel = movieTitle + " | " + show.cinema + " | " + show.hall + " | " + show.time;
                showSelected = true;
                break;
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        goBack = true;
    }
}

void ShowtimesScreen::Draw() {
    DrawText("SHOWTIMES", 50, 20, 32, TITLE_COLOR);
    DrawText(movieTitle.c_str(), 50, 58, 18, ACCENT_COLOR);
    DrawText("BACKSPACE to return", screenWidth - 190, 28, 14, SUBTEXT_COLOR);

    Vector2 mouse = GetMousePosition();

    bool onlineSelected = bookingChannel == BookingChannel::Online;
    bool walkInSelected = bookingChannel == BookingChannel::WalkIn;
    DrawRectangleRounded(onlineBtn, 0.35f, 8, onlineSelected ? ACCENT_COLOR : CARD_COLOR);
    DrawRectangleRoundedLines(onlineBtn, 0.35f, 8, onlineSelected ? ACCENT_COLOR : BORDER_COLOR);
    DrawText("ONLINE", (int)onlineBtn.x + 44, (int)onlineBtn.y + 12, 18, onlineSelected ? DARK_BG : TITLE_COLOR);

    DrawRectangleRounded(walkInBtn, 0.35f, 8, walkInSelected ? PINK_ACC : CARD_COLOR);
    DrawRectangleRoundedLines(walkInBtn, 0.35f, 8, walkInSelected ? PINK_ACC : BORDER_COLOR);
    DrawText("WALK-IN", (int)walkInBtn.x + 44, (int)walkInBtn.y + 12, 18, walkInSelected ? DARK_BG : TITLE_COLOR);

    DrawText("Choose a booking type, then select a cinema hall and showtime.", 430, 108, 16, SUBTEXT_COLOR);

    for (const auto& show : shows) {
        bool hovered = CheckCollisionPointRec(mouse, show.bounds);
        DrawRectangleRounded(show.bounds, 0.12f, 8, hovered ? CARD_HOVER : CARD_COLOR);
        DrawRectangleRoundedLines(show.bounds, 0.12f, 8, hovered ? ACCENT_COLOR : BORDER_COLOR);

        std::string cinemaLine = show.cinema + " - " + show.city;
        DrawText(cinemaLine.c_str(), (int)show.bounds.x + 22, (int)show.bounds.y + 14, 22, TITLE_COLOR);
        std::string hallLine = show.hall + "  |  " + show.time;
        DrawText(hallLine.c_str(), (int)show.bounds.x + 22, (int)show.bounds.y + 44, 18, ACCENT_COLOR);

        const char* hint = "Select >";
        int hw = MeasureText(hint, 16);
        DrawText(hint, (int)(show.bounds.x + show.bounds.width - hw - 24), (int)show.bounds.y + 30, 16, hovered ? ACCENT_COLOR : SUBTEXT_COLOR);
    }
}

const std::string& ShowtimesScreen::GetSelectedShowLabel() const {
    return selectedShowLabel;
}

BookingChannel ShowtimesScreen::GetBookingChannel() const {
    return bookingChannel;
}
