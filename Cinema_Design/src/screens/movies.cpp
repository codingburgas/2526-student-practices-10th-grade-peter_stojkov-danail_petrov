#include "movies.h"
#include <algorithm>
#include <cctype>

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TITLE_COLOR   = { 248, 250, 252, 255 };
static Color SUBTEXT_COLOR = { 148, 163, 184, 255 };
static Color ACCENT_COLOR  = { 0, 240, 255, 255 };
static Color CARD_HOVER    = { 40, 55, 80, 255 };
static Color PINK_ACC      = { 255, 42, 109, 255 };

static int cardHeight = 126;
static constexpr float FONT_SPACING = 1.0f;

static Font GetMovieFont() {
    static Font font{};
    static bool initialized = false;
    static bool loaded = false;

    if (!initialized) {
        initialized = true;

        const char* fontPaths[] = {
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/trebuc.ttf",
            "C:/Windows/Fonts/arial.ttf"
        };

        for (const char* path : fontPaths) {
            if (FileExists(path)) {
                font = LoadFontEx(path, 48, nullptr, 0);
                SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
                loaded = true;
                break;
            }
        }
    }

    return loaded ? font : GetFontDefault();
}

static Vector2 MeasureMovieText(const std::string& text, float fontSize) {
    return MeasureTextEx(GetMovieFont(), text.c_str(), fontSize, FONT_SPACING);
}

static void DrawMovieText(const std::string& text, float x, float y, float fontSize, Color color) {
    DrawTextEx(GetMovieFont(), text.c_str(), { x, y }, fontSize, FONT_SPACING, color);
}

static void DrawMovieText(const char* text, float x, float y, float fontSize, Color color) {
    DrawTextEx(GetMovieFont(), text, { x, y }, fontSize, FONT_SPACING, color);
}

MoviesScreen::MoviesScreen(int w, int h)
    : screenWidth(w), screenHeight(h), scrollOffset(0.0f),
    typingSearch(false)
{
    searchBox = { 60, 130, 320, 38 };
    LoadMovies();
}

void MoviesScreen::LoadMovies() {
    movies = {
        { "Inception",       "English", "Sci-Fi",  "2010-07-16", "A thief enters dreams.",              148 },
        { "Interstellar",    "English", "Sci-Fi",  "2014-11-07", "A team travels through a wormhole.",  169 },
        { "The Dark Knight", "English", "Action",  "2008-07-18", "Batman faces the Joker.",             152 },
        { "Avatar",         "English", "Fantasy", "2009-12-18", "A marine joins the Na'vi.",           162 },
        { "Titanic",        "English", "Romance", "1997-12-19", "A love story aboard Titanic.",        195 }
    };
}

void MoviesScreen::Update(bool& goBack, bool& movieSelected) {
    goBack = false;
    movieSelected = false;

    Vector2 mouse = GetMousePosition();

    Rectangle searchArea = { (float)screenWidth - 310, 18, 250, 44 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        typingSearch = CheckCollisionPointRec(mouse, searchArea);
    }

    scrollOffset += GetMouseWheelMove() * -30;
    if (scrollOffset < 0) scrollOffset = 0;

    if (typingSearch) {
        bool changed = false;
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) {
                searchText.push_back((char)key);
                changed = true;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !searchText.empty()) {
            searchText.pop_back();
            changed = true;
        }

        if (changed) {
            filteredMovies.clear();
            std::string searchLower = toLower(searchText);
            for (auto& m : movies) {
                std::string titleLower = toLower(m.title);
                std::string languageLower = toLower(m.language);
                std::string genreLower = toLower(m.genre);
                std::string releaseLower = toLower(m.releaseDate);
                if (titleLower.find(searchLower) != std::string::npos ||
                    languageLower.find(searchLower) != std::string::npos ||
                    genreLower.find(searchLower) != std::string::npos ||
                    releaseLower.find(searchLower) != std::string::npos) {
                    filteredMovies.push_back(m);
                }
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && (!typingSearch || searchText.empty()))
        goBack = true;

    int y = 130 - (int)scrollOffset;
    auto& list = searchText.empty() ? movies : filteredMovies;

    for (auto& m : list) {
        Rectangle card = { 50, (float)y, (float)screenWidth - 100, (float)cardHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, card) && mouse.y > 90) {
            selectedMovieTitle = m.title;
            movieSelected = true;
            break;
        }

        y += cardHeight + 16;
    }
}

