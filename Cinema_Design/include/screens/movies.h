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
    std::string posterPath;
    int duration;
};

class MoviesScreen {
public:
    MoviesScreen(int w, int h);
    void LoadMovies();
    void SaveMovies() const;
    void LoadPosterTextures();
    void SetAdminMode(bool enabled);
    void Update(bool& goBack, bool& movieSelected);
    void Draw();
    const std::string& GetSelectedMovieTitle() const;

private:
    bool TryAddMovie();

    int screenWidth;
    int screenHeight;

    std::vector<Movie> movies;
    std::vector<Movie> filteredMovies;
    std::vector<Texture2D> posterTextures;

    std::string searchText;
    std::string selectedMovieTitle;
    std::string newTitle;
    std::string newLanguage;
    std::string newGenre;
    std::string newReleaseDate;
    std::string newDescription;
    std::string newDuration;
    std::string newPosterPath;
    std::string addMovieMessage;

    bool typingSearch;
    bool postersLoaded;
    bool isAdmin;
    bool showingAddForm;
    int activeAdminField;
    Rectangle searchBox;
    Rectangle addMovieBtn;
    Rectangle saveMovieBtn;
    Rectangle cancelAddBtn;

    float scrollOffset;
};
