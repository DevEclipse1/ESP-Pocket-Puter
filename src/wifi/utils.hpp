#pragma once

#include <Arduino.h>
#include <WiFi.h>

extern "C" {
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
  esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
  esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

/*
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    if (arg == 31337) return 1;
    else return 0;
}*/

uint8_t deauth_frame_default[26] = {
   /*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate)
   /*  2 - 3  */ 0x00, 0x00,                         // duration (SDK takes care of that)
   /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
   /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
   /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
   /* 22 - 23 */ 0x00, 0x00,                         // fragment & squence number
   /* 24 - 25 */ 0x01, 0x00                          // reason code (1 = unspecified reason)
};

uint8_t beacon_packet[109] = {/*  0 - 3  */ 0x80,
                                 0x00,
                                 0x00,
                                 0x00, // Type/Subtype: managment beacon frame
                                 /*  4 - 9  */ 0xFF,
                                 0xFF,
                                 0xFF,
                                 0xFF,
                                 0xFF,
                                 0xFF, // Destination: broadcast
                                 /* 10 - 15 */ 0x01,
                                 0x02,
                                 0x03,
                                 0x04,
                                 0x05,
                                 0x06, // Source
                                 /* 16 - 21 */ 0x01,
                                 0x02,
                                 0x03,
                                 0x04,
                                 0x05,
                                 0x06, // Source

                                 // Fixed parameters
                                 /* 22 - 23 */ 0x00,
                                 0x00, // Fragment & sequence number (will be done by the SDK)
                                 /* 24 - 31 */ 0x83,
                                 0x51,
                                 0xf7,
                                 0x8f,
                                 0x0f,
                                 0x00,
                                 0x00,
                                 0x00, // Timestamp
                                 /* 32 - 33 */ 0xe8,
                                 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
                                 /* 34 - 35 */ 0x31,
                                 0x00, // capabilities Tnformation

                                 // Tagged parameters

                                 // SSID parameters
                                 /* 36 - 37 */ 0x00,
                                 0x20, // Tag: Set SSID length, Tag length: 32
                                 /* 38 - 69 */ 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20,
                                 0x20, // SSID

                                 // Supported Rates
                                 /* 70 - 71 */ 0x01,
                                 0x08,          // Tag: Supported Rates, Tag length: 8
                                 /* 72 */ 0x82, // 1(B)
                                 /* 73 */ 0x84, // 2(B)
                                 /* 74 */ 0x8b, // 5.5(B)
                                 /* 75 */ 0x96, // 11(B)
                                 /* 76 */ 0x24, // 18
                                 /* 77 */ 0x30, // 24
                                 /* 78 */ 0x48, // 36
                                 /* 79 */ 0x6c, // 54

                                 // Current Channel
                                 /* 80 - 81 */ 0x03,
                                 0x01,          // Channel set, length
                                 /* 82 */ 0x01, // Current Channel

                                 // RSN information
                                 /*  83 -  84 */ 0x30,
                                 0x18,
                                 /*  85 -  86 */ 0x01,
                                 0x00,
                                 /*  87 -  90 */ 0x00,
                                 0x0f,
                                 0xac,
                                 0x02,
                                 /*  91 -  92 */ 0x02,
                                 0x00,
                                 /*  93 - 100 */ 0x00,
                                 0x0f,
                                 0xac,
                                 0x04,
                                 0x00,
                                 0x0f,
                                 0xac,
                                 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not
                                          supported by many devices*/
                                 /* 101 - 102 */ 0x01,
                                 0x00,
                                 /* 103 - 106 */ 0x00,
                                 0x0f,
                                 0xac,
                                 0x02,
                                 /* 107 - 108 */ 0x00,
                                 0x00};

void WiFi_Init()
{
    static bool initialized = false;
    if (initialized) return;

    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    initialized = true;
}

uint8_t* WiFi_RandomMac() {
    static uint8_t macAddr[6];
    for (int i = 0; i < 6; i++)
        macAddr[i] = (uint8_t)random(256);
    macAddr[0] &= 0xFE;
    macAddr[0] |= 0x02;
    return macAddr;
}