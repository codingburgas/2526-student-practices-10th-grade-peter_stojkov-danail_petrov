#pragma once
#include <cstring>

enum class AppLanguage {
    English,
    Bulgarian
};

inline AppLanguage currentLanguage = AppLanguage::English;

inline bool IsBulgarian() {
    return currentLanguage == AppLanguage::Bulgarian;
}

inline void ToggleLanguage() {
    currentLanguage = IsBulgarian() ? AppLanguage::English : AppLanguage::Bulgarian;
}

inline const char* TextFor(const char* key) {
    if (!IsBulgarian()) {
        return key;
    }

#define TXT(en, bg) if (std::strcmp(key, en) == 0) return bg
    TXT("CINEMA", "КИНО");
    TXT("DASHBOARD", "ТАБЛО");
    TXT("BROWSE MOVIES", "ФИЛМИ");
    TXT("MY BOOKINGS", "МОИТЕ РЕЗЕРВАЦИИ");
    TXT("EXIT", "ИЗХОД");
    TXT("LOG OUT", "ИЗХОД ОТ ПРОФИЛ");
    TXT("LIGHT", "СВЕТЛА");
    TXT("DARK", "ТЪМНА");
    TXT("Sign in to continue", "Влезте, за да продължите");
    TXT("Username", "Потребител");
    TXT("Password", "Парола");
    TXT("Enter username", "Въведете потребител");
    TXT("Enter password", "Въведете парола");
    TXT("LOGIN", "ВХОД");
    TXT("REGISTER", "РЕГИСТРАЦИЯ");
    TXT("Enter username and password.", "Въведете потребител и парола.");
    TXT("No users yet. Register first.", "Няма потребители. Регистрирайте се.");
    TXT("Login successful.", "Успешен вход.");
    TXT("Invalid username or password.", "Грешен потребител или парола.");
    TXT("Username already exists.", "Потребителят вече съществува.");
    TXT("Could not save user.", "Потребителят не беше записан.");
    TXT("Registered successfully.", "Успешна регистрация.");
    TXT("MOVIES", "ФИЛМИ");
    TXT("Admin mode: add or delete movies", "Админ режим: добавяне и триене");
    TXT("Press BACKSPACE to return", "BACKSPACE за връщане");
    TXT("ADD MOVIE", "ДОБАВИ");
    TXT("Search movies...", "Търси филми...");
    TXT("Genre: ", "Жанр: ");
    TXT("Language: ", "Език: ");
    TXT("Sort: ", "Сортиране: ");
    TXT("Title", "Име");
    TXT("Duration", "Време");
    TXT("Newest", "Нови");
    TXT("Clear", "Изчисти");
    TXT("All", "Всички");
    TXT("No movies match these filters.", "Няма филми с тези филтри.");
    TXT("Click to book >", "Към резервация >");
    TXT("DELETE", "ИЗТРИЙ");
    TXT("ADD", "ДОБАВИ");
    TXT("CANCEL", "ОТКАЗ");
    TXT("Required: title, language, genre, duration", "Задължителни: име, език, жанр, време");
    TXT("Fill title, language, genre and duration.", "Попълнете име, език, жанр и време.");
    TXT("Duration must be a positive number.", "Времето трябва да е положително число.");
    TXT("MOVIE DETAILS", "ДЕТАЙЛИ ЗА ФИЛМА");
    TXT("Review the movie, then book tickets.", "Прегледайте филма и резервирайте.");
    TXT("BACK", "НАЗАД");
    TXT("Description", "Описание");
    TXT("Title", "Име");
    TXT("Genre", "Жанр");
    TXT("Language", "Език");
    TXT("BOOK TICKETS", "РЕЗЕРВИРАЙ");
    TXT("BACKSPACE to return", "BACKSPACE за връщане");
    TXT("No bookings yet.", "Няма резервации.");
    TXT("Seats: ", "Места: ");
    TXT("Credit card", "Карта");
    TXT("Cash", "В брой");
    TXT("Online", "Онлайн");
    TXT("Walk-in", "На каса");
    TXT("SHOWTIMES", "ПРОЖЕКЦИИ");
    TXT("ONLINE", "ОНЛАЙН");
    TXT("WALK-IN", "НА КАСА");
    TXT("Choose a booking type, then select a cinema hall and showtime.", "Изберете тип резервация, кино, зала и час.");
    TXT("Select >", "Избери >");
    TXT("SELECT YOUR SEATS", "ИЗБЕРЕТЕ МЕСТА");
    TXT("SCREEN", "ЕКРАН");
    TXT("Selected", "Избрани");
    TXT("Taken", "Заети");
    TXT("CONFIRM BOOKING", "ПОТВЪРДИ");
    TXT("BACKSPACE to go back", "BACKSPACE за назад");
    TXT("CHECKOUT", "ПЛАЩАНЕ");
    TXT("Online booking: credit card only", "Онлайн: само с карта");
    TXT("Walk-in booking: cash or credit card", "На каса: в брой или карта");
    TXT("CREDIT CARD (Online)", "КАРТА (Онлайн)");
    TXT("CASH (On-site)", "В БРОЙ (На място)");
    TXT("Payment Successful!", "Успешно плащане!");
    TXT("Returning to menu...", "Връщане към менюто...");
#undef TXT

    return key;
}
