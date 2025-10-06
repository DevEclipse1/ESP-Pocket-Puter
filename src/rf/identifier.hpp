#pragma once

#include "utils.hpp"
#include "../menu.hpp"
#include <vector>
#include <algorithm>

struct FrequencyHit {
    float freq;
    int rssi;
    unsigned long lastSeen;
};

void RF_Identifier()
{
    RF_InitModuleDisplay();
    RF_ReceiveMode();
    radio.setRxBandwidth(80);

    const size_t count = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);

    std::vector<FrequencyHit> hits;
    hits.reserve(count);

    unsigned long bestSeenTime = 0;
    float bestFreq = 0;
    int bestRssi = -127;

    while (!ReadButton(BUTTON_CENTER))
    {
        for (size_t i = 0; i < count; ++i)
        {
            float freq = subghz_frequency_list[i];
            RF_Frequency(freq);
            RF_CleanRxFifo();
            RF_ReceiveMode();
            delay(3);
            int rssi = radio.getRSSI();
            if (rssi > -70)
            {
                bool found = false;
                for (auto& h : hits)
                {
                    if (h.freq == freq)
                    {
                        h.rssi = rssi;
                        h.lastSeen = millis();
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    hits.push_back({freq, rssi, millis()});
                }

                if (rssi > bestRssi)
                {
                    bestRssi = rssi;
                    bestFreq = freq;
                    bestSeenTime = millis();
                }
            }
        }

        unsigned long now = millis();
        hits.erase(std::remove_if(hits.begin(), hits.end(), [now](const FrequencyHit& h) {
            return now - h.lastSeen > 2000;
        }), hits.end());

        std::sort(hits.begin(), hits.end(), [](const FrequencyHit& a, const FrequencyHit& b) {
            return a.rssi > b.rssi;
        });

        Menu menu;
        for (auto& hit : hits)
            menu.AddItem(MenuItem(String(hit.freq, 2) + "MHz " + String(hit.rssi) + "dBm", nullptr));

        menu.Revive();
        menu.Render(true);

        display.setCursor(3, 3);

        if (bestSeenTime != 0 && now - bestSeenTime < 5000)
        {
            display.print("Top: ");
            display.print(String(bestFreq, 2));
            display.print(" ");
            display.print(bestRssi);
        }
        else
        {
            display.print("Top: ---");
        }

        display.display();
    }

    HaltTillRelease(BUTTON_CENTER);
    RF_DeInitModule();
}
