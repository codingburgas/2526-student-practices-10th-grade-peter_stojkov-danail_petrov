#include "movies.h"
#include "language.h"
#include "theme.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

static std::string Trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace((unsigned char)value[start])) {
        start++;
    }

    size_t end = value.size();
    while (end > start && std::isspace((unsigned char)value[end - 1])) {
        end--;
    }

    return value.substr(start, end - start);
}

static bool TryParsePositiveInt(const std::string& value, int& result) {
    std::string trimmed = Trim(value);
    if (trimmed.empty()) {
        return false;
    }

    for (char ch : trimmed) {
        if (!std::isdigit((unsigned char)ch)) {
            return false;
        }
    }

    try {
        result = std::stoi(trimmed);
    } catch (...) {
        return false;
    }

    return result > 0;
}

static bool ContainsOption(const std::vector<std::string>& options, const std::string& value) {
    return std::find(options.begin(), options.end(), value) != options.end();
}

static int ExtractReleaseYear(const std::string& value) {
    if (value.size() < 4) {
        return 0;
    }

    std::string yearText = value.substr(0, 4);
    for (char ch : yearText) {
        if (!std::isdigit((unsigned char)ch)) {
            return 0;
        }
    }

    try {
        return std::stoi(yearText);
    } catch (...) {
        return 0;
    }
}

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TITLE_COLOR   = { 248, 250, 252, 255 };
static Color SUBTEXT_COLOR = { 148, 163, 184, 255 };
static Color ACCENT_COLOR  = { 0, 240, 255, 255 };
static Color CARD_HOVER    = { 40, 55, 80, 255 };
static Color PINK_ACC      = { 255, 42, 109, 255 };

static void ApplyMoviesTheme() {
    const ThemePalette& theme = GetTheme();
    DARK_BG = theme.background;
    CARD_COLOR = theme.card;
    BORDER_COLOR = theme.border;
    TITLE_COLOR = theme.text;
    SUBTEXT_COLOR = theme.textDim;
    ACCENT_COLOR = theme.accent;
    CARD_HOVER = theme.cardHover;
    PINK_ACC = theme.pink;
}

