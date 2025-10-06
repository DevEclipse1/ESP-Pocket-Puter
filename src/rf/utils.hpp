#pragma once

#include <Arduino.h>
#include "../global.hpp"
#include <RadioLib.h>
#include "../display_utils.h"
#include <math.h>
#include "driver/gpio.h"

Module mod(CC1101_CS, CC1101_GDO0, RADIOLIB_NC, RADIOLIB_NC);
CC1101 radio(&mod);

struct StrongestHit {
    float freq;
    int rssi;
};

const float subghz_frequency_list[] = {
    300.000f,
    302.757f,
    303.875f,
    303.900f,
    304.250f,
    307.000f,
    307.500f,
    307.800f,
    309.000f,
    310.000f,
    312.000f,
    312.100f,
    312.200f,
    313.000f,
    313.850f,
    314.000f,
    314.350f,
    314.980f,
    315.000f,
    318.000f,
    330.000f,
    345.000f,
    348.000f,
    350.000f,

    387.000f,
    390.000f,
    418.000f,
    430.000f,
    430.500f,
    431.000f,
    431.500f,
    433.075f,
    433.220f,
    433.420f,
    433.657f,
    433.889f,
    433.920f,
    434.075f,
    434.177f,
    434.190f,
    434.390f,
    434.420f,
    434.620f,
    434.775f,
    438.900f,
    440.175f,
    464.000f,
    467.750f,

    779.000f,
    868.350f,
    868.400f,
    868.800f,
    868.950f,
    906.400f,
    915.000f,
    925.000f,
    928.000f
};

bool RF_InitModule()
{
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        radio.setFrequency(433.92);
        radio.setOOK(true);
        radio.setOutputPower(12);
        return true;
    } else {
        Serial.printf("CC1101 init failed, code: %d\n", state);
        return false;
    }
}

bool RF_InitModuleDisplay()
{
    if (RF_InitModule())
    {
        display.clearDisplay();
        Display_PrintCentered("Radio found");
        display.display();
        delay(500);
        return true;
    }
    else
    {
        display.clearDisplay();
        Display_PrintCentered("Radio not found");
        display.display();
        HaltTillPress(BUTTON_CENTER);
        return false;
    }
}

bool RF_DeInitModule()
{
    if (radio.getChipVersion() == RADIOLIB_CC1101_VERSION_CURRENT || radio.getChipVersion() == RADIOLIB_CC1101_VERSION_LEGACY || radio.getChipVersion() == RADIOLIB_CC1101_VERSION_CLONE)
    {
        radio.sleep();
        return true;
    }
    else
    {
        return false;
    }
}

void RF_Frequency(float freq)
{
    radio.setFrequency(freq);
}

void RF_SendBit(bool bit)
{
    digitalWrite(CC1101_GDO0, bit ? HIGH : LOW);
}

void RF_TransmitMode()
{
    pinMode(CC1101_GDO0, OUTPUT);
    radio.transmitDirectAsync();
}

void RF_SendByte(uint8_t byte, int t)
{
    for (int8_t bit = 7; bit >= 0; bit--) {
        RF_SendBit((byte & (1 << bit)));
        delayMicroseconds(t);
    }
}

void RF_ReceiveMode()
{
    pinMode(CC1101_GDO0, INPUT);
    radio.receiveDirectAsync();
}

void RF_CleanRxFifo()
{

}

StrongestHit RF_GetStrongestFrequency()
{
    const size_t count = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);

    radio.setRxBandwidth(80);

    float bestFreq = 0;
    int bestRssi = -127;

    for (size_t i = 0; i < count; ++i)
    {
        float freq = subghz_frequency_list[i];
        RF_Frequency(freq);
        RF_CleanRxFifo();
        RF_ReceiveMode();
        delay(2);
        int rssi = radio.getRSSI();
        if (rssi > bestRssi)
        {
            bestRssi = rssi;
            bestFreq = freq;
        }
    }

    return {bestFreq, bestRssi};
}

float RF_SelectFrequency()
{
    size_t count = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
    std::vector<String> names;
    names.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        char buf[16];
        dtostrf(subghz_frequency_list[i], 1, 3, buf);
        String s = String(buf) + " MHz";
        s.trim();
        names.emplace_back(s);
    }

    int start_index = 0;
    for (size_t i = 0; i < count; ++i) {
        if (fabs(subghz_frequency_list[i] - 433.92f) < 0.001f) {
            start_index = (int)i;
            break;
        }
    }

    String selected = SelectionMenu(names.data(), (int)count, start_index);
    selected.trim();

    if (selected.length() > 0) {
        for (size_t i = 0; i < count; ++i) {
            String tmp = names[i];
            tmp.trim();
            if (selected == tmp) {
                float freq = subghz_frequency_list[i];

                return freq;
            }
        }
    }

    return 433.92f;
}