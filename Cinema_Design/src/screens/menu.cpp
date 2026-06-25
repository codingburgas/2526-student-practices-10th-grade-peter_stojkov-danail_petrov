#include "menu.h"
#include "language.h"
#include "theme.h"
#include "ui_font.h"

#define DrawText DrawAppText
#define MeasureText MeasureAppText

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TEXT_WHITE     = { 248, 250, 252, 255 };
static Color TEXT_DIM      = { 148, 163, 184, 255 };
static Color CYAN_ACC      = { 0, 240, 255, 255 };
static Color PINK_ACC      = { 255, 42, 109, 255 };
static Color PINK_HOVER    = { 255, 80, 130, 255 };

static void ApplyMenuTheme() {
    const ThemePalette& theme = GetTheme();
    DARK_BG = theme.background;
    CARD_COLOR = theme.card;
    BORDER_COLOR = theme.border;
    TEXT_WHITE = theme.text;
    TEXT_DIM = theme.textDim;
    CYAN_ACC = theme.accent;
    PINK_ACC = theme.pink;
    PINK_HOVER = theme.pinkHover;
}

CinemaMenu::CinemaMenu(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    float cx = w / 2.0f;
    btnMovies  = { cx - 130, 250, 260, 55 };
    btnMyBookings = { cx - 130, 330, 260, 55 };
    btnExit    = { cx - 130, 410, 260, 55 };
    btnTheme   = { (float)w - 222.0f, 28.0f, 92.0f, 36.0f };
    btnLanguage = { (float)w - 118.0f, 28.0f, 68.0f, 36.0f };
    btnLogout  = { cx - 55.0f, (float)h - 62.0f, 110.0f, 34.0f };

    hoverMovies = false;
    hoverMyBookings = false;
    hoverExit = false;
    hoverTheme = false;
    hoverLanguage = false;
    hoverLogout = false;
}

void CinemaMenu::Update(MenuResult& result) {
    result = MenuResult::NONE;

    Vector2 mouse = GetMousePosition();

    hoverMovies = CheckCollisionPointRec(mouse, btnMovies);
    hoverMyBookings = CheckCollisionPointRec(mouse, btnMyBookings);
    hoverTheme = CheckCollisionPointRec(mouse, btnTheme);
    hoverLanguage = CheckCollisionPointRec(mouse, btnLanguage);
    hoverExit = CheckCollisionPointRec(mouse, btnExit);
    hoverLogout = CheckCollisionPointRec(mouse, btnLogout);

    if (hoverMovies && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        result = MenuResult::OPEN_MOVIES;

    if (hoverMyBookings && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        result = MenuResult::OPEN_MY_BOOKINGS;

    if (hoverTheme && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        ToggleTheme();

    if (hoverLanguage && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        ToggleLanguage();

    if (hoverLogout && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        result = MenuResult::LOG_OUT;

    if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        result = MenuResult::EXIT_APP;
}

void CinemaMenu::Draw() {
    ApplyMenuTheme();

    int cx = screenWidth / 2;

    const char* title = TextFor("CINEMA");
    int tw = MeasureText(title, 70);
    DrawText(title, cx - tw / 2 + 2, 70, 70, Color{0, 0, 0, 80});
    DrawText(title, cx - tw / 2, 68, 70, CYAN_ACC);

    const char* sub = TextFor("DASHBOARD");
    int sw = MeasureText(sub, 20);
    DrawText(sub, cx - sw / 2, 150, 20, TEXT_DIM);

    DrawLine(cx - 120, 188, cx + 120, 188, BORDER_COLOR);

    DrawRectangleRounded(btnMovies, 0.4f, 8, hoverMovies ? PINK_HOVER : PINK_ACC);
    const char* moviesText = TextFor("BROWSE MOVIES");
    int mw = MeasureText(moviesText, 22);
    DrawText(moviesText, (int)(btnMovies.x + (btnMovies.width - mw) / 2), (int)(btnMovies.y + 17), 22, DARK_BG);

    DrawRectangleRounded(btnMyBookings, 0.4f, 8, hoverMyBookings ? CYAN_ACC : CARD_COLOR);
    DrawRectangleRoundedLines(btnMyBookings, 0.4f, 8, hoverMyBookings ? CYAN_ACC : BORDER_COLOR);
    const char* bookingsText = TextFor("MY BOOKINGS");
    int bw = MeasureText(bookingsText, 22);
    DrawText(bookingsText, (int)(btnMyBookings.x + (btnMyBookings.width - bw) / 2), (int)(btnMyBookings.y + 17), 22,
        hoverMyBookings ? DARK_BG : TEXT_WHITE);

    DrawRectangleRounded(btnTheme, 0.35f, 8, hoverTheme ? CYAN_ACC : CARD_COLOR);
    DrawRectangleRoundedLines(btnTheme, 0.35f, 8, hoverTheme ? CYAN_ACC : BORDER_COLOR);
    const char* themeText = TextFor(IsLightTheme() ? "DARK" : "LIGHT");
    int thw = MeasureText(themeText, 16);
    DrawText(themeText, (int)(btnTheme.x + (btnTheme.width - thw) / 2), (int)(btnTheme.y + 10), 16,
        hoverTheme ? DARK_BG : TEXT_WHITE);

    DrawRectangleRounded(btnLanguage, 0.35f, 8, hoverLanguage ? CYAN_ACC : CARD_COLOR);
    DrawRectangleRoundedLines(btnLanguage, 0.35f, 8, hoverLanguage ? CYAN_ACC : BORDER_COLOR);
    const char* languageText = IsBulgarian() ? "EN" : "BG";
    int langW = MeasureText(languageText, 16);
    DrawText(languageText, (int)(btnLanguage.x + (btnLanguage.width - langW) / 2), (int)(btnLanguage.y + 10), 16,
        hoverLanguage ? DARK_BG : TEXT_WHITE);

    DrawRectangleRounded(btnExit, 0.4f, 8, hoverExit ? GetTheme().danger : CARD_COLOR);
    DrawRectangleRoundedLines(btnExit, 0.4f, 8, hoverExit ? GetTheme().dangerHover : BORDER_COLOR);
    const char* exitText = TextFor("EXIT");
    int ew = MeasureText(exitText, 22);
    DrawText(exitText, (int)(btnExit.x + (btnExit.width - ew) / 2), (int)(btnExit.y + 17), 22,
        hoverExit ? WHITE : TEXT_WHITE);

    DrawRectangleRounded(btnLogout, 0.35f, 8, hoverLogout ? CARD_COLOR : GetTheme().background);
    DrawRectangleRoundedLines(btnLogout, 0.35f, 8, hoverLogout ? CYAN_ACC : BORDER_COLOR);
    const char* logoutText = TextFor("LOG OUT");
    int lw = MeasureText(logoutText, 15);
    DrawText(logoutText, (int)(btnLogout.x + (btnLogout.width - lw) / 2), (int)(btnLogout.y + 10), 15,
        hoverLogout ? CYAN_ACC : TEXT_DIM);
}
