#include "my_bookings.h"
#include "theme.h"
#include <fstream>
#include <string>
#include <vector>

static std::vector<std::string> SplitCsvLine(const std::string& line) {
    std::vector<std::string> values;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); i++) {
        char ch = line[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            values.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    values.push_back(current);
    return values;
}

static std::string GetBookingsCsvPath() {
    if (FileExists("data/bookings.csv")) {
        return "data/bookings.csv";
    }

    if (FileExists("Cinema_Design/data/bookings.csv") || DirectoryExists("Cinema_Design")) {
        return "Cinema_Design/data/bookings.csv";
    }

    return "data/bookings.csv";
}

static std::string FormatPayment(const std::string& value) {
    if (value == "credit_card") {
        return "Credit card";
    }
    if (value == "cash") {
        return "Cash";
    }
    return value;
}

static std::string FormatChannel(const std::string& value) {
    if (value == "walk_in") {
        return "Walk-in";
    }
    if (value == "online") {
        return "Online";
    }
    return value;
}

MyBookingsScreen::MyBookingsScreen(int w, int h)
    : screenWidth(w), screenHeight(h), scrollOffset(0.0f)
{
}

void MyBookingsScreen::SetUsername(const std::string& name) {
    username = name;
    scrollOffset = 0.0f;
    LoadBookings();
}

void MyBookingsScreen::LoadBookings() {
    bookings.clear();

    std::ifstream file(GetBookingsCsvPath());
    if (!file.is_open()) {
        return;
    }

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }

        std::vector<std::string> values = SplitCsvLine(line);
        if (values.size() >= 9 && values[1] == username) {
            bookings.push_back({
                line,
                values[0],
                values[2],
                values[3],
                values[4],
                values[5],
                values[6],
                values[7],
                values[8]
            });
        }
    }
}

void MyBookingsScreen::DeleteBooking(const std::string& sourceLine) {
    std::string path = GetBookingsCsvPath();
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string header;
    std::getline(file, header);

    std::vector<std::string> rows;
    std::string line;
    bool deleted = false;
    while (std::getline(file, line)) {
        if (!deleted && line == sourceLine) {
            deleted = true;
        } else {
            rows.push_back(line);
        }
    }
    file.close();

    std::ofstream out(path);
    if (!out.is_open()) {
        return;
    }

    out << header << '\n';
    for (const std::string& row : rows) {
        out << row << '\n';
    }

    LoadBookings();
}

void MyBookingsScreen::Update(bool& goBack) {
    goBack = false;

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float y = 125.0f - scrollOffset;
        for (const BookingCard& booking : bookings) {
            Rectangle card = { 50.0f, y, (float)screenWidth - 100.0f, 132.0f };
            Rectangle deleteBtn = { card.x + card.width - 118.0f, card.y + card.height - 42.0f, 86.0f, 28.0f };
            if (CheckCollisionPointRec(mouse, deleteBtn)) {
                DeleteBooking(booking.sourceLine);
                return;
            }
            y += card.height + 16.0f;
        }
    }

    scrollOffset += GetMouseWheelMove() * -30.0f;
    if (scrollOffset < 0.0f) {
        scrollOffset = 0.0f;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        goBack = true;
    }
}

void MyBookingsScreen::Draw() {
    const ThemePalette& theme = GetTheme();

    DrawText("MY BOOKINGS", 50, 24, 34, theme.text);
    DrawText("BACKSPACE to return", screenWidth - 190, 34, 14, theme.textDim);
    DrawLine(0, 90, screenWidth, 90, theme.border);

    if (bookings.empty()) {
        const char* emptyText = "No bookings yet.";
        int ew = MeasureText(emptyText, 28);
        DrawText(emptyText, screenWidth / 2 - ew / 2, screenHeight / 2 - 20, 28, theme.textDim);
        return;
    }

    float y = 125.0f - scrollOffset;
    for (const BookingCard& booking : bookings) {
        Rectangle card = { 50.0f, y, (float)screenWidth - 100.0f, 132.0f };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), card);
        DrawRectangleRounded(card, 0.12f, 8, theme.card);
        DrawRectangleRoundedLines(card, 0.12f, 8, hovered ? theme.accent : theme.border);

        DrawText(booking.show.c_str(), (int)card.x + 24, (int)card.y + 18, 21, theme.text);
        DrawText(booking.timestamp.c_str(), (int)card.x + 24, (int)card.y + 50, 15, theme.textDim);

        std::string seats = "Seats: " + booking.seats + " (" + booking.seatCount + ")";
        DrawText(seats.c_str(), (int)card.x + 24, (int)card.y + 78, 17, theme.accent);

        std::string total = booking.totalAmount + " Euro";
        int tw = MeasureText(total.c_str(), 24);
        DrawText(total.c_str(), (int)(card.x + card.width - tw - 26), (int)card.y + 22, 24, theme.text);

        std::string meta = FormatPayment(booking.paymentMethod) + " | " +
            FormatChannel(booking.channel) + " | " + booking.status;
        int mw = MeasureText(meta.c_str(), 15);
        DrawText(meta.c_str(), (int)(card.x + card.width - mw - 130), (int)card.y + 74, 15, theme.textDim);

        Rectangle deleteBtn = { card.x + card.width - 118.0f, card.y + card.height - 42.0f, 86.0f, 28.0f };
        bool deleteHover = CheckCollisionPointRec(GetMousePosition(), deleteBtn);
        DrawRectangleRounded(deleteBtn, 0.35f, 8, deleteHover ? theme.dangerHover : theme.danger);
        DrawText("DELETE", (int)deleteBtn.x + 16, (int)deleteBtn.y + 7, 14, WHITE);

        y += card.height + 16.0f;
    }
}
