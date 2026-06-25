#include "payment.h"
#include "theme.h"
#include "ui_font.h"
#include <string>

#define DrawText DrawAppText
#define MeasureText MeasureAppText

static Color DARK_BG       = { 15, 23, 42, 255 };
static Color CARD_BG       = { 22, 33, 62, 240 };
static Color CARD_COLOR    = { 30, 41, 59, 255 };
static Color BORDER_COLOR  = { 51, 65, 85, 255 };
static Color TEXT_LIGHT    = { 248, 250, 252, 255 };
static Color TEXT_DIM      = { 148, 163, 184, 255 };
static Color CYAN_ACC      = { 0, 240, 255, 255 };
static Color CYAN_HOVER    = { 0, 200, 220, 255 };
static Color PINK_ACC      = { 255, 42, 109, 255 };
static Color PINK_HOVER    = { 255, 80, 130, 255 };
static Color GREEN_ACC     = { 0, 220, 100, 255 };

static void ApplyPaymentTheme() {
    const ThemePalette& theme = GetTheme();
    DARK_BG = theme.background;
    CARD_BG = theme.cardBg;
    CARD_COLOR = theme.card;
    BORDER_COLOR = theme.border;
    TEXT_LIGHT = theme.text;
    TEXT_DIM = theme.textDim;
    CYAN_ACC = theme.accent;
    CYAN_HOVER = theme.accentHover;
    PINK_ACC = theme.pink;
    PINK_HOVER = theme.pinkHover;
    GREEN_ACC = theme.success;
}

PaymentScreen::PaymentScreen(int w, int h)
    : screenWidth(w), screenHeight(h), totalAmount(0.0f),
    bookingChannel(BookingChannel::Online), paymentSuccess(false), successTimer(0)
{
    float cx = (float)w / 2.0f;
    btnCreditCard = { cx - 130, 300, 260, 52 };
    btnCash       = { cx - 130, 370, 260, 52 };
    btnCancel     = { cx - 130, 450, 260, 48 };

    hoverCC = false;
    hoverCash = false;
    hoverCancel = false;
}

void PaymentScreen::SetTotalAmount(float amount) {
    totalAmount = amount;
    paymentSuccess = false;
    successTimer = 0;
}

void PaymentScreen::SetBookingChannel(BookingChannel channel) {
    bookingChannel = channel;
}

void PaymentScreen::Update(PaymentResult& result) {
    result = PaymentResult::NONE;

    if (paymentSuccess) {
        successTimer++;
        if (successTimer > 120) {
            result = PaymentResult::DONE;
            paymentSuccess = false;
        }
        return;
    }

    Vector2 mouse = GetMousePosition();

    hoverCC = CheckCollisionPointRec(mouse, btnCreditCard);
    hoverCash = bookingChannel == BookingChannel::WalkIn && CheckCollisionPointRec(mouse, btnCash);
    hoverCancel = CheckCollisionPointRec(mouse, btnCancel);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hoverCC) {
            paymentSuccess = true;
            successTimer = 0;
            result = PaymentResult::PAID_CREDIT_CARD;
        } else if (hoverCash) {
            paymentSuccess = true;
            successTimer = 0;
            result = PaymentResult::PAID_CASH;
        } else if (hoverCancel) {
            result = PaymentResult::CANCEL;
        }
    }
}

void PaymentScreen::Draw() {
    ApplyPaymentTheme();

    int cx = screenWidth / 2;
    int cy = screenHeight / 2;

    if (paymentSuccess) {
        for (int i = 0; i < 4; i++) {
            DrawCircle(cx, cy, 120.0f + (float)(i * 30), Color{0, 220, 100, (unsigned char)(15 - i * 3)});
        }

        DrawCircle(cx, cy, 60, GREEN_ACC);
        DrawText("V", cx - MeasureText("V", 50) / 2, cy - 25, 50, DARK_BG);

        const char* msg = "Payment Successful!";
        DrawText(msg, cx - MeasureText(msg, 30) / 2, cy + 80, 30, GREEN_ACC);

        const char* hint = "Returning to menu...";
        DrawText(hint, cx - MeasureText(hint, 16) / 2, cy + 120, 16, TEXT_DIM);
        return;
    }

    Rectangle cardRect = { (float)(cx - 200), 100, 400, 430 };
    DrawRectangleRounded(cardRect, 0.06f, 8, CARD_BG);
    DrawRectangleRoundedLines(cardRect, 0.06f, 8, BORDER_COLOR);

    const char* title = "CHECKOUT";
    DrawText(title, cx - MeasureText(title, 34) / 2, 130, 34, TEXT_LIGHT);

    DrawLine(cx - 140, 175, cx + 140, 175, BORDER_COLOR);

    int roundedAmount = (int)(totalAmount + 0.5f);
    std::string priceText = std::to_string(roundedAmount) + " Euro";
    DrawText(priceText.c_str(), cx - MeasureText(priceText.c_str(), 48) / 2, 200, 48, CYAN_ACC);

    const char* sub = bookingChannel == BookingChannel::Online
        ? "Online booking: credit card only"
        : "Walk-in booking: cash or credit card";
    DrawText(sub, cx - MeasureText(sub, 16) / 2, 260, 16, TEXT_DIM);

    DrawRectangleRounded(btnCreditCard, 0.4f, 8, hoverCC ? CYAN_HOVER : CYAN_ACC);
    const char* ccText = "CREDIT CARD (Online)";
    int ccw = MeasureText(ccText, 20);
    DrawText(ccText, (int)(btnCreditCard.x + (btnCreditCard.width - ccw) / 2), (int)(btnCreditCard.y + 16), 20, DARK_BG);

    Color cashColor = bookingChannel == BookingChannel::WalkIn
        ? (hoverCash ? PINK_HOVER : PINK_ACC)
        : BORDER_COLOR;
    DrawRectangleRounded(btnCash, 0.4f, 8, cashColor);
    const char* cashText = "CASH (On-site)";
    int cw = MeasureText(cashText, 20);
    DrawText(cashText, (int)(btnCash.x + (btnCash.width - cw) / 2), (int)(btnCash.y + 16), 20,
        bookingChannel == BookingChannel::WalkIn ? DARK_BG : TEXT_DIM);

    DrawRectangleRounded(btnCancel, 0.4f, 8, hoverCancel ? GetTheme().danger : CARD_COLOR);
    DrawRectangleRoundedLines(btnCancel, 0.4f, 8, hoverCancel ? GetTheme().dangerHover : BORDER_COLOR);
    const char* cancelText = "CANCEL";
    int canw = MeasureText(cancelText, 20);
    DrawText(cancelText, (int)(btnCancel.x + (btnCancel.width - canw) / 2), (int)(btnCancel.y + 14), 20,
        hoverCancel ? WHITE : TEXT_LIGHT);
}
