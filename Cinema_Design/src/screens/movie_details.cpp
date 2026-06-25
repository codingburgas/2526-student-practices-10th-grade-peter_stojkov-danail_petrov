#include "movie_details.h"
#include "language.h"
#include "theme.h"
#include "ui_font.h"
#include <fstream>
#include <string>
#include <vector>

#define DrawText DrawAppText
#define MeasureText MeasureAppText

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

MovieDetailsScreen::MovieDetailsScreen(int w, int h)
    : screenWidth(w), screenHeight(h), posterTexture{}, posterLoaded(false)
{
    backBtn = { 50.0f, 28.0f, 92.0f, 36.0f };
    bookBtn = { (float)screenWidth - 250.0f, (float)screenHeight - 92.0f, 200.0f, 50.0f };
}

void MovieDetailsScreen::SetMovie(const std::string& title) {
    if (posterTexture.id > 0) {
        UnloadTexture(posterTexture);
        posterTexture = Texture2D{};
    }

    posterLoaded = false;
    movie = {};
    movie.title = title;
    movie.releaseDate = "TBA";
    movie.description = "No description yet.";
    movie.duration = 0;

    LoadMovieFromCsv(title);
}

bool MovieDetailsScreen::LoadMovieFromCsv(const std::string& title) {
    std::ifstream file(GetMoviesCsvPath());
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }

        std::vector<std::string> values = SplitCsvLine(line);
        if (values.size() >= 7 && values[0] == title) {
            movie.title = values[0];
            movie.language = values[1];
            movie.genre = values[2];
            movie.releaseDate = values[3];
            movie.description = values[4];
            movie.posterPath = values[5];
            try {
                movie.duration = std::stoi(values[6]);
            } catch (...) {
                movie.duration = 0;
            }
            return true;
        }
    }

    return false;
}

void MovieDetailsScreen::LoadPoster() {
    if (posterLoaded) {
        return;
    }

    posterLoaded = true;
    std::string posterPath = ResolveAssetPath(movie.posterPath);
    if (!posterPath.empty()) {
        posterTexture = LoadTexture(posterPath.c_str());
        SetTextureFilter(posterTexture, TEXTURE_FILTER_BILINEAR);
    }
}

void MovieDetailsScreen::Update(bool& goBack, bool& bookTickets) {
    goBack = false;
    bookTickets = false;

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, backBtn)) {
            goBack = true;
        } else if (CheckCollisionPointRec(mouse, bookBtn)) {
            bookTickets = true;
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        goBack = true;
    }
}

void MovieDetailsScreen::Draw() {
    const ThemePalette& theme = GetTheme();
    LoadPoster();

    Vector2 mouse = GetMousePosition();
    bool backHover = CheckCollisionPointRec(mouse, backBtn);
    bool bookHover = CheckCollisionPointRec(mouse, bookBtn);

    DrawRectangleRounded(backBtn, 0.35f, 8, backHover ? theme.cardHover : theme.card);
    DrawRectangleRoundedLines(backBtn, 0.35f, 8, backHover ? theme.accent : theme.border);
    DrawText(TextFor("BACK"), (int)backBtn.x + 18, (int)backBtn.y + 10, 15, theme.text);

    DrawText(TextFor("MOVIE DETAILS"), 170, 28, 30, theme.text);
    DrawText(TextFor("Review the movie, then book tickets."), 172, 62, 15, theme.textDim);

    Rectangle posterFrame = { 70.0f, 130.0f, 260.0f, 380.0f };
    DrawRectangleRounded(posterFrame, 0.04f, 8, theme.border);
    Rectangle poster = { posterFrame.x + 3, posterFrame.y + 3, posterFrame.width - 6, posterFrame.height - 6 };

    if (posterTexture.id > 0) {
        DrawTextureCover(posterTexture, poster);
    } else {
        DrawRectangleRounded(poster, 0.04f, 8, theme.pink);
        std::string initial = movie.title.empty() ? "?" : movie.title.substr(0, 1);
        int iw = MeasureText(initial.c_str(), 72);
        DrawText(initial.c_str(), (int)(poster.x + (poster.width - iw) / 2), (int)(poster.y + 145), 72, theme.background);
    }

    float infoX = 380.0f;
    DrawText(movie.title.c_str(), (int)infoX, 145, 42, theme.text);

    std::string meta = movie.genre + "  |  " + movie.language + "  |  " + movie.releaseDate;
    if (movie.duration > 0) {
        meta += "  |  " + std::to_string(movie.duration) + " min";
    }
    DrawText(meta.c_str(), (int)infoX, 205, 19, theme.accent);

    DrawText(TextFor("Description"), (int)infoX, 270, 22, theme.text);
    DrawText(movie.description.c_str(), (int)infoX, 308, 20, theme.textDim);

    Rectangle detailCard = { infoX, 380.0f, (float)screenWidth - infoX - 70.0f, 120.0f };
    DrawRectangleRounded(detailCard, 0.08f, 8, theme.card);
    DrawRectangleRoundedLines(detailCard, 0.08f, 8, theme.border);
    DrawText(TextFor("Genre"), (int)detailCard.x + 24, (int)detailCard.y + 24, 16, theme.textDim);
    DrawText(movie.genre.c_str(), (int)detailCard.x + 24, (int)detailCard.y + 54, 22, theme.text);
    DrawText(TextFor("Language"), (int)detailCard.x + 210, (int)detailCard.y + 24, 16, theme.textDim);
    DrawText(movie.language.c_str(), (int)detailCard.x + 210, (int)detailCard.y + 54, 22, theme.text);
    DrawText(TextFor("Duration"), (int)detailCard.x + 420, (int)detailCard.y + 24, 16, theme.textDim);
    std::string duration = movie.duration > 0 ? std::to_string(movie.duration) + " min" : "TBA";
    DrawText(duration.c_str(), (int)detailCard.x + 420, (int)detailCard.y + 54, 22, theme.text);

    DrawRectangleRounded(bookBtn, 0.35f, 8, bookHover ? theme.accentHover : theme.accent);
    const char* bookText = TextFor("BOOK TICKETS");
    int bw = MeasureText(bookText, 20);
    DrawText(bookText, (int)(bookBtn.x + (bookBtn.width - bw) / 2), (int)bookBtn.y + 15, 20, theme.background);
}

const std::string& MovieDetailsScreen::GetMovieTitle() const {
    return movie.title;
}
