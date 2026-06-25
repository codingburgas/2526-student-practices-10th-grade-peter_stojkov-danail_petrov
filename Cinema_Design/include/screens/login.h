#pragma once
#include "raylib.h"
#include <string>

class LoginScreen {
public:
    LoginScreen(int w, int h);

    void Update(bool& loggedIn);
    void Draw();
    void Reset();
    const std::string& GetUsername() const;
    bool IsAdmin() const;

private:
    bool LoginUser();
    bool RegisterUser();
    bool UserExists(const std::string& name) const;

    int screenWidth;
    int screenHeight;

    Rectangle userBox;
    Rectangle passBox;
    Rectangle loginBtn;
    Rectangle registerBtn;

    bool typingUser;
    bool typingPass;
    bool isAdmin;

    std::string username;
    std::string password;
    std::string statusMessage;
    Color statusColor;
};
