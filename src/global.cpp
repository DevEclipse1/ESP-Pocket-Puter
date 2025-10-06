#include "global.hpp"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
IRsend irtx(IR_TX);

bool ReadButton(int P)
{
    return digitalRead(P) == LOW;
}

bool ReadButtonWait(int P)
{
    bool pressed = false;
    if (ReadButton(P)) {
        pressed = true;
    }

    if (pressed) {
        while (ReadButton(P)) delay(10);
        return true;
    }

    return false;
}

bool WaitForButton(int P)
{
    while (ReadButton(P)) delay(10);
    while (!ReadButton(P)) delay(10);
    delay(50);
    return true;
}

void HaltTillRelease(int P)
{
    while (ReadButton(P)) delay(10);
    delay(1);
    while (ReadButton(P)) delay(10);
    delay(1);
    while (ReadButton(P)) delay(10);
}

void HaltTillPress(int P)
{
    while (ReadButton(P)) delay(10);
    while (!ReadButton(P)) delay(10);
    delay(50);
}
