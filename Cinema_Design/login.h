#ifndef LOGIN_H
#define LOGIN_H

#include "raylib.h"
#include <string>

enum class LoginState { SCREEN_MAIN, SCREEN_ADMIN_LOGIN, SCREEN_ADMIN_MENU, SCREEN_USER_MENU };

class LoginSystem {
public:
    LoginSystem();
    void Update();
    void Draw();

    bool IsUserLoggedIn() const { return userLoggedIn; }
    bool ShouldCloseApp() const { return shouldClose; }
    void ResetLoginStatus() { userLoggedIn = false; }

private:
    bool IsButtonClicked(Rectangle rect);

    LoginState currentScreen;
    std::string passwordInput;
    bool passwordWrong;
    bool userLoggedIn;
    bool shouldClose;

    Rectangle btnAdmin;
    Rectangle btnUser;
    Rectangle btnExit;
    Rectangle btnBack;
    Rectangle btnSchedule;
    Rectangle btnReserve;
    Rectangle btnBuy;
};

#endif
