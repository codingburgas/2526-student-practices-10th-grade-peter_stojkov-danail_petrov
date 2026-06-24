#pragma once
#include "raylib.h"
#include "screens/showtimes.h"

enum class PaymentResult {
    NONE,
    PAID_CREDIT_CARD,
    PAID_CASH,
    DONE,
    CANCEL
};

class PaymentScreen {
public:
    PaymentScreen(int screenWidth, int screenHeight);
    
    void SetTotalAmount(float amount);
    void SetBookingChannel(BookingChannel channel);
    void Update(PaymentResult& result);
    void Draw();

private:
    int screenWidth;
    int screenHeight;
    float totalAmount;
    BookingChannel bookingChannel;

    Rectangle btnCreditCard;
    Rectangle btnCash;
    Rectangle btnCancel;

    bool hoverCC;
    bool hoverCash;
    bool hoverCancel;
    bool paymentSuccess;
    int successTimer;
};
