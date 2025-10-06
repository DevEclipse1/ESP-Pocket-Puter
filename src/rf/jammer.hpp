#pragma once

#include "utils.hpp"
#include "../display_utils.h"
#include "../selection.hpp"
#include "driver/rmt.h"

void RF_Jammer()
{
    float freq = RF_SelectFrequency();

    RF_InitModuleDisplay();
    RF_Frequency(freq);
    RF_TransmitMode();

    display.clearDisplay();
    Display_PrintCentered("Jamming...\n%s", String(freq, 2).c_str());
    display.display();

    while (!ReadButton(BUTTON_CENTER))
    {
        digitalWrite(CC1101_GDO0, HIGH);
        delayMicroseconds(random(50, 800));
        digitalWrite(CC1101_GDO0, LOW);
        delayMicroseconds(random(50, 200));
    }
    noTone(CC1101_GDO0);

    RF_DeInitModule();
    HaltTillRelease(BUTTON_CENTER);
}
