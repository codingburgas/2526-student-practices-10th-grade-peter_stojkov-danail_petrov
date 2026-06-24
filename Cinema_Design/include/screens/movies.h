#pragma once
#include <string>
#include <vector>
#include "raylib.h"

struct Movie {
    std::string title;
    std::string language;
    std::string genre;
    std::string releaseDate;
    std::string description;
    int duration;
};

class MoviesScreen {
public:
    MoviesScreen(int w, int h);
    void LoadMovies();
    void Update(bool& goBack, bool& movieSelected);
    void Draw();
    const std::string& GetSelectedMovieTitle() const;

private:
    int screenWidth;
    int screenHeight;

    std::vector<Movie> movies;
    std::vector<Movie> filteredMovies;

    std::string searchText;
    std::string selectedMovieTitle;

    bool typingSearch;
    Rectangle searchBox;

    float scrollOffset;
};