const std::string& MoviesScreen::GetSelectedMovieTitle() const {
    return selectedMovieTitle;
}

void MoviesScreen::Draw() {
    int y = 130 - (int)scrollOffset;
    auto& list = searchText.empty() ? movies : filteredMovies;

    for (int i = 0; i < (int)list.size(); i++) {
        auto& m = list[i];
        Rectangle card = { 50, (float)y, (float)screenWidth - 100, (float)cardHeight };

        bool hovered = CheckCollisionPointRec(GetMousePosition(), card) && GetMousePosition().y > 90;

        DrawRectangleRounded(card, 0.15f, 8, hovered ? CARD_HOVER : CARD_COLOR);
        DrawRectangleRoundedLines(card, 0.15f, 8, hovered ? ACCENT_COLOR : BORDER_COLOR);

        Rectangle poster = { card.x + 14, card.y + 14, 66, (float)cardHeight - 28 };
        DrawRectangleRounded(poster, 0.25f, 6, hovered ? ACCENT_COLOR : PINK_ACC);
        std::string initial = m.title.substr(0, 1);
        Vector2 initialSize = MeasureMovieText(initial, 34);
        DrawMovieText(
            initial,
            poster.x + (poster.width - initialSize.x) / 2,
            poster.y + (poster.height - initialSize.y) / 2 - 2,
            34,
            DARK_BG
        );

        float tx = card.x + 96;
        DrawMovieText(m.title, tx, card.y + 16, 24, TITLE_COLOR);

        std::string meta = m.genre + "  |  " + m.language + "  |  " + m.releaseDate;
        DrawMovieText(meta, tx, card.y + 50, 16, ACCENT_COLOR);

        DrawMovieText(m.description, tx, card.y + 76, 16, SUBTEXT_COLOR);

        std::string dur = std::to_string(m.duration) + " min";
        Vector2 durSize = MeasureMovieText(dur, 16);
        DrawMovieText(dur, card.x + card.width - durSize.x - 24, card.y + 18, 16, SUBTEXT_COLOR);

        if (hovered) {
            const char* hint = "Click to book >";
            Vector2 hintSize = MeasureMovieText(hint, 15);
            DrawMovieText(hint, card.x + card.width - hintSize.x - 24, card.y + cardHeight - 32, 15, ACCENT_COLOR);
        }

        y += cardHeight + 16;
    }

    DrawRectangle(0, 0, screenWidth, 90, DARK_BG);

    float lineAlpha = 60.0f;
    DrawLine(0, 90, screenWidth, 90, Color{0, 240, 255, (unsigned char)lineAlpha});

    DrawMovieText("MOVIES", 50, 18, 34, TITLE_COLOR);
    DrawMovieText("Press BACKSPACE to return", 50, 58, 15, SUBTEXT_COLOR);

    Rectangle searchArea = { (float)screenWidth - 310, 18, 250, 44 };
    DrawRectangleRounded(searchArea, 0.5f, 8, CARD_COLOR);
    DrawRectangleRoundedLines(searchArea, 0.5f, 8, typingSearch ? ACCENT_COLOR : BORDER_COLOR);

    DrawMovieText("Q", searchArea.x + 14, searchArea.y + 10, 20, SUBTEXT_COLOR);

    if (searchText.empty() && !typingSearch) {
        DrawMovieText("Search movies...", searchArea.x + 42, searchArea.y + 12, 18, SUBTEXT_COLOR);
    } else {
        DrawMovieText(searchText, searchArea.x + 42, searchArea.y + 12, 18, TITLE_COLOR);
    }
}
