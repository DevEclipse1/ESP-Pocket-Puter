#pragma once

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include "../global.hpp"
#include "../menu.hpp"
#include "../selection.hpp"
#include "../display_utils.h"
#include "ir_codes.hpp"

uint16_t rawData[300];

uint8_t bitsleft_r = 0;
uint8_t bits_r = 0;
uint8_t code_ptr;
volatile const IrCode* powerCode;

#define MAX_WAIT_TIME 65535

uint8_t read_bits(uint8_t count) {
  uint8_t tmp = 0;
  for (uint8_t i = 0; i < count; i++) {
    if (bitsleft_r == 0) {
      bits_r = powerCode->codes[code_ptr++];
      bitsleft_r = 8;
    }
    bitsleft_r--;
    tmp |= (((bits_r >> bitsleft_r) & 1) << (count - 1 - i));
  }
  return tmp;
}

uint16_t ontime, offtime;
uint8_t i, num_codes;
uint8_t region;

void TvbGone_SendAll(int region) {
  pinMode(IR_TX, OUTPUT);

  if (region == NA) {
    num_codes = num_NAcodes;
  } else {
    num_codes = num_EUcodes;
  }

  for (int i = 0; i < num_codes; i++) {
    if (ReadButton(BUTTON_CENTER)) {
      HaltTillRelease(BUTTON_CENTER);
      digitalWrite(IR_TX, LOW);
      display.clearDisplay();
      Display_PrintCentered("Canceled!");
      display.display();
      delay(1000);
      return;
    }

    if (region == NA) {
      powerCode = NApowerCodes[i];
    } else {
      powerCode = EUpowerCodes[i];
    }

    const uint8_t freq = powerCode->timer_val;
    const uint8_t numpairs = powerCode->numpairs;
    const uint8_t bitcompression = powerCode->bitcompression;

    code_ptr = 0;
    for (uint8_t k = 0; k < numpairs; k++) {
      uint16_t ti = (read_bits(bitcompression)) * 2;
      ontime = powerCode->times[ti];
      offtime = powerCode->times[ti + 1];

      rawData[k * 2]     = ontime * 10;
      rawData[(k * 2)+1] = offtime * 10;
      yield();
    }

    irtx.sendRaw(rawData, numpairs * 2, freq);
    bitsleft_r = 0;

    int display_width = display.width();
    int display_height = display.height();

    display.clearDisplay();
    display.drawRect(0, 0, display_width, display_height, SSD1306_WHITE);

    int bar_width = (display_width - 10) * (i + 1) / num_codes;
    display.drawRect(5, display_height / 2 - 5, display_width - 10, 10, SSD1306_WHITE);
    display.fillRect(5, display_height / 2 - 5, bar_width, 10, SSD1306_WHITE);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, display_height - 10);
    char buf[32];
    snprintf(buf, sizeof(buf), "Sending %d/%d", i + 1, num_codes);
    display.print(buf);

    display.display();
  }

  display.clearDisplay();
  Display_PrintCentered("Done!");
  display.display();
  delay(1000);

  HaltTillRelease(BUTTON_CENTER);

  display.clearDisplay();
  display.display();
}

void TvbGone_Callback_EU() {
  TvbGone_SendAll(EU);
}

void TvbGone_Callback_NA() {
  TvbGone_SendAll(NA);
}