static int cardHeight = 142;
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

        static int codepoints[352]{};
        static bool codepointsReady = false;
        if (!codepointsReady) {
            int index = 0;
            for (int cp = 32; cp <= 126; cp++) {
                codepoints[index++] = cp;
            }
            for (int cp = 0x0400; cp <= 0x04FF; cp++) {
                codepoints[index++] = cp;
            }
            codepointsReady = true;
        }

        for (const char* path : fontPaths) {
            if (FileExists(path)) {
                font = LoadFontEx(path, 48, codepoints, 351);
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

static std::string CsvEscape(const std::string& value) {
    bool needsQuotes = value.find_first_of(",\"\n") != std::string::npos;
    if (!needsQuotes) {
        return value;
    }

    std::string escaped = "\"";
    for (char ch : value) {
        escaped += ch == '"' ? "\"\"" : std::string(1, ch);
    }
    escaped += '"';
    return escaped;
}

static std::string GetMoviesCsvPath() {
    if (FileExists("data/movies.csv")) {
        return "data/movies.csv";
    }

    if (FileExists("Cinema_Design/data/movies.csv") || DirectoryExists("Cinema_Design")) {
        return "Cinema_Design/data/movies.csv";
    }

    return "data/movies.csv";
}

static std::string ResolveAssetPath(const std::string& path) {
    const std::string candidates[] = {
        path,
        "Cinema_Design/" + path,
        "../" + path
    };

    for (const std::string& candidate : candidates) {
        if (FileExists(candidate.c_str())) {
            return candidate;
        }
    }

    return "";
}

static void DrawTextureCover(Texture2D texture, Rectangle dest) {
    float textureRatio = (float)texture.width / (float)texture.height;
    float destRatio = dest.width / dest.height;

    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    if (textureRatio > destRatio) {
        float sourceWidth = texture.height * destRatio;
        source.x = ((float)texture.width - sourceWidth) / 2.0f;
        source.width = sourceWidth;
    } else {
        float sourceHeight = texture.width / destRatio;
        source.y = ((float)texture.height - sourceHeight) / 2.0f;
        source.height = sourceHeight;
    }

    DrawTexturePro(texture, source, dest, { 0, 0 }, 0.0f, WHITE);
}

static void DrawAdminField(Rectangle bounds, const char* label, const std::string& value, bool active) {
    DrawMovieText(label, bounds.x + 4, bounds.y - 20, 15, SUBTEXT_COLOR);
    DrawRectangleRounded(bounds, 0.25f, 8, CARD_COLOR);
    DrawRectangleRoundedLines(bounds, 0.25f, 8, active ? ACCENT_COLOR : BORDER_COLOR);
    DrawMovieText(value, bounds.x + 12, bounds.y + 10, 17, value.empty() ? SUBTEXT_COLOR : TITLE_COLOR);
}

MoviesScreen::MoviesScreen(int w, int h)
    : screenWidth(w), screenHeight(h), scrollOffset(0.0f),
    typingSearch(false), postersLoaded(false), isAdmin(false),
    showingAddForm(false), activeAdminField(-1),
    selectedGenreIndex(0), selectedLanguageIndex(0), sortMode(0)
{
    searchBox = { 60, 130, 320, 38 };
    addMovieBtn = { (float)screenWidth - 570, 22, 118, 36 };
    saveMovieBtn = { 0, 0, 120, 40 };
    cancelAddBtn = { 0, 0, 100, 40 };
    LoadMovies();
    RebuildFilterOptions();
    ApplyMovieFilters();
}

void MoviesScreen::LoadMovies() {
    movies.clear();

    std::ifstream file(GetMoviesCsvPath());
    if (file.is_open()) {
        std::string line;
        bool isHeader = true;
        while (std::getline(file, line)) {
            if (isHeader) {
                isHeader = false;
                continue;
            }

            std::vector<std::string> values = SplitCsvLine(line);
            if (values.size() >= 7) {
                int duration = 0;
                try {
                    duration = std::stoi(values[6]);
                } catch (...) {
                    duration = 0;
                }

                movies.push_back({
                    values[0],
                    values[1],
                    values[2],
                    values[3],
                    values[4],
                    values[5],
                    duration
                });
            }
        }
    }

    if (!movies.empty()) {
        return;
    }

    movies = {
        { "Inception",       "English", "Sci-Fi",  "2010-07-16", "A thief enters dreams.",             "assets/posters/inception.png",        148 },
        { "Interstellar",    "English", "Sci-Fi",  "2014-11-07", "A team travels through a wormhole.", "assets/posters/interstellar.png",     169 },
        { "The Dark Knight", "English", "Action",  "2008-07-18", "Batman faces the Joker.",            "assets/posters/the-dark-knight.png",  152 },
        { "Avatar",         "English", "Fantasy", "2009-12-18", "A marine joins the Na'vi.",          "assets/posters/avatar.png",           162 },
        { "Titanic",        "English", "Romance", "1997-12-19", "A love story aboard Titanic.",       "assets/posters/titanic.png",          195 }
    };
    SaveMovies();
}

void MoviesScreen::SaveMovies() const {
    std::string path = GetMoviesCsvPath();
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path);
    if (!file.is_open()) {
        return;
    }

    file << "title,language,genre,release_date,description,poster_path,duration\n";
    for (const Movie& movie : movies) {
        file << CsvEscape(movie.title) << ','
             << CsvEscape(movie.language) << ','
             << CsvEscape(movie.genre) << ','
             << CsvEscape(movie.releaseDate) << ','
             << CsvEscape(movie.description) << ','
             << CsvEscape(movie.posterPath) << ','
             << movie.duration << '\n';
    }
}

void MoviesScreen::SetAdminMode(bool enabled) {
    isAdmin = enabled;
    showingAddForm = false;
    activeAdminField = -1;
    addMovieMessage.clear();
}

void MoviesScreen::LoadPosterTextures() {
    if (postersLoaded) {
        return;
    }

    posterTextures.clear();
    posterTextures.reserve(movies.size());

    for (const Movie& movie : movies) {
        std::string posterPath = ResolveAssetPath(movie.posterPath);
        if (!posterPath.empty()) {
            Texture2D texture = LoadTexture(posterPath.c_str());
            SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
            posterTextures.push_back(texture);
        } else {
            posterTextures.push_back(Texture2D{});
        }
    }

    postersLoaded = true;
}

void MoviesScreen::RebuildFilterOptions() {
    std::string selectedGenre = selectedGenreIndex >= 0 && selectedGenreIndex < (int)genreOptions.size()
        ? genreOptions[selectedGenreIndex]
        : "All";
    std::string selectedLanguage = selectedLanguageIndex >= 0 && selectedLanguageIndex < (int)languageOptions.size()
        ? languageOptions[selectedLanguageIndex]
        : "All";

    genreOptions.clear();
    languageOptions.clear();
    genreOptions.push_back("All");
    languageOptions.push_back("All");

    for (const Movie& movie : movies) {
        if (!movie.genre.empty() && !ContainsOption(genreOptions, movie.genre)) {
            genreOptions.push_back(movie.genre);
        }
        if (!movie.language.empty() && !ContainsOption(languageOptions, movie.language)) {
            languageOptions.push_back(movie.language);
        }
    }

    std::sort(genreOptions.begin() + 1, genreOptions.end());
    std::sort(languageOptions.begin() + 1, languageOptions.end());

    selectedGenreIndex = 0;
    selectedLanguageIndex = 0;
    for (int i = 0; i < (int)genreOptions.size(); i++) {
        if (genreOptions[i] == selectedGenre) {
            selectedGenreIndex = i;
        }
    }
    for (int i = 0; i < (int)languageOptions.size(); i++) {
        if (languageOptions[i] == selectedLanguage) {
            selectedLanguageIndex = i;
        }
    }
}

void MoviesScreen::ApplyMovieFilters() {
    filteredMovies.clear();
    std::string searchLower = toLower(searchText);
    std::string selectedGenre = selectedGenreIndex >= 0 && selectedGenreIndex < (int)genreOptions.size()
        ? genreOptions[selectedGenreIndex]
        : "All";
    std::string selectedLanguage = selectedLanguageIndex >= 0 && selectedLanguageIndex < (int)languageOptions.size()
        ? languageOptions[selectedLanguageIndex]
        : "All";

    for (const Movie& movie : movies) {
        bool matchesSearch = searchLower.empty();
        if (!matchesSearch) {
            std::string titleLower = toLower(movie.title);
            std::string languageLower = toLower(movie.language);
            std::string genreLower = toLower(movie.genre);
            std::string releaseLower = toLower(movie.releaseDate);
            matchesSearch =
                titleLower.find(searchLower) != std::string::npos ||
                languageLower.find(searchLower) != std::string::npos ||
                genreLower.find(searchLower) != std::string::npos ||
                releaseLower.find(searchLower) != std::string::npos;
        }

        bool matchesGenre = selectedGenre == "All" || movie.genre == selectedGenre;
        bool matchesLanguage = selectedLanguage == "All" || movie.language == selectedLanguage;
        if (matchesSearch && matchesGenre && matchesLanguage) {
            filteredMovies.push_back(movie);
        }
    }

    if (sortMode == 0) {
        std::sort(filteredMovies.begin(), filteredMovies.end(), [](const Movie& a, const Movie& b) {
            return toLower(a.title) < toLower(b.title);
        });
    } else if (sortMode == 1) {
        std::sort(filteredMovies.begin(), filteredMovies.end(), [](const Movie& a, const Movie& b) {
            return a.duration < b.duration;
        });
    } else {
        std::sort(filteredMovies.begin(), filteredMovies.end(), [](const Movie& a, const Movie& b) {
            return ExtractReleaseYear(a.releaseDate) > ExtractReleaseYear(b.releaseDate);
        });
    }
}

bool MoviesScreen::TryAddMovie() {
    int duration = 0;

    if (Trim(newTitle).empty() || Trim(newLanguage).empty() ||
        Trim(newGenre).empty() || Trim(newDuration).empty()) {
        addMovieMessage = "Fill title, language, genre and duration.";
        return false;
    }

    if (!TryParsePositiveInt(newDuration, duration)) {
        addMovieMessage = "Duration must be a positive number.";
        return false;
    }

    movies.push_back({
        Trim(newTitle),
        Trim(newLanguage),
        Trim(newGenre),
        Trim(newReleaseDate).empty() ? "TBA" : Trim(newReleaseDate),
        Trim(newDescription).empty() ? "No description yet." : Trim(newDescription),
        Trim(newPosterPath),
        duration
    });
    SaveMovies();
    RebuildFilterOptions();
    postersLoaded = false;
    posterTextures.clear();
    newTitle.clear();
    newLanguage.clear();
    newGenre.clear();
    newReleaseDate.clear();
    newDescription.clear();
    newDuration.clear();
    newPosterPath.clear();
    searchText.clear();
    ApplyMovieFilters();
    showingAddForm = false;
    activeAdminField = -1;
    addMovieMessage.clear();
    return true;
}

void MoviesScreen::Update(bool& goBack, bool& movieSelected) {
    goBack = false;
    movieSelected = false;

    Vector2 mouse = GetMousePosition();

    Rectangle searchArea = { (float)screenWidth - 430, 18, 220, 44 };
    Rectangle themeBtn = { (float)screenWidth - 190, 18, 58, 44 };
    Rectangle languageTopBtn = { (float)screenWidth - 118, 18, 58, 44 };
    Rectangle genreBtn = { 50.0f, 104.0f, 150.0f, 34.0f };
    Rectangle languageFilterBtn = { 214.0f, 104.0f, 170.0f, 34.0f };
    Rectangle sortBtn = { 398.0f, 104.0f, 170.0f, 34.0f };
    Rectangle clearFiltersBtn = { 582.0f, 104.0f, 92.0f, 34.0f };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, themeBtn)) {
            ToggleTheme();
            typingSearch = false;
            return;
        }

        if (CheckCollisionPointRec(mouse, languageTopBtn)) {
            ToggleLanguage();
            typingSearch = false;
            return;
        }

        if (isAdmin && CheckCollisionPointRec(mouse, addMovieBtn)) {
            showingAddForm = true;
            activeAdminField = 0;
            typingSearch = false;
            addMovieMessage.clear();
            return;
        }

        if (showingAddForm) {
            float formX = screenWidth / 2.0f - 260.0f;
            float formY = 120.0f;
            Rectangle fields[] = {
                { formX, formY + 70, 520, 38 },
                { formX, formY + 135, 250, 38 },
                { formX + 270, formY + 135, 250, 38 },
                { formX, formY + 200, 250, 38 },
                { formX + 270, formY + 200, 250, 38 },
                { formX, formY + 265, 520, 38 },
                { formX, formY + 330, 520, 38 }
            };
            saveMovieBtn = { formX + 290, formY + 390, 110, 40 };
            cancelAddBtn = { formX + 410, formY + 390, 110, 40 };

            activeAdminField = -1;
            for (int i = 0; i < 7; i++) {
                if (CheckCollisionPointRec(mouse, fields[i])) {
                    activeAdminField = i;
                }
            }

            if (CheckCollisionPointRec(mouse, cancelAddBtn)) {
                showingAddForm = false;
                activeAdminField = -1;
                addMovieMessage.clear();
            } else if (CheckCollisionPointRec(mouse, saveMovieBtn)) {
                TryAddMovie();
            }

            return;
        }

        if (!showingAddForm && CheckCollisionPointRec(mouse, genreBtn)) {
            selectedGenreIndex = genreOptions.empty() ? 0 : (selectedGenreIndex + 1) % (int)genreOptions.size();
            scrollOffset = 0.0f;
            ApplyMovieFilters();
            return;
        }

        if (!showingAddForm && CheckCollisionPointRec(mouse, languageFilterBtn)) {
            selectedLanguageIndex = languageOptions.empty() ? 0 : (selectedLanguageIndex + 1) % (int)languageOptions.size();
            scrollOffset = 0.0f;
            ApplyMovieFilters();
            return;
        }

        if (!showingAddForm && CheckCollisionPointRec(mouse, sortBtn)) {
            sortMode = (sortMode + 1) % 3;
            scrollOffset = 0.0f;
            ApplyMovieFilters();
            return;
        }

        if (!showingAddForm && CheckCollisionPointRec(mouse, clearFiltersBtn)) {
            selectedGenreIndex = 0;
            selectedLanguageIndex = 0;
            sortMode = 0;
            searchText.clear();
            typingSearch = false;
            scrollOffset = 0.0f;
            ApplyMovieFilters();
            return;
        }

        typingSearch = CheckCollisionPointRec(mouse, searchArea);
    }

    if (showingAddForm && activeAdminField >= 0) {
        std::string* activeValue = nullptr;
        if (activeAdminField == 0) activeValue = &newTitle;
        if (activeAdminField == 1) activeValue = &newLanguage;
        if (activeAdminField == 2) activeValue = &newGenre;
        if (activeAdminField == 3) activeValue = &newReleaseDate;
        if (activeAdminField == 4) activeValue = &newDuration;
        if (activeAdminField == 5) activeValue = &newPosterPath;
        if (activeAdminField == 6) activeValue = &newDescription;

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && activeValue) {
                activeValue->push_back((char)key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && activeValue && !activeValue->empty()) {
            activeValue->pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            TryAddMovie();
        }

        return;
    }

    if (showingAddForm) {
        return;
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
            scrollOffset = 0.0f;
            ApplyMovieFilters();
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && (!typingSearch || searchText.empty()))
        goBack = true;

    int y = 160 - (int)scrollOffset;
    auto& list = filteredMovies;

    for (auto& m : list) {
        Rectangle card = { 50, (float)y, (float)screenWidth - 100, (float)cardHeight };
        Rectangle deleteBtn = { card.x + card.width - 104, card.y + card.height - 42, 78, 28 };

        if (isAdmin && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(mouse, deleteBtn) && mouse.y > 90) {
            movies.erase(std::remove_if(movies.begin(), movies.end(), [&m](const Movie& movie) {
                return movie.title == m.title;
            }), movies.end());
            SaveMovies();
            RebuildFilterOptions();
            searchText.clear();
            ApplyMovieFilters();
            postersLoaded = false;
            posterTextures.clear();
            break;
        }

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
    ApplyMoviesTheme();
    LoadPosterTextures();

    int y = 160 - (int)scrollOffset;
    auto& list = filteredMovies;

    for (int i = 0; i < (int)list.size(); i++) {
        auto& m = list[i];
        Rectangle card = { 50, (float)y, (float)screenWidth - 100, (float)cardHeight };

        bool hovered = CheckCollisionPointRec(GetMousePosition(), card) && GetMousePosition().y > 90;

        DrawRectangleRounded(card, 0.15f, 8, hovered ? CARD_HOVER : CARD_COLOR);
        DrawRectangleRoundedLines(card, 0.15f, 8, hovered ? ACCENT_COLOR : BORDER_COLOR);

        Rectangle posterFrame = { card.x + 16, card.y + 14, 78, (float)cardHeight - 28 };
        DrawRectangleRounded(posterFrame, 0.18f, 6, hovered ? ACCENT_COLOR : BORDER_COLOR);

        Rectangle poster = { posterFrame.x + 2, posterFrame.y + 2, posterFrame.width - 4, posterFrame.height - 4 };
        auto movieIt = std::find_if(movies.begin(), movies.end(), [&m](const Movie& movie) {
            return movie.title == m.title;
        });
        int movieIndex = movieIt == movies.end() ? -1 : (int)std::distance(movies.begin(), movieIt);

        if (movieIndex >= 0 && movieIndex < (int)posterTextures.size() && posterTextures[movieIndex].id > 0) {
            DrawTextureCover(posterTextures[movieIndex], poster);
        } else {
            DrawRectangleRounded(poster, 0.18f, 6, hovered ? ACCENT_COLOR : PINK_ACC);
            std::string initial = m.title.substr(0, 1);
            Vector2 initialSize = MeasureMovieText(initial, 34);
            DrawMovieText(
                initial,
                poster.x + (poster.width - initialSize.x) / 2,
                poster.y + (poster.height - initialSize.y) / 2 - 2,
                34,
                DARK_BG
            );
        }

        float tx = card.x + 116;
        DrawMovieText(m.title, tx, card.y + 18, 26, TITLE_COLOR);

        std::string meta = m.genre + "  |  " + m.language + "  |  " + m.releaseDate;
        DrawMovieText(meta, tx, card.y + 56, 17, ACCENT_COLOR);

        DrawMovieText(m.description, tx, card.y + 84, 17, SUBTEXT_COLOR);

        std::string dur = std::to_string(m.duration) + " min";
        Vector2 durSize = MeasureMovieText(dur, 16);
        DrawMovieText(dur, card.x + card.width - durSize.x - 24, card.y + 18, 16, SUBTEXT_COLOR);

        if (hovered && !isAdmin) {
            const char* hint = TextFor("Click to book >");
            Vector2 hintSize = MeasureMovieText(hint, 15);
            DrawMovieText(hint, card.x + card.width - hintSize.x - 24, card.y + cardHeight - 32, 15, ACCENT_COLOR);
        }

        if (isAdmin) {
            Rectangle deleteBtn = { card.x + card.width - 104, card.y + card.height - 42, 78, 28 };
            bool deleteHover = CheckCollisionPointRec(GetMousePosition(), deleteBtn);
            DrawRectangleRounded(deleteBtn, 0.35f, 8, deleteHover ? GetTheme().dangerHover : GetTheme().danger);
            DrawMovieText(TextFor("DELETE"), deleteBtn.x + 8, deleteBtn.y + 7, 14, WHITE);
        }

        y += cardHeight + 16;
    }

    if (list.empty()) {
        const char* emptyText = TextFor("No movies match these filters.");
        Vector2 emptySize = MeasureMovieText(emptyText, 24);
        DrawMovieText(emptyText, (screenWidth - emptySize.x) / 2.0f, 260.0f, 24, SUBTEXT_COLOR);
    }

    DrawRectangle(0, 0, screenWidth, 150, DARK_BG);

    float lineAlpha = 60.0f;
    DrawLine(0, 90, screenWidth, 90, Color{0, 240, 255, (unsigned char)lineAlpha});
    DrawLine(0, 150, screenWidth, 150, Color{0, 240, 255, (unsigned char)(lineAlpha * 0.65f)});

    DrawMovieText(TextFor("MOVIES"), 50, 18, 34, TITLE_COLOR);
    DrawMovieText(TextFor(isAdmin ? "Admin mode: add or delete movies" : "Press BACKSPACE to return"), 50, 58, 15, SUBTEXT_COLOR);

    if (isAdmin) {
        bool addHover = CheckCollisionPointRec(GetMousePosition(), addMovieBtn);
        DrawRectangleRounded(addMovieBtn, 0.35f, 8, addHover ? Color{0, 200, 220, 255} : ACCENT_COLOR);
        DrawMovieText(TextFor("ADD MOVIE"), addMovieBtn.x + 13, addMovieBtn.y + 9, 16, DARK_BG);
    }

    Rectangle genreBtn = { 50.0f, 104.0f, 150.0f, 34.0f };
    Rectangle languageFilterBtn = { 214.0f, 104.0f, 170.0f, 34.0f };
    Rectangle sortBtn = { 398.0f, 104.0f, 170.0f, 34.0f };
    Rectangle clearFiltersBtn = { 582.0f, 104.0f, 92.0f, 34.0f };

    auto drawFilterButton = [](Rectangle bounds, const std::string& text, bool active) {
        bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
        DrawRectangleRounded(bounds, 0.35f, 8, hovered || active ? ACCENT_COLOR : CARD_COLOR);
        DrawRectangleRoundedLines(bounds, 0.35f, 8, hovered || active ? ACCENT_COLOR : BORDER_COLOR);
        Vector2 textSize = MeasureMovieText(text, 14);
        DrawMovieText(
            text,
            bounds.x + (bounds.width - textSize.x) / 2.0f,
            bounds.y + 9.0f,
            14,
            hovered || active ? DARK_BG : TITLE_COLOR
        );
    };

    std::string genreValue = selectedGenreIndex >= 0 && selectedGenreIndex < (int)genreOptions.size()
        ? genreOptions[selectedGenreIndex]
        : "All";
    std::string languageValue = selectedLanguageIndex >= 0 && selectedLanguageIndex < (int)languageOptions.size()
        ? languageOptions[selectedLanguageIndex]
        : "All";
    const char* sortLabels[] = { "Title", "Duration", "Newest" };
    drawFilterButton(genreBtn, std::string(TextFor("Genre: ")) + (genreValue == "All" ? TextFor("All") : genreValue), selectedGenreIndex > 0);
    drawFilterButton(languageFilterBtn, std::string(TextFor("Language: ")) + (languageValue == "All" ? TextFor("All") : languageValue), selectedLanguageIndex > 0);
    drawFilterButton(sortBtn, std::string(TextFor("Sort: ")) + TextFor(sortLabels[sortMode]), sortMode != 0);
    drawFilterButton(clearFiltersBtn, TextFor("Clear"), selectedGenreIndex > 0 || selectedLanguageIndex > 0 || sortMode != 0 || !searchText.empty());

    Rectangle searchArea = { (float)screenWidth - 430, 18, 220, 44 };
    DrawRectangleRounded(searchArea, 0.5f, 8, CARD_COLOR);
    DrawRectangleRoundedLines(searchArea, 0.5f, 8, typingSearch ? ACCENT_COLOR : BORDER_COLOR);

    DrawMovieText("Q", searchArea.x + 14, searchArea.y + 10, 20, SUBTEXT_COLOR);

    if (searchText.empty() && !typingSearch) {
        DrawMovieText(TextFor("Search movies..."), searchArea.x + 42, searchArea.y + 12, 18, SUBTEXT_COLOR);
    } else {
        DrawMovieText(searchText, searchArea.x + 42, searchArea.y + 12, 18, TITLE_COLOR);
    }

    Rectangle themeBtn = { (float)screenWidth - 190, 18, 58, 44 };
    bool themeHover = CheckCollisionPointRec(GetMousePosition(), themeBtn);
    DrawRectangleRounded(themeBtn, 0.35f, 8, themeHover ? ACCENT_COLOR : CARD_COLOR);
    DrawRectangleRoundedLines(themeBtn, 0.35f, 8, themeHover ? ACCENT_COLOR : BORDER_COLOR);
    const char* themeText = TextFor(IsLightTheme() ? "DARK" : "LIGHT");
    Vector2 themeTextSize = MeasureMovieText(themeText, 13);
    DrawMovieText(
        themeText,
        themeBtn.x + (themeBtn.width - themeTextSize.x) / 2,
        themeBtn.y + 14,
        13,
        themeHover ? DARK_BG : TITLE_COLOR
    );

    Rectangle languageTopBtn = { (float)screenWidth - 118, 18, 58, 44 };
    bool languageHover = CheckCollisionPointRec(GetMousePosition(), languageTopBtn);
    DrawRectangleRounded(languageTopBtn, 0.35f, 8, languageHover ? ACCENT_COLOR : CARD_COLOR);
    DrawRectangleRoundedLines(languageTopBtn, 0.35f, 8, languageHover ? ACCENT_COLOR : BORDER_COLOR);
    const char* languageText = IsBulgarian() ? "EN" : "BG";
    Vector2 languageTextSize = MeasureMovieText(languageText, 13);
    DrawMovieText(
        languageText,
        languageTopBtn.x + (languageTopBtn.width - languageTextSize.x) / 2,
        languageTopBtn.y + 14,
        13,
        languageHover ? DARK_BG : TITLE_COLOR
    );

    if (showingAddForm) {
        Color overlay = IsLightTheme() ? Color{245, 247, 251, 230} : Color{15, 23, 42, 230};
        DrawRectangle(0, 90, screenWidth, screenHeight - 90, overlay);

        float formX = screenWidth / 2.0f - 260.0f;
        float formY = 120.0f;
        Rectangle form = { formX - 30, formY - 30, 580, 500 };
        DrawRectangleRounded(form, 0.06f, 8, GetTheme().cardBg);
        DrawRectangleRoundedLines(form, 0.06f, 8, BORDER_COLOR);

        DrawMovieText(TextFor("ADD MOVIE"), formX, formY - 6, 28, TITLE_COLOR);
        DrawMovieText(TextFor("Required: title, language, genre, duration"), formX, formY + 28, 15, SUBTEXT_COLOR);

        Rectangle fields[] = {
            { formX, formY + 70, 520, 38 },
            { formX, formY + 135, 250, 38 },
            { formX + 270, formY + 135, 250, 38 },
            { formX, formY + 200, 250, 38 },
            { formX + 270, formY + 200, 250, 38 },
            { formX, formY + 265, 520, 38 },
            { formX, formY + 330, 520, 38 }
        };

        DrawAdminField(fields[0], TextFor("Title"), newTitle, activeAdminField == 0);
        DrawAdminField(fields[1], TextFor("Language"), newLanguage, activeAdminField == 1);
        DrawAdminField(fields[2], TextFor("Genre"), newGenre, activeAdminField == 2);
        DrawAdminField(fields[3], "Release date", newReleaseDate, activeAdminField == 3);
        DrawAdminField(fields[4], "Duration", newDuration, activeAdminField == 4);
        DrawAdminField(fields[5], "Poster path", newPosterPath, activeAdminField == 5);
        DrawAdminField(fields[6], "Description", newDescription, activeAdminField == 6);

        saveMovieBtn = { formX + 290, formY + 390, 110, 40 };
        cancelAddBtn = { formX + 410, formY + 390, 110, 40 };

        bool canSave = !Trim(newTitle).empty() && !Trim(newLanguage).empty() &&
            !Trim(newGenre).empty() && !Trim(newDuration).empty();
        DrawRectangleRounded(saveMovieBtn, 0.35f, 8, canSave ? ACCENT_COLOR : BORDER_COLOR);
        DrawMovieText(TextFor("ADD"), saveMovieBtn.x + 24, saveMovieBtn.y + 11, 17, canSave ? DARK_BG : SUBTEXT_COLOR);

        bool cancelHover = CheckCollisionPointRec(GetMousePosition(), cancelAddBtn);
        DrawRectangleRounded(cancelAddBtn, 0.35f, 8, cancelHover ? GetTheme().danger : CARD_COLOR);
        DrawRectangleRoundedLines(cancelAddBtn, 0.35f, 8, cancelHover ? GetTheme().danger : BORDER_COLOR);
        DrawMovieText(TextFor("CANCEL"), cancelAddBtn.x + 14, cancelAddBtn.y + 11, 17, cancelHover ? WHITE : TITLE_COLOR);

        if (!addMovieMessage.empty()) {
            DrawMovieText(TextFor(addMovieMessage.c_str()), formX, formY + 448, 15, PINK_ACC);
        }
    }
}
