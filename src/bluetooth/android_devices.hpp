#pragma once

#include <Arduino.h>

#pragma once

#include <stdint.h>

const uint32_t ANDROID_MODEL_IDS[] = {
    0x000047, // arduino
    0x470000, // arduino
    0x03F5D4, // writing api key
    0x8D13B9, // ble tws
    0x8D5B67, // pixel 90c
    0x989D0A, // setup your new pixel
    0xC7A267, // fake test mouse
    0xCC93A5, // ford
    0x99F098, // s22 ultra
    0x9DB896, // your bmw
};

constexpr size_t ANDROID_MODEL_IDS_COUNT = sizeof(ANDROID_MODEL_IDS) / sizeof(ANDROID_MODEL_IDS[0]);
