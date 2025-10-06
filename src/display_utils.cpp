#include "display_utils.h"

void Display_PrintCentered(const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    String text(buf);

    int textSize = 1;
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);

    std::vector<String> lines;
    int start = 0;
    for (int i = 0; i <= text.length(); i++) {
        if (i == text.length() || text[i] == '\n') {
            lines.push_back(text.substring(start, i));
            start = i + 1;
        }
    }

    uint16_t lineHeight = 8 * textSize;
    uint16_t totalHeight = lines.size() * lineHeight;
    int16_t startY = (display.height() - totalHeight) / 2;

    display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);

    for (size_t i = 0; i < lines.size(); i++) {
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(lines[i], 0, 0, &x1, &y1, &w, &h);

        int16_t cx = (display.width() - w) / 2;
        int16_t cy = startY + i * lineHeight;

        display.setCursor(cx, cy - y1);
        display.print(lines[i]);
    }
}