#pragma ocne

#include "../universal_remote.hpp"

const Remote PROJECTORS_REMOTES[] PROGMEM = {
    {
        "Acer",
        REMOTE_TYPE_PROJECTOR,
        {
            {"Power", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "87 78 00 00"},
            {"Source", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "8C 73 00 00"},
            {"Hide", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "8F 70 00 00"},
            {"Freeze", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "8E 71 00 00"},
            {"Menu", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "84 7B 00 00"},
            {"Up", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "82 7D 00 00"},
            {"Down", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "85 7A 00 00"},
            {"Left", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "83 7C 00 00"},
            {"Right", REMOTE_PROTOCOL_NEC_EXT, "08 13 00 00", "08 13 00 00"},
        }
    },
    {
        "Benq",
        REMOTE_TYPE_PROJECTOR,
        {
            {"Power", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "4F B0 00 00"},
            {"Off", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "4E B1 00 00"},
            {"Eco", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "07 F8 00 00"},
            {"Source", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "04 FB 00 00"},
            {"Input", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "0C F3 00 00"},
            {"Freeze", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "03 FC 00 00"},
            {"Menu", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "03 FC 00 00"},
            {"Home", REMOTE_PROTOCOL_NEC_EXT, "00 30 00 00", "1A E5 00 00"},
        }
    },
    {
        "Optoma 1",
        REMOTE_TYPE_PROJECTOR,
        {
            {"Power", REMOTE_PROTOCOL_NEC_EXT, "4F 50 00 00", "02 FD 00 00"},
            {"Freeze", REMOTE_PROTOCOL_NEC_EXT, "4F 50 00 00", "0C F3 00 00"},
            {"Hide", REMOTE_PROTOCOL_NEC_EXT, "4F 50 00 00", "03 FC 00 00"},
            {"Menu", REMOTE_PROTOCOL_NEC_EXT, "4F 50 00 00", "11 EE 00 00"},
        }
    },
    {
        "Optoma 2",
        REMOTE_TYPE_PROJECTOR,
        {
            {"Power", REMOTE_PROTOCOL_NEC_EXT, "32 00 00 00", "02 00 00 00"},
            {"Off", REMOTE_PROTOCOL_NEC_EXT, "32 00 00 00", "2E 00 00 00"},
            {"Input", REMOTE_PROTOCOL_NEC_EXT, "32 00 00 00", "18 00 00 00"},
            {"Freeze", REMOTE_PROTOCOL_NEC_EXT, "32 00 00 00", "06 00 00 00"},
            {"Info", REMOTE_PROTOCOL_NEC_EXT, "32 00 00 00", "25 00 00 00"},
            {"Menu", REMOTE_PROTOCOL_NEC_EXT, "32 00 00 00", "88 00 00 00"},
        }
    },
    {
        "Epson",
        REMOTE_TYPE_PROJECTOR,
        {
            {"Power", REMOTE_PROTOCOL_NEC_EXT, "83 55 00 00", "90 6F 00 00"},
            {"Menu", REMOTE_PROTOCOL_NEC_EXT, "83 55 00 00", "95 6A 00 00"},
            {"Freeze", REMOTE_PROTOCOL_NEC_EXT, "83 55 00 00", "92 6D 00 00"}
        }
    },
    {
        "Viewsonic",
        REMOTE_TYPE_PROJECTOR,
        {
            {"Power", REMOTE_PROTOCOL_NEC_EXT, "83 F4 00 00", "4F B0 00 00"},
            {"Off", REMOTE_PROTOCOL_NEC_EXT, "83 F4 00 00", "4E B1 00 00"},
            {"Menu", REMOTE_PROTOCOL_NEC_EXT, "83 F4 00 00", "30 CF 00 00"},
            {"Freeze", REMOTE_PROTOCOL_NEC_EXT, "83 F4 00 00", "03 FC 00 00"},
        }
    },
};