#include "universal_remote.hpp"

#include "remotes/tvs.hpp"
#include "remotes/acs.hpp"
#include "remotes/projectors.hpp"

#include "../menu.hpp"
#include "send.hpp"

#include "../display_utils.h"

struct RemoteList {
    const Remote* remotes;
    size_t count;
};

const RemoteList ALL_REMOTES[] = {
    { TVS_REMOTES, sizeof(TVS_REMOTES) / sizeof(Remote) },
    { PROJECTORS_REMOTES, sizeof(PROJECTORS_REMOTES) / sizeof(Remote) },
    //{ ACS_REMOTES, sizeof(ACS_REMOTES) / sizeof(Remote) },
};

void IR_RemoteHandler(Remote* remote)
{
    Menu menu;
    for (auto& cmd : remote->commands) {
        menu.AddItem(MenuItem(cmd.name, [&]() {
            if (cmd.protocol == REMOTE_PROTOCOL_NEC) {
                IR_SendNEC(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_NEC_EXT) {
                IR_SendNECExt(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_RC5) {
                IR_SendRC5(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_RC6) {
                IR_SendRC6(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_RCA) {
                IR_SendRCA(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_SAMSUNG) {
                IR_SendSamsung(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_SONY) {
                IR_SendSony(cmd.address, cmd.command, cmd.nbits);
            } else if (cmd.protocol == REMOTE_PROTOCOL_KASEIKYO) {
                IR_SendKaseikyo(cmd.address, cmd.command);
            } else if (cmd.protocol == REMOTE_PROTOCOL_RAW) {
                IR_SendRaw(cmd.raw_frequency, cmd.raw_data);
            }
        }));
    }

    bool exit_menu = false;
    menu.AddItem(MenuItem("[Exit]", [&]() { exit_menu = true; }));

    menu.Revive();
    while (!exit_menu) {
        menu.HandleButtons();
        menu.Render();
    }
}

void IR_UniversalRemote(RemoteType type)
{
    Menu menu;
    Remote* selected_remote = nullptr;

    if (type < 0 || type >= (sizeof(ALL_REMOTES) / sizeof(RemoteList))) {
        return;
    }

    const RemoteList& list = ALL_REMOTES[type];

    for (size_t i = 0; i < list.count; i++) {
        const Remote& remote = list.remotes[i];
        menu.AddItem(MenuItem(remote.brand, [&]() {
            selected_remote = (Remote*)&remote;
        }));
    }

    bool exit_menu = false;
    menu.AddItem(MenuItem("[Exit]", [&]() { exit_menu = true; }));

    menu.Revive();
    while (selected_remote == nullptr && !exit_menu) {
        menu.HandleButtons();
        menu.Render();
    }

    if (selected_remote != nullptr) {
        IR_RemoteHandler(selected_remote);
        HaltTillRelease(BUTTON_CENTER);
    }
}
