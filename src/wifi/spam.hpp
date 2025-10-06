#pragma once

#include <Arduino.h>
#include "../global.hpp"
#include "utils.hpp"

#define BEACON_SSID_OFFSET 38
#define SRCADDR_OFFSET 10
#define BSSID_OFFSET 16
#define SEQNUM_OFFSET 22

char empty_ssid[32];

String WiFi_RandomSSID(uint8_t len) {
    String s = "";
    for (uint8_t i = 0; i < len; i++) {
        s += char(random(0, 255));
    }
    return s;
}

void WiFi_BeaconSpam()
{
    for (int i = 0; i < 32; i++)
        empty_ssid[i] = ' ';

    WiFi.mode(WIFI_MODE_STA);

    int channels[] = {1, 6, 11};
    const int NUM_CHANNELS = sizeof(channels) / sizeof(channels[0]);

    int channelIndex = 0;
    esp_wifi_set_channel(channels[channelIndex], WIFI_SECOND_CHAN_NONE);

    const int NUM_SSIDS = 20;
    String ssids[NUM_SSIDS];
    char ssidChars[NUM_SSIDS][32];

    for (int i = 0; i < NUM_SSIDS; i++) {
        uint8_t ssid_len = uint8_t(random(6, 16));
        ssids[i] = WiFi_RandomSSID(ssid_len);
        ssids[i].toCharArray(ssidChars[i], sizeof(ssidChars[i]));
    }

    while (!ReadButton(BUTTON_CENTER))
    {
        for (int n = 0; n < NUM_SSIDS; n++) {
            uint8_t mac[6];
            for (int j = 0; j < 6; j++) {
                mac[j] = uint8_t(random(0, 256));
            }

            uint8_t packet[109];
            memcpy(packet, beacon_packet, 109);

            memcpy(&packet[10], mac, 6);
            memcpy(&packet[16], mac, 6);

            memcpy(&packet[38], empty_ssid, 32);
            memcpy(&packet[38], ssidChars[n], strlen(ssidChars[n]));
            packet[82] = channels[channelIndex];
            packet[34] = 0x31;

            for (int i = 0; i < 3; i++) {
                esp_wifi_80211_tx(WIFI_IF_STA, packet, sizeof(packet), false);
                delay(1);
            }
        }

        channelIndex++;
        if (channelIndex >= NUM_CHANNELS) channelIndex = 0;
        esp_wifi_set_channel(channels[channelIndex], WIFI_SECOND_CHAN_NONE);

        display.clearDisplay();
        Display_PrintCentered("Beacon Spam");
        display.display();

        delay(50);
    }

    HaltTillRelease(BUTTON_CENTER);
}
