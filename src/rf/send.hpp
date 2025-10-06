#pragma once

#include "utils.hpp"
#include "../random_names.hpp"
#include <LittleFS.h>
#include "../menu.hpp"

void RF_SaveTimings(uint16_t size, uint32_t* timings, bool start_level = false, float frequency = 433.92) 
{
    String name;
    int attempts = 0;
    const int maxAttempts = 50;

    do {
        name = "/" + GenerateRandomName() + ".bin";
        attempts++;
    } while (LittleFS.exists(name) && attempts < maxAttempts);

    if (attempts >= maxAttempts) {
        name = "/" + String(random(10000, 99999)) + ".bin";
    }

    File file = LittleFS.open(name, "w");
    if (!file) return;

    file.write((uint8_t*)&frequency, sizeof(frequency));
    file.write((uint8_t*)&size, sizeof(size));
    file.write((uint8_t*)&start_level, sizeof(start_level));
    file.write((uint8_t*)timings, size * sizeof(uint32_t));
    file.close();

    display.clearDisplay();
    Display_PrintCentered("Saved:\n%s", name.c_str());
    display.display();
    delay(1000);
}

bool RF_LoadTimings(const String& name, uint16_t& size, uint32_t* timings, bool& start_level, float& frequency) 
{
    File file = LittleFS.open(name, "r");
    if (!file) return false;

    if (file.size() < sizeof(frequency) + sizeof(uint16_t) + sizeof(bool)) {
        file.close();
        return false;
    }

    file.read((uint8_t*)&frequency, sizeof(frequency));
    file.read((uint8_t*)&size, sizeof(size));
    file.read((uint8_t*)&start_level, sizeof(start_level));

    if (size > 8000) size = 8000;
    file.read((uint8_t*)timings, size * sizeof(uint32_t));
    file.close();

    return true;
}

void RF_SendTimings(uint16_t size, uint32_t* timings, bool start_level = false)
{
    volatile uint32_t* gdo0_port_in = portInputRegister(digitalPinToPort(CC1101_GDO0));
    volatile uint32_t* gdo0_port_out = portOutputRegister(digitalPinToPort(CC1101_GDO0));
    uint32_t gdo0_bitmask = digitalPinToBitMask(CC1101_GDO0);

    RF_TransmitMode();
    pinMode(CC1101_GDO0, OUTPUT);

    bool out_level = start_level;
    if (out_level)
        *gdo0_port_out |= gdo0_bitmask;
    else
        *gdo0_port_out &= ~gdo0_bitmask;

    for (uint16_t i = 0; i < size; i++) {
        delayMicroseconds(timings[i]);
        out_level = !out_level;
        if (out_level)
            *gdo0_port_out |= gdo0_bitmask;
        else
            *gdo0_port_out &= ~gdo0_bitmask;
    }

    *gdo0_port_out &= ~gdo0_bitmask;
    pinMode(CC1101_GDO0, INPUT_PULLUP);
    RF_ReceiveMode();
}

void RF_PlaySaved(const String& filename)
{
    uint16_t size;
    bool start_level;
    float frequency;
    static uint32_t timings[8000];

    if (!RF_LoadTimings(filename, size, timings, start_level, frequency)) {
        display.clearDisplay();
        Display_PrintCentered("Failed to load:\n%s", filename.c_str());
        display.display();
        delay(1000);
        return;
    }

    RF_Frequency(frequency);
    RF_InitModule();
    display.clearDisplay();
    Display_PrintCentered("Sending:\n%s\n%.2f MHz\n%d samples", filename.c_str(), frequency, size);
    display.display();

    RF_SendTimings(size, timings, start_level);

    display.clearDisplay();
    Display_PrintCentered("Done!");
    display.display();
    delay(500);
}

void RF_Send()
{
read_files:
    Menu menu;
    bool running = true;

    display.clearDisplay();
    File dir = LittleFS.open("/");
    while (true) {
        File file = dir.openNextFile();
        if (!file) break;
        String name = file.name();
        if (name.endsWith(".bin")) {
            menu.AddItem(MenuItem(name, [name]() {
                RF_PlaySaved("/" + name);
            }));
        }
        file.close();
    }
    display.display();

    menu.AddItem(MenuItem("[Exit]", [&](){ running = false; }));
    menu.Revive();

    while (running)
    {
        if (ReadButton(BUTTON_LEFT) && ReadButton(BUTTON_RIGHT))
        {
            unsigned long hold_start = millis();
            while (ReadButton(BUTTON_LEFT) && ReadButton(BUTTON_RIGHT))
            {
                if (millis() - hold_start > 1000)
                {
                    String selected = menu.GetSelectedTitle();
                    if (!selected.startsWith("/")) selected = "/" + selected;

                    display.clearDisplay();
                    Display_PrintCentered("Deleting:\n%s", selected.c_str());
                    display.display();
                    delay(1000);

                    if (LittleFS.exists(selected))
                    {
                        LittleFS.remove(selected);
                        display.clearDisplay();
                        Display_PrintCentered("Deleted:\n%s", selected.c_str());
                        display.display();
                        delay(1000);
                        HaltTillRelease(BUTTON_LEFT);
                        HaltTillRelease(BUTTON_RIGHT);
                        goto read_files;
                    }
                }
            }
        }

        menu.HandleButtons();
        menu.Render();
        delay(10);
    }

    RF_DeInitModule();
    HaltTillRelease(BUTTON_CENTER);
}
