#pragma once

#include <WiFi.h>
#include "esp_wifi.h"

#include "../global.hpp"
#include "../menu.hpp"
#include "utils.hpp"
#include "../display_utils.h"
#include "deauth.hpp"

#include <vector>

struct APRecord {
    String ssid;
    int32_t rssi;
    uint8_t channel;
    uint8_t bssid[6];
};

void WiFi_Scan()
{
    display.clearDisplay();
    Display_PrintCentered("Scanning...");
    display.display();

    WiFi_Init();

    wifi_scan_config_t scanConf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };

    esp_err_t r = esp_wifi_scan_start(&scanConf, true);
    if (r != ESP_OK) {
        display.clearDisplay();
        Display_PrintCentered("Scan failed!");
        display.display();
        return;
    }

    uint16_t apCount = 0;
    esp_wifi_scan_get_ap_num(&apCount);

    String found_str = "Found " + String(apCount) + " APs";
    display.clearDisplay();
    Display_PrintCentered(found_str.c_str());
    display.display();

    delay(1000);

    if (apCount == 0) {
        return;
    }

    std::vector<wifi_ap_record_t> apRecords(apCount);
    uint16_t got = apCount;
    esp_err_t err = esp_wifi_scan_get_ap_records(&got, apRecords.data());

    if (err != ESP_OK || got == 0) {
        
        return;
    }
    apCount = got;

    std::vector<APRecord> myAPs(apCount);

    for (int i = 0; i < apCount; i++) {
        char ssidBuf[33];
        strncpy(ssidBuf, (const char*)apRecords[i].ssid, 32);
        ssidBuf[32] = '\0';

        String ssid = String(ssidBuf);
        if (ssid.length() == 0) {
            ssid = "<Hidden SSID>";
        }

        myAPs[i].ssid = ssid;
        myAPs[i].rssi = apRecords[i].rssi;
        myAPs[i].channel = apRecords[i].primary;
        memcpy(myAPs[i].bssid, apRecords[i].bssid, 6);
    }

    Menu* menu = new Menu();

    for (auto& ap : myAPs) {
        String ssid = ap.ssid;
        int rssi = ap.rssi;
        uint8_t channel = ap.channel;
        uint8_t bssid[6];
        memcpy(bssid, ap.bssid, 6);

        menu->AddItem(MenuItem(String(ssid), [ssid, rssi, channel, bssid]() {
            HaltTillRelease(BUTTON_CENTER);

            bool info_running = true;

            char bssidStr[18];
            sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    bssid[0], bssid[1], bssid[2],
                    bssid[3], bssid[4], bssid[5]);

            Menu info;
            info.AddItem(MenuItem(ssid, nullptr));
            info.AddItem(MenuItem("RSSI: " + String(rssi), nullptr));
            info.AddItem(MenuItem("CHANNEL: " + String(channel), nullptr));
            info.AddItem(MenuItem("MAC: " + String(bssidStr), nullptr));
            info.AddItem(MenuItem("Deauth", [bssid, ssid, channel]() {
                WiFi_DeauthAP(bssid, ssid, channel);
            }));
            info.AddItem(MenuItem("[Back]", [&](){ info_running = false; }));
            info.Revive();

            while (info_running)
            {
                info.HandleButtons();
                info.Render();
            }
        }));
    }

    bool running = true;
    menu->AddItem(MenuItem("[Exit]", [&](){ running = false; }));

    menu->Revive(nullptr);

    while (running)
    {
        menu->HandleButtons();
        menu->Render();
        delay(10);
    }

    delete menu;
}
