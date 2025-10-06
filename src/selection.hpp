#pragma once

#include "global.hpp"

String SelectionMenu(String options[], int count, int initial = 0)
{
    int selected = initial;
    int prev_selected = initial;
    bool animating = false;
    unsigned long anim_start = 0;
    int anim_dir = 0;
    float anim_progress = 1.0f;

    while (true)
    {
        display.setTextWrap(false);

        unsigned long now = millis();

        if (animating) {
            unsigned long elapsed = now - anim_start;
            anim_progress = (float)elapsed / 180.0f;
            if (anim_progress >= 1.0f) {
                anim_progress = 1.0f;
                animating = false;
                prev_selected = selected;
            }
        }

        display.clearDisplay();
        display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);

        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

        int16_t x, y;
        uint16_t w, h;

        display.getTextBounds(options[prev_selected], 0, 0, &x, &y, &w, &h);
        int prev_w = w;
        int prev_h = h;

        display.getTextBounds(options[selected], 0, 0, &x, &y, &w, &h);
        int cur_w = w;
        int cur_h = h;

        int cy = (display.height() / 2) - (cur_h / 2);

        display.setCursor(5, cy);
        display.print("<");
        display.setCursor(display.width() - 10, cy);
        display.print(">");

        int center_x = display.width() / 2;
        int offset = display.width();

        if (animating) {
            float t = anim_progress;
            int prev_x = center_x - (prev_w / 2) - (int)(anim_dir * t * offset);
            int cur_x = center_x - (cur_w / 2) + (int)(anim_dir * (1.0f - t) * offset);

            display.setCursor(prev_x, cy);
            display.print(options[prev_selected]);

            display.setCursor(cur_x, cy);
            display.print(options[selected]);
        } else {
            int cur_x = center_x - (cur_w / 2);
            display.setCursor(cur_x, cy);
            display.print(options[selected]);
        }

        display.display();

        if (!animating) {
            if (ReadButtonWait(BUTTON_LEFT)) {
                prev_selected = selected;
                selected--;
                if (selected < 0) selected = count - 1;
                anim_start = millis();
                anim_progress = 0.0f;
                anim_dir = -1;
                animating = true;
            }

            if (ReadButtonWait(BUTTON_RIGHT)) {
                prev_selected = selected;
                selected++;
                if (selected >= count) selected = 0;
                anim_start = millis();
                anim_progress = 0.0f;
                anim_dir = 1;
                animating = true;
            }

            if (ReadButton(BUTTON_CENTER)) {
                HaltTillRelease(BUTTON_CENTER);
                return options[selected];
            }
        }
    }
}
