#include "tickets.h"
#include "language.h"
#include "theme.h"
#include "ui_font.h"
#include <sstream>
#include <string>

#define DrawText DrawAppText
#define MeasureText MeasureAppText

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color SEAT_AVAIL    = { 51, 65, 85, 255 };
static Color SEAT_HOVER    = { 80, 95, 120, 255 };
static Color SEAT_RESERVED = { 120, 30, 30, 255 };
static Color SEAT_SELECTED = { 0, 240, 255, 255 };
static Color TEXT_LIGHT    = { 248, 250, 252, 255 };
static Color TEXT_DIM      = { 148, 163, 184, 255 };
static Color GREEN_BTN     = { 0, 200, 100, 255 };
static Color GREEN_HOVER   = { 0, 240, 120, 255 };
static Color CYAN_GLOW     = { 0, 240, 255, 60 };

static void ApplyTicketsTheme() {
    const ThemePalette& theme = GetTheme();
    DARK_BG = theme.background;
    CARD_COLOR = theme.card;
    BORDER_COLOR = theme.border;
    SEAT_AVAIL = theme.seatAvailable;
    SEAT_HOVER = theme.seatHover;
    SEAT_SELECTED = theme.accent;
    TEXT_LIGHT = theme.text;
    TEXT_DIM = theme.textDim;
    GREEN_BTN = theme.success;
    GREEN_HOVER = Color{0, 200, 105, 255};
    CYAN_GLOW = Color{theme.accent.r, theme.accent.g, theme.accent.b, 60};
}

TicketsScreen::TicketsScreen(int w, int h)
    : screenWidth(w),
    screenHeight(h),
    rows(8),
    cols(12),
    selectedCount(0),
    lastBookingSeatCount(0),
    lastBookingTotal(0.0f)
{
    confirmBtn = { screenWidth / 2.0f - 100.0f, screenHeight - 82.0f, 200.0f, 50.0f };
}

void TicketsScreen::LoadDemoHall(const std::string& label) {
    showLabel = label;
    seats.clear();
    selectedCount = 0;
    lastBookingSeats.clear();
    lastBookingSeatCount = 0;
    lastBookingTotal = 0.0f;

    const float seatStep = 46.0f;
    const float seatSize = 38.0f;
    float startX = screenWidth / 2.0f - (cols * seatStep) / 2.0f;
    float startY = 160.0f;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            UISeat s{};
            s.row = r + 1;
            s.col = c + 1;
            if (r < 2) {
                s.type = "Platinum";
                s.price = 20.0f;
            } else if (r < 5) {
                s.type = "Gold";
                s.price = 15.0f;
            } else {
                s.type = "Silver";
                s.price = 10.0f;
            }
            s.isSelected = false;
            s.isReserved = ((r + c) % 7 == 0);

            s.bounds = { startX + c * seatStep, startY + r * seatStep, seatSize, seatSize };

            seats.push_back(s);
        }
    }
}

void TicketsScreen::Update(bool& goBack, bool& confirmed) {
    goBack = false;
    confirmed = false;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (auto& s : seats) {
            if (CheckCollisionPointRec(mouse, s.bounds)) {
                if (!s.isReserved) {
                    s.isSelected = !s.isSelected;
                    selectedCount += s.isSelected ? 1 : -1;
                }
            }
        }

        if (CheckCollisionPointRec(mouse, confirmBtn) && selectedCount > 0) {
            confirmed = true;
            lastBookingTotal = GetTotalAmount();
            lastBookingSeatCount = selectedCount;
            std::ostringstream bookedSeats;
            for (auto& s : seats) {
                if (s.isSelected) {
                    if (bookedSeats.tellp() > 0) {
                        bookedSeats << ' ';
                    }
                    bookedSeats << "R" << s.row << "C" << s.col;
                    s.isSelected = false;
                    s.isReserved = true;
                }
            }
            lastBookingSeats = bookedSeats.str();
            selectedCount = 0;
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE))
        goBack = true;
}

