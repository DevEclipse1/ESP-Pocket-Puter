#include "global.hpp"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
IRsend irtx(IR_TX);

VirtualButtons virtualButtons = { false, false, false };

bool ReadButton(int P)
{
    bool v = false;
    if (P == BUTTON_LEFT) v = virtualButtons.left;
    else if (P == BUTTON_CENTER) v = virtualButtons.center;
    else if (P == BUTTON_RIGHT) v = virtualButtons.right;
    return (digitalRead(P) == LOW) || v;
}

bool ReadButtonWait(int P)
{
    bool pressed = false;
    if (ReadButton(P)) pressed = true;
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
