#pragma once
#include "utils.hpp"
#include <Arduino.h>
#include <vector>
#include <algorithm>

#include <LittleFS.h>
#include "send.hpp"
#include "../selection.hpp"

void RF_Scan() {
    float freq = RF_SelectFrequency();

    RF_InitModuleDisplay();
    RF_Frequency(freq);
    RF_ReceiveMode();
    pinMode(CC1101_GDO0, INPUT_PULLUP);

    const int threshold = -60;
    const uint16_t max_sample = 8000;
    bool ready_to_replay = false;
    uint16_t sample = 0;
    static uint32_t timings[max_sample] = {0};
    bool start_level = false;

    display.clearDisplay();
    Display_PrintCentered("Scanning...\nAt %.2f MHz", freq);
    display.display();

    volatile uint32_t* gdo0_port_in = portInputRegister(digitalPinToPort(CC1101_GDO0));
    volatile uint32_t* gdo0_port_out = portOutputRegister(digitalPinToPort(CC1101_GDO0));
    uint32_t gdo0_bitmask = digitalPinToBitMask(CC1101_GDO0);

    uint32_t prev_time = micros();
    bool prev_level = (*gdo0_port_in & gdo0_bitmask);
    bool capturing = false;
    uint32_t last_rssi_update = millis();

    while (!ReadButton(BUTTON_CENTER)) {
        uint32_t time = micros();

        if (!capturing && !ready_to_replay && radio.getRSSI() > threshold) {
            delayMicroseconds(100);
            capturing = true;
            memset(timings, 0, sizeof(timings));
            sample = 0;
            prev_time = micros();
            prev_level = (*gdo0_port_in & gdo0_bitmask);
        }

        if (capturing) {
            bool level = (*gdo0_port_in & gdo0_bitmask);
            if (level != prev_level) {
                uint32_t duration = time - prev_time;
                if (sample >= max_sample) {
                    capturing = false;
                    ready_to_replay = true;
                    continue;
                }
                if (sample == 0) start_level = prev_level;
                timings[sample++] = duration;
                prev_time = time;
                prev_level = level;
            }

            if (sample > 25 && time - prev_time >= 250000) {
                ledcAttachPin(BUZZER_PIN, 0);
                ledcSetup(0, 500, 8);
                ledcWrite(0, 50);
                delay(50);
                ledcWrite(0, 0);

                uint16_t size = sample;
                display.clearDisplay();
                Display_PrintCentered("Captured signal:\n%d samples", size);
                display.display();
                prev_time = time;
                ready_to_replay = true;
                capturing = false;

                noTone(BUZZER_PIN);
            }
        }

        if (ReadButton(BUTTON_RIGHT) && ready_to_replay) {
            int size = sample;
            display.clearDisplay();
            Display_PrintCentered("Sending signal:\n%d samples", size);
            display.display();

            RF_SendTimings(size, timings, start_level);

            HaltTillRelease(BUTTON_RIGHT);
            display.clearDisplay();
            Display_PrintCentered("Captured signal:\n%d samples", size);
            display.display();
        }

        if (ReadButton(BUTTON_LEFT)) {
            display.clearDisplay();
            Display_PrintCentered("Saving signal:\n%d samples", sample);
            delay(500);
            display.display();
            RF_SaveTimings(sample, timings, start_level, freq);
        }
    }

    RF_DeInitModule();
    HaltTillRelease(BUTTON_CENTER);
}
