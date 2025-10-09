#pragma once

#include <unordered_map>

#include "protocol.h"

#include "protocols/ansonic.hpp"
#include "protocols/came.hpp"
#include "protocols/chamberlain.hpp"
#include "protocols/holtek.hpp"
#include "protocols/hormann.hpp"
#include "protocols/keeloq.hpp"
#include "protocols/linear.hpp"
#include "protocols/nice_flo.hpp"
#include "protocols/nice_flor_s.hpp"
#include "protocols/princeton.hpp"    
#include "protocols/bett.hpp"
#include "protocols/security1.hpp"
#include "protocols/security2.hpp"

Protocol RF_GetProtocol(uint16_t sample_count, uint32_t* timings, bool start_level) {
    std::vector<uint32_t> valid;
    valid.reserve(sample_count);
    for (uint16_t i = 0; i < sample_count; i++) {
        if (timings[i] > 50 && timings[i] < 10000)
            valid.push_back(timings[i]);
    }

    if (valid.size() < 4) return Protocol(0, 0, 0, 0);

    uint64_t total_high = 0;
    uint64_t total_low = 0;
    uint16_t count_high = 0;
    uint16_t count_low = 0;

    bool level = start_level;
    for (auto d : valid) {
        if (level) {
            total_high += d;
            count_high++;
        } else {
            total_low += d;
            count_low++;
        }
        level = !level;
    }

    float avg_high = count_high ? (float)total_high / count_high : 0;
    float avg_low = count_low ? (float)total_low / count_low : 0;

    float split_threshold = (avg_high + avg_low) / 2.0f;

    std::vector<uint32_t> short_pulses;
    std::vector<uint32_t> long_pulses;

    for (auto d : valid) {
        if (d < split_threshold)
            short_pulses.push_back(d);
        else
            long_pulses.push_back(d);
    }

    float avg_short = 0, avg_long = 0;
    if (!short_pulses.empty()) {
        uint64_t sum = 0;
        for (auto v : short_pulses) sum += v;
        avg_short = (float)sum / short_pulses.size();
    }
    if (!long_pulses.empty()) {
        uint64_t sum = 0;
        for (auto v : long_pulses) sum += v;
        avg_long = (float)sum / long_pulses.size();
    }

    if (avg_short > avg_long)
        std::swap(avg_short, avg_long);

    float delta = fabs(avg_long - avg_short);
    uint16_t bit_count = valid.size() / 2;

    return Protocol((int)round(avg_short), (int)round(avg_long), (int)round(delta), bit_count);
}

const Protocol protocols[] = {
    protocol_ansonic,
    protocol_bett,
    protocol_came,
    protocol_chamberlain,
    protocol_holtek,
    protocol_hormann,
    protocol_keeloq,
    protocol_linear,
    protocol_nice_flo,
    protocol_nice_flor_s,
    protocol_princeton,
    protocol_security1,
    protocol_security2,
};

const String protocol_names[] = {
    "Ansonic",
    "Bett",
    "Came",
    "Chamberlain",
    "Holtek",
    "Hormann",
    "Keeloq",
    "Linear",
    "Nice Flo",
    "Nice Flor S",
    "Princeton",
    "Security 1.0",
    "Security 2.0",
};

const size_t num_protocols = sizeof(protocols) / sizeof(Protocol);

float RF_ProtocolDistance(const Protocol& a, const Protocol& b) {
    float ds = fabs((float)a.t_short - b.t_short);
    float dh = fabs((float)a.t_high - b.t_high);
    float dd = fabs((float)a.t_delta - b.t_delta);
    return ds * 0.5f + dh * 0.4f + dd * 0.1f;
}

const Protocol* RF_FindProtocol(const Protocol& detected) {
    const Protocol* best = nullptr;
    float best_dist = 999999.0f;

    for (size_t i = 0; i < num_protocols; i++) {
        float d = RF_ProtocolDistance(detected, protocols[i]);
        if (d < best_dist) {
            best_dist = d;
            best = &protocols[i];
        }
    }
    return best;
}

String RF_FindProtocolName(const Protocol& detected) {
    const Protocol* best = RF_FindProtocol(detected);
    if (!best) return "Unknown";
    for (size_t i = 0; i < num_protocols; i++) {
        if (&protocols[i] == best) return protocol_names[i];
    }
    return "Unknown";
}
