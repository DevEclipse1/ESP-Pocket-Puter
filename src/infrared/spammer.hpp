#include "../global.hpp"
#include "../display_utils.h"

void Ir_Spammer()
{
  display.clearDisplay();
  Display_PrintCentered("Spamming IR...");
  display.display();

  pinMode(IR_TX, OUTPUT);
  while (!ReadButton(BUTTON_CENTER))
  {
    irtx.sendNEC(random(0x1000000, 0xFFFFFFF), 32);
  }

  HaltTillRelease(BUTTON_CENTER);

  display.clearDisplay();
  display.display();
}