void TicketsScreen::Draw() {
    ApplyTicketsTheme();

    int cx = screenWidth / 2;

    DrawText(TextFor("SELECT YOUR SEATS"), cx - MeasureText(TextFor("SELECT YOUR SEATS"), 28) / 2, 14, 28, TEXT_LIGHT);
    if (!showLabel.empty()) {
        DrawText(showLabel.c_str(), cx - MeasureText(showLabel.c_str(), 14) / 2, 48, 14, TEXT_DIM);
    }

    Rectangle screenBar = { (float)(cx - 260), 82, 520, 8 };
    for (int i = 0; i < 3; i++) {
        float spread = (float)(i * 6);
        DrawRectangleRounded(
            { screenBar.x - spread, screenBar.y - spread, screenBar.width + spread * 2, screenBar.height },
            1.0f, 12, Color{0, 240, 255, (unsigned char)(20 + i * 10)});
    }
    DrawRectangleRounded(screenBar, 1.0f, 12, CYAN_GLOW);
    DrawText(TextFor("SCREEN"), cx - MeasureText(TextFor("SCREEN"), 14) / 2, 102, 14, TEXT_DIM);

    Vector2 mouse = GetMousePosition();
    for (auto& s : seats) {
        Color col;
        if (s.isReserved) {
            col = SEAT_RESERVED;
        } else if (s.isSelected) {
            col = SEAT_SELECTED;
        } else {
            bool hovered = CheckCollisionPointRec(mouse, s.bounds);
            if (hovered) {
                col = SEAT_HOVER;
            } else if (s.type == "Platinum") {
                col = IsLightTheme() ? Color{ 196, 181, 253, 255 } : Color{ 90, 70, 130, 255 };
            } else if (s.type == "Gold") {
                col = IsLightTheme() ? Color{ 245, 191, 86, 255 } : Color{ 120, 92, 38, 255 };
            } else {
                col = SEAT_AVAIL;
            }
        }
        DrawRectangleRounded(s.bounds, 0.35f, 6, col);

        if (s.isSelected) {
            Rectangle glow = { s.bounds.x - 2, s.bounds.y - 2, s.bounds.width + 4, s.bounds.height + 4 };
            DrawRectangleRoundedLines(glow, 0.35f, 6, Color{0, 240, 255, 80});
        }
    }

    int lx = cx - 170;
    int ly = screenHeight - 150;
    DrawRectangleRounded({(float)(lx - 90), (float)ly, 18, 18}, 0.35f, 4, SEAT_AVAIL);
    DrawText("Silver 10", lx - 64, ly + 1, 16, TEXT_DIM);

    DrawRectangleRounded({(float)(lx + 20), (float)ly, 18, 18}, 0.35f, 4,
        IsLightTheme() ? Color{ 245, 191, 86, 255 } : Color{ 120, 92, 38, 255 });
    DrawText("Gold 15", lx + 46, ly + 1, 16, TEXT_DIM);

    DrawRectangleRounded({(float)(lx + 120), (float)ly, 18, 18}, 0.35f, 4,
        IsLightTheme() ? Color{ 196, 181, 253, 255 } : Color{ 90, 70, 130, 255 });
    DrawText("Platinum 20", lx + 146, ly + 1, 16, TEXT_DIM);

    DrawRectangleRounded({(float)(lx + 260), (float)ly, 18, 18}, 0.35f, 4, SEAT_SELECTED);
    DrawText(TextFor("Selected"), lx + 286, ly + 1, 16, TEXT_DIM);

    DrawRectangleRounded({(float)(lx + 380), (float)ly, 18, 18}, 0.35f, 4, SEAT_RESERVED);
    DrawText(TextFor("Taken"), lx + 406, ly + 1, 16, TEXT_DIM);

    float total = GetTotalAmount();
    int roundedTotal = (int)(total + 0.5f);
    std::string info = "Seats: " + std::to_string(selectedCount) +
        "  |  Total: " + std::to_string(roundedTotal) + " Euro";

    DrawText(info.c_str(), cx - MeasureText(info.c_str(), 22) / 2, screenHeight - 118, 22, TEXT_LIGHT);

    bool confirmHover = CheckCollisionPointRec(mouse, confirmBtn);
    Color btnColor = selectedCount > 0 ? (confirmHover ? GREEN_HOVER : GREEN_BTN) : SEAT_AVAIL;
    DrawRectangleRounded(confirmBtn, 0.5f, 8, btnColor);

    const char* btnText = TextFor("CONFIRM BOOKING");
    int bw = MeasureText(btnText, 20);
    DrawText(btnText, (int)(confirmBtn.x + (confirmBtn.width - bw) / 2), (int)(confirmBtn.y + 14), 20, DARK_BG);

    DrawText(TextFor("BACKSPACE to go back"), 20, screenHeight - 28, 14, TEXT_DIM);
}

float TicketsScreen::GetTotalAmount() const {
    if (lastBookingSeatCount > 0) {
        return lastBookingTotal;
    }

    float total = 0.0f;
    for (const auto& s : seats) {
        if (s.isSelected) {
            total += s.price;
        }
    }
    return total;
}

const std::string& TicketsScreen::GetLastBookingSeats() const {
    return lastBookingSeats;
}

int TicketsScreen::GetLastBookingSeatCount() const {
    return lastBookingSeatCount;
}

const std::string& TicketsScreen::GetShowLabel() const {
    return showLabel;
}
