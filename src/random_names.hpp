#pragma once

#include <Arduino.h>

const char* phonetic[] PROGMEM = {
    "Alpha","Bravo","Charlie","Delta","Echo","Foxtrot","Golf","Hotel","India","Juliett",
    "Kilo","Lima","Mike","November","Oscar","Papa","Quebec","Romeo","Sierra","Tango",
    "Uniform","Victor","Whiskey","X-ray","Yankee","Zulu",
    "Zero","One","Two","Three","Four","Five","Six","Seven","Eight","Nine","Niner",
    "Decimal"
};

String GenerateRandomName() {
    uint16_t total = sizeof(phonetic) / sizeof(phonetic[0]);
    uint16_t index = random(total);
    char buffer[32];
    strcpy_P(buffer, (char*)pgm_read_ptr(&phonetic[index]));
    return String(buffer);
}
