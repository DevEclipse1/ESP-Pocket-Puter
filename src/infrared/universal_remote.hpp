#pragma once

#include "../global.hpp"
#include <vector>

enum RemoteType {
    REMOTE_TYPE_TV,
    REMOTE_TYPE_PROJECTOR,
    REMOTE_TYPE_AC,
    REMOTE_TYPE_SPEAKERS,
};

enum RemoteProtocol {
    REMOTE_PROTOCOL_NEC,
    REMOTE_PROTOCOL_NEC_EXT,
    REMOTE_PROTOCOL_RC5,
    REMOTE_PROTOCOL_RC6,
    REMOTE_PROTOCOL_RCA,
    REMOTE_PROTOCOL_SAMSUNG,
    REMOTE_PROTOCOL_SONY,
    REMOTE_PROTOCOL_KASEIKYO,
    REMOTE_PROTOCOL_RAW
};

struct RemoteCommand {
    String name;
    RemoteProtocol protocol;
    String address;
    String command;
    uint16_t nbits;
    uint16_t raw_frequency;
    String raw_data;

    RemoteCommand(String n, RemoteProtocol p, String addr, String cmd, uint16_t bits = 0)
        : name(n), protocol(p), address(addr), command(cmd), nbits(bits), raw_frequency(0), raw_data("") {}

    RemoteCommand(String n, RemoteProtocol p, uint16_t freq, String raw)
        : name(n), protocol(p), address(""), command(""), nbits(0), raw_frequency(freq), raw_data(raw) {}
};

struct Remote {
    String brand;
    RemoteType type;
    std::vector<RemoteCommand> commands;

    Remote(String b, RemoteType t, std::initializer_list<RemoteCommand> cmds)
        : brand(b), type(t), commands(cmds) {}
};

void IR_UniversalRemote(RemoteType type);
