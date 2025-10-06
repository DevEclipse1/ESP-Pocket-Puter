#include "../global.hpp"

void Piano_Callback()
{
  const int tones[3] = { 262, 330, 392 };
  unsigned long holdStart = 0;

  display.clearDisplay();
  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
  display.display();

  while (true)
  {
    bool pressedL = ReadButton(BUTTON_LEFT);
    bool pressedC = ReadButton(BUTTON_CENTER);
    bool pressedR = ReadButton(BUTTON_RIGHT);

    if (pressedC && !pressedL && !pressedR) {
      if (holdStart == 0) holdStart = millis();
      if (millis() - holdStart >= 1000) {
        HaltTillRelease(BUTTON_CENTER);
        noTone(BUZZER_PIN);
        break;
      }
    } else {
      holdStart = 0;
    }

    if (pressedL) tone(BUZZER_PIN, tones[0]);
    else if (pressedC) tone(BUZZER_PIN, tones[1]);
    else if (pressedR) tone(BUZZER_PIN, tones[2]);
    else noTone(BUZZER_PIN);

    display.clearDisplay();
    display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);

    int keyWidth = display.width() / 3;
    int keyHeight = display.height() - 15;

    for (int i = 0; i < 3; i++) {
      int x = i * keyWidth;
      int y = 15;
      if ((i == 0 && pressedL) ||
          (i == 1 && pressedC) ||
          (i == 2 && pressedR)) {
        display.fillRect(x+2, y+2, keyWidth-4, keyHeight-4, SSD1306_WHITE);
        display.drawRect(x, y, keyWidth, keyHeight, SSD1306_BLACK);
      } else {
        display.drawRect(x, y, keyWidth, keyHeight, SSD1306_WHITE);
      }
    }

    display.display();
    delay(30);
  }

  display.clearDisplay();
  display.display();
}
