#include "login.h"
#include "language.h"
#include "theme.h"
#include "ui_font.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#define DrawText DrawAppText
#define MeasureText MeasureAppText

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_BG       = { 22, 33, 62, 240 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TITLE_COLOR   = { 248, 250, 252, 255 };
static Color SUBTEXT_COLOR = { 148, 163, 184, 255 };
static Color ACCENT_COLOR  = { 0, 240, 255, 255 };
static Color ACCENT_DIM    = { 0, 180, 200, 255 };
static Color BUTTON_HOVER  = { 0, 200, 220, 255 };
static Color PINK_ACCENT   = { 255, 42, 109, 255 };
static Color ERROR_COLOR   = { 255, 100, 100, 255 };
static Color SUCCESS_COLOR = { 0, 220, 100, 255 };

static void ApplyLoginTheme() {
    const ThemePalette& theme = GetTheme();
    DARK_BG = theme.background;
    CARD_BG = theme.cardBg;
    CARD_COLOR = theme.card;
    BORDER_COLOR = theme.border;
    TITLE_COLOR = theme.text;
    SUBTEXT_COLOR = theme.textDim;
    ACCENT_COLOR = theme.accent;
    ACCENT_DIM = theme.accentDim;
    BUTTON_HOVER = theme.accentHover;
    PINK_ACCENT = theme.pink;
    ERROR_COLOR = theme.dangerHover;
    SUCCESS_COLOR = theme.success;
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
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped += ch;
        }
    }
    escaped += '"';
    return escaped;
}

LoginScreen::LoginScreen(int w, int h)
    : screenWidth(w), screenHeight(h),
      typingUser(false), typingPass(false),
      isAdmin(false),
      statusColor(SUBTEXT_COLOR)
{
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    userBox  = { cx - 160, cy - 42, 320, 44 };
    passBox  = { cx - 160, cy + 32, 320, 44 };
    loginBtn = { cx - 160, cy + 118, 150, 48 };
    registerBtn = { cx + 10, cy + 118, 150, 48 };
}

void LoginScreen::Update(bool& loggedIn) {
    loggedIn = false;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        typingUser = CheckCollisionPointRec(mouse, userBox);
        typingPass = CheckCollisionPointRec(mouse, passBox);
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125) {
            if (typingUser) username.push_back((char)key);
            if (typingPass) password.push_back((char)key);
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (typingUser && !username.empty()) username.pop_back();
        if (typingPass && !password.empty()) password.pop_back();
    }

    bool buttonHovered = CheckCollisionPointRec(mouse, loginBtn);
    bool registerHovered = CheckCollisionPointRec(mouse, registerBtn);
    if (buttonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (LoginUser()) {
            loggedIn = true;
        }
    }

    if (registerHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (RegisterUser()) {
            loggedIn = true;
        }
    }
}

bool LoginScreen::LoginUser() {
    isAdmin = false;

    if (username.empty() || password.empty()) {
        statusMessage = "Enter username and password.";
        statusColor = ERROR_COLOR;
        return false;
    }

    std::ifstream file("data/users.csv");
    if (!file.is_open()) {
        statusMessage = "No users yet. Register first.";
        statusColor = ERROR_COLOR;
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
        if (values.size() >= 2 && values[0] == username && values[1] == password) {
            isAdmin = values.size() >= 3 && values[2] == "1";
            statusMessage = "Login successful.";
            statusColor = SUCCESS_COLOR;
            return true;
        }
    }

    statusMessage = "Invalid username or password.";
    statusColor = ERROR_COLOR;
    return false;
}

bool LoginScreen::RegisterUser() {
    isAdmin = false;

    if (username.empty() || password.empty()) {
        statusMessage = "Enter username and password.";
        statusColor = ERROR_COLOR;
        return false;
    }

    if (UserExists(username)) {
        statusMessage = "Username already exists.";
        statusColor = ERROR_COLOR;
        return false;
    }

    std::filesystem::create_directories("data");
    const char* path = "data/users.csv";
    std::ifstream existing(path);
    bool needsHeader = !existing.good() || existing.peek() == std::ifstream::traits_type::eof();
    existing.close();

    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        statusMessage = "Could not save user.";
        statusColor = ERROR_COLOR;
        return false;
    }

    if (needsHeader) {
        file << "username,password,is_admin\n";
    }

    file << CsvEscape(username) << ',' << CsvEscape(password) << ",0\n";
    statusMessage = "Registered successfully.";
    statusColor = SUCCESS_COLOR;
    return true;
}

