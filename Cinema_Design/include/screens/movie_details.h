#pragma once
#include "raylib.h"
#include <string>

struct MovieDetailsData {
    std::string title;
    std::string language;
    std::string genre;
    std::string releaseDate;
    std::string description;
    std::string posterPath;
    int duration;
};

class MovieDetailsScreen {
public:
    MovieDetailsScreen(int screenWidth, int screenHeight);

    void SetMovie(const std::string& title);
    void Update(bool& goBack, bool& bookTickets);
    void Draw();

    const std::string& GetMovieTitle() const;

private:
    bool LoadMovieFromCsv(const std::string& title);
    void LoadPoster();

    int screenWidth;
    int screenHeight;
    MovieDetailsData movie;
    Texture2D posterTexture;
    bool posterLoaded;

    Rectangle backBtn;
    Rectangle bookBtn;
};
