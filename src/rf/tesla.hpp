#pragma once

#include "utils.hpp"
#include "../display_utils.h"

const uint8_t tesla_sequence[] = {0x02, 0xAA, 0xAA, 0xAA, 0x2B, 0x2C, 0xCB, 0x33, 0x33, 0x2D, 0x34,
                                  0xB5, 0x2B, 0x4D, 0x32, 0xAD, 0x2C, 0x56, 0x59, 0x96, 0x66, 0x66,
                                  0x5A, 0x69, 0x6A, 0x56, 0x9A, 0x65, 0x5A, 0x58, 0xAC, 0xB3, 0x2C,
                                  0xCC, 0xCC, 0xB4, 0xD2, 0xD4, 0xAD, 0x34, 0xCA, 0xB4, 0xA0};
const uint8_t tesla_sequence_length = sizeof(tesla_sequence);

void RF_TeslaChargePort()
{
    float freq = 433.92;

    RF_InitModuleDisplay();
    display.clearDisplay();
    Display_PrintCentered("Opening Tesla\nCharging Port...");
    display.display();

    while (!ReadButton(BUTTON_CENTER))
    {
        RF_Frequency(freq);
        RF_TransmitMode();

        for (int i = 0; i < 5; ++i)
        {
            for (uint8_t j = 0; j < tesla_sequence_length; j++) 
            { 
                RF_SendByte(tesla_sequence[j], 400); 
            }
            delay(23);
        }

        if (freq == 433.92f)
        {
            freq = 315.0f;
        }
        else
        {
            freq = 433.92f;
        }
    }

    HaltTillRelease(BUTTON_CENTER);
    display.clearDisplay();
    display.display();
    RF_DeInitModule();
}