#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define BUTTON_LEFT 1
#define BUTTON_CENTER 2
#define BUTTON_RIGHT 3
#define BUZZER_PIN 0
#define IR_TX 21

#define CC1101_SCK   4
#define CC1101_MISO  5
#define CC1101_MOSI  6
#define CC1101_CS    10
#define CC1101_GDO0  7

extern Adafruit_SSD1306 display;
extern IRsend irtx;

bool ReadButton(int P);
bool ReadButtonWait(int P);
bool WaitForButton(int P);
void HaltTillRelease(int P);
void HaltTillPress(int P);

#endif
