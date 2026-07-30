#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <Wire.h>
#include <U8g2lib.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include "freertos/semphr.h"

// تعديل الدبابيس لترتيبك الخاص
#define BUTTON_LEFT   12
#define BUTTON_CENTER 27
#define BUTTON_RIGHT  14

#define BUZZER_PIN    0
#define IR_TX         4

// دبابيس مستقبلات الـ IR
#define IR_RX1_PIN    17
#define IR_RX2_PIN    19

#define CC1101_CS     10
#define CC1101_GDO0   7

struct VirtualButtons {
    bool left;
    bool center;
    bool right;
};

extern VirtualButtons virtualButtons;

#define VIRTUAL_BUTTON_LEFT   0
#define VIRTUAL_BUTTON_CENTER 1
#define VIRTUAL_BUTTON_RIGHT  2

// استبدال كائن الشاشة إلى U8g2
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;
extern IRsend irtx;

bool ReadButton(int P);
bool ReadButtonWait(int P);
bool WaitForButton(int P);
void HaltTillRelease(int P);
void HaltTillPress(int P);

#endif
