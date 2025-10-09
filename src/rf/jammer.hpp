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
        uint16_t r = random(200, 1000);
        GPIO.out_w1ts.val = (1 << CC1101_GDO0);
        ets_delay_us(r);
        GPIO.out_w1tc.val = (1 << CC1101_GDO0);
        ets_delay_us(r);
    }
    noTone(CC1101_GDO0);

    RF_DeInitModule();
    HaltTillRelease(BUTTON_CENTER);
}
