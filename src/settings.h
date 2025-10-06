#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <map>

std::map<String, String> ReadConfig(const char* path)
{
    std::map<String, String> config;
    if (!LittleFS.begin()) return config;
    if (!LittleFS.exists(path)) return config;
    File file = LittleFS.open(path, "r");
    if (!file) return config;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) continue;
        int eq = line.indexOf('=');
        if (eq <= 0) continue;
        String key = line.substring(0, eq);
        String value = line.substring(eq + 1);
        key.trim();
        value.trim();
        config[key] = value;
    }
    file.close();
    return config;
}

bool WriteConfig(const char* path, const std::map<String, String>& config)
{
    if (!LittleFS.begin()) return false;
    File file = LittleFS.open(path, "w");
    if (!file) return false;
    for (auto& it : config) {
        file.print(it.first);
        file.print("=");
        file.println(it.second);
    }
    file.close();
    return true;
}