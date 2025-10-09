#pragma once
#include <Arduino.h>
#include <vector>
#include <algorithm>

struct Protocol {
    int t_short;
    int t_high;
    int t_delta;
    int min_bit_count;

    Protocol(int s = 0, int h = 0, int d = 0, int bits = 0)
        : t_short(s), t_high(h), t_delta(d), min_bit_count(bits) {}
};
