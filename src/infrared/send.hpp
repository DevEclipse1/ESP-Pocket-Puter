#pragma once

#include "../global.hpp"
#include "../display_utils.h"

uint32_t swap32(uint32_t value) {
    return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

uint64_t reverse_bits(uint64_t input, uint16_t nbits) {
    if (nbits <= 1) return input;

    nbits = std::min(nbits, (uint16_t)(sizeof(input) * 8));
    uint64_t output = 0;
    for (uint16_t i = 0; i < nbits; i++) {
      output <<= 1;
      output |= (input & 1);
      input >>= 1;
    }

    return (input << nbits) | output;
  }

void IR_SendSamsung(String address, String command) {
    display.clearDisplay();
    Display_PrintCentered("Sending Samsung\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    uint8_t addressValue = strtoul(address.substring(0, 2).c_str(), nullptr, 16);
    uint8_t commandValue = strtoul(command.substring(0, 2).c_str(), nullptr, 16);
    uint64_t data = irtx.encodeSAMSUNG(addressValue, commandValue);

    irtx.sendSAMSUNG(data, 32);
}

void IR_SendSony(String address, String command, uint8_t nbits) {
    display.clearDisplay();
    Display_PrintCentered("Sending Sony\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    address.replace(" ", "");
    command.replace(" ", "");

    uint16_t addressValue = strtoul(address.c_str(), nullptr, 16);
    uint16_t commandValue = strtoul(command.c_str(), nullptr, 16);

    uint32_t data = irtx.encodeSony(nbits, commandValue, addressValue, 0);

    irtx.sendSony(data, nbits, 2);
}

void IR_SendRC5(String address, String command)
{
    display.clearDisplay();
    Display_PrintCentered("Sending RC5\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    uint8_t addressValue = strtoul(address.substring(0, 2).c_str(), nullptr, 16);
    uint8_t commandValue = strtoul(command.substring(0, 2).c_str(), nullptr, 16);
    uint16_t data = irtx.encodeRC5(addressValue, commandValue);
    irtx.sendRC5(data, 13);
}

void IR_SendRC6(String address, String command)
{
    display.clearDisplay();
    Display_PrintCentered("Sending RC6\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    address.replace(" ", "");
    command.replace(" ", "");
    uint32_t addressValue = strtoul(address.substring(0, 2).c_str(), nullptr, 16);
    uint32_t commandValue = strtoul(command.substring(0, 2).c_str(), nullptr, 16);
    uint64_t data = irtx.encodeRC6(addressValue, commandValue);

    irtx.sendRC6(data, 20);
}

void IR_SendNECExt(String address, String command)
{
    display.clearDisplay();
    Display_PrintCentered("Sending NECext\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    int first_zero_byte_pos = address.indexOf("00", 2);
    if (first_zero_byte_pos != -1) address = address.substring(0, first_zero_byte_pos);
    first_zero_byte_pos = command.indexOf("00", 2);
    if (first_zero_byte_pos != -1) command = command.substring(0, first_zero_byte_pos);

    address.replace(" ", "");
    command.replace(" ", "");

    uint16_t addressValue = strtoul(address.c_str(), nullptr, 16);
    uint16_t commandValue = strtoul(command.c_str(), nullptr, 16);

    uint16_t newAddress = (addressValue >> 8) | (addressValue << 8);
    uint16_t newCommand = (commandValue >> 8) | (commandValue << 8);

    uint16_t lsbAddress = reverse_bits(newAddress, 16);
    uint16_t lsbCommand = reverse_bits(newCommand, 16);

    uint32_t data = ((uint32_t)lsbAddress << 16) | lsbCommand;

    irtx.sendNEC(data, 32);
}

void IR_SendNEC(String address, String command)
{
    display.clearDisplay();
    Display_PrintCentered("Sending NEC\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    uint16_t addressValue = strtoul(address.substring(0, 2).c_str(), nullptr, 16);
    uint16_t commandValue = strtoul(command.substring(0, 2).c_str(), nullptr, 16);
    uint64_t data = irtx.encodeNEC(addressValue, commandValue);
    irtx.sendNEC(data, 32);
}

void fn_send_nec_code(uint32_t cmd)
{
    irtx.begin();
    pinMode(IR_TX, OUTPUT);
    irtx.sendNEC(cmd, 32);
}

void IR_SendRaw(uint16_t frequency, String rawData)
{
    display.clearDisplay();
    Display_PrintCentered("Sending Raw\n%d\n%s", frequency, rawData.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    uint16_t dataBufferSize = 1;
    for (int i = 0; i < rawData.length(); i++) {
        if (rawData[i] == ' ') dataBufferSize += 1;
    }
    uint16_t *dataBuffer = (uint16_t *)malloc((dataBufferSize) * sizeof(uint16_t));

    uint16_t count = 0;
    while (rawData.length() > 0 && count < dataBufferSize) {
        int delimiterIndex = rawData.indexOf(' ');
        if (delimiterIndex == -1) { delimiterIndex = rawData.length(); }
        String dataChunk = rawData.substring(0, delimiterIndex);
        rawData.remove(0, delimiterIndex + 1);
        dataBuffer[count++] = (dataChunk.toInt());
    }

    irtx.sendRaw(dataBuffer, count, frequency);

    free(dataBuffer);
}

void IR_SendKaseikyo(String address, String command) {
    display.clearDisplay();
    Display_PrintCentered("Sending Kaseikyo\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    address.replace(" ", "");
    command.replace(" ", "");

    uint32_t addressValue = strtoul(address.c_str(), nullptr, 16);
    uint16_t commandValue = strtoul(command.c_str(), nullptr, 16);

    uint8_t id = (addressValue >> 24) & 0x03;
    uint16_t vendor_id = (addressValue >> 8) & 0xFFFF;
    uint8_t genre1 = (addressValue >> 4) & 0x0F;
    uint8_t genre2 = addressValue & 0x0F;

    uint8_t data[6];
    data[0] = vendor_id & 0xFF;
    data[1] = vendor_id >> 8;

    uint8_t vendor_parity = data[0] ^ data[1];
    vendor_parity = (vendor_parity & 0x0F) ^ (vendor_parity >> 4);

    data[2] = (vendor_parity & 0x0F) | (genre1 << 4);
    data[3] = (genre2 & 0x0F) | ((commandValue & 0x0F) << 4);
    data[4] = (id << 6) | (commandValue >> 4);
    data[5] = data[2] ^ data[3] ^ data[4];

    uint64_t frame = 0;
    for (int i = 0; i < 6; i++) {
        frame |= (uint64_t)data[i] << (8 * i);
    }

    uint64_t frame_msb = reverse_bits(frame, 48);

    irtx.sendPanasonic64(frame_msb, 48);
}

void IR_SendRCA(String address, String command) {
    display.clearDisplay();
    Display_PrintCentered("Sending RCA\n%s\n%s", address.c_str(), command.c_str());
    display.display();

    irtx.begin();
    pinMode(IR_TX, OUTPUT);

    address.replace(" ", "");
    command.replace(" ", "");

    uint8_t addressValue = strtoul(address.c_str(), nullptr, 16);
    uint8_t commandValue = strtoul(command.c_str(), nullptr, 16);

    uint8_t address_inv = ~addressValue;
    uint8_t command_inv = ~commandValue;

    uint32_t rca_data = 0;
    rca_data |= (addressValue & 0x0F);
    rca_data |= (commandValue & 0xFF) << 4;
    rca_data |= (address_inv & 0x0F) << 12;
    rca_data |= (command_inv & 0xFF) << 16;

    const uint16_t preamble_mark = 4000;
    const uint16_t preamble_space = 4000;
    const uint16_t bit_mark = 500;
    const uint16_t one_space = 2000;
    const uint16_t zero_space = 1000;

    uint16_t raw[50];
    size_t index = 0;

    raw[index++] = preamble_mark;
    raw[index++] = preamble_space;

    for (int i = 23; i >= 0; i--) {
        raw[index++] = bit_mark;
        raw[index++] = (rca_data & (1UL << i)) ? one_space : zero_space;
    }

    irtx.sendRaw(raw, index, 38);
}