bool LoginScreen::UserExists(const std::string& name) const {
    std::ifstream file("data/users.csv");
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
        if (!values.empty() && values[0] == name) {
            return true;
        }
    }

    return false;
}

void LoginScreen::Draw() {
    ApplyLoginTheme();

    float cx = screenWidth / 2.0f;
    float cy = screenHeight / 2.0f;

    for (int i = 0; i < 3; i++) {
        float alpha = IsLightTheme() ? 16.0f - i * 4.0f : 30.0f - i * 10.0f;
        DrawCircle((int)cx, (int)cy, 280.0f + i * 40.0f,
            Color{ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, (unsigned char)alpha});
    }

    Rectangle cardRect = { cx - 200, cy - 175, 400, 390 };
    DrawRectangleRounded(cardRect, 0.08f, 8, CARD_BG);
    DrawRectangleRoundedLines(cardRect, 0.08f, 8, BORDER_COLOR);

    const char* logo = TextFor("CINEMA");
    int logoW = MeasureText(logo, 36);
    DrawText(logo, (int)(cx - logoW / 2), (int)(cardRect.y + 30), 36, ACCENT_COLOR);

    const char* sub = TextFor("Sign in to continue");
    int subW = MeasureText(sub, 18);
    DrawText(sub, (int)(cx - subW / 2), (int)(cardRect.y + 80), 18, SUBTEXT_COLOR);

    DrawText(TextFor("Username"), (int)(userBox.x + 5), (int)(userBox.y - 22), 16, SUBTEXT_COLOR);
    DrawRectangleRounded(userBox, 0.3f, 8, CARD_COLOR);
    DrawRectangleRoundedLines(userBox, 0.3f, 8, typingUser ? ACCENT_COLOR : BORDER_COLOR);
    DrawText(username.c_str(), (int)(userBox.x + 14), (int)(userBox.y + 13), 20, TITLE_COLOR);

    if (typingUser && username.empty()) {
        DrawText(TextFor("Enter username"), (int)(userBox.x + 14), (int)(userBox.y + 13), 20, BORDER_COLOR);
    }

    DrawText(TextFor("Password"), (int)(passBox.x + 5), (int)(passBox.y - 22), 16, SUBTEXT_COLOR);
    DrawRectangleRounded(passBox, 0.3f, 8, CARD_COLOR);
    DrawRectangleRoundedLines(passBox, 0.3f, 8, typingPass ? ACCENT_COLOR : BORDER_COLOR);
    std::string masked(password.size(), '*');
    DrawText(masked.c_str(), (int)(passBox.x + 14), (int)(passBox.y + 13), 20, TITLE_COLOR);

    if (typingPass && password.empty()) {
        DrawText(TextFor("Enter password"), (int)(passBox.x + 14), (int)(passBox.y + 13), 20, BORDER_COLOR);
    }

    bool buttonHovered = CheckCollisionPointRec(GetMousePosition(), loginBtn);
    bool registerHovered = CheckCollisionPointRec(GetMousePosition(), registerBtn);
    bool canLogin = !username.empty() && !password.empty();
    Color btnCol = canLogin ? (buttonHovered ? BUTTON_HOVER : ACCENT_COLOR) : BORDER_COLOR;
    DrawRectangleRounded(loginBtn, 0.5f, 8, btnCol);

    const char* btnText = TextFor("LOGIN");
    int btnW = MeasureText(btnText, 22);
    DrawText(btnText, (int)(loginBtn.x + (loginBtn.width - btnW) / 2), (int)(loginBtn.y + 14), 22, DARK_BG);

    Color regCol = canLogin ? (registerHovered ? PINK_ACCENT : Color{255, 80, 130, 255}) : BORDER_COLOR;
    DrawRectangleRounded(registerBtn, 0.5f, 8, regCol);

    const char* regText = TextFor("REGISTER");
    int regW = MeasureText(regText, 20);
    DrawText(regText, (int)(registerBtn.x + (registerBtn.width - regW) / 2), (int)(registerBtn.y + 15), 20, DARK_BG);

    if (!statusMessage.empty()) {
        const char* statusText = TextFor(statusMessage.c_str());
        int msgW = MeasureText(statusText, 16);
        DrawText(statusText, (int)(cx - msgW / 2), (int)(loginBtn.y + 65), 16, statusColor);
    }
}

void LoginScreen::Reset() {
    username.clear();
    password.clear();
    statusMessage.clear();
    typingUser = false;
    typingPass = false;
    isAdmin = false;
}

const std::string& LoginScreen::GetUsername() const {
    return username;
}

bool LoginScreen::IsAdmin() const {
    return isAdmin;
}
