#include "Client.h"

void Client::run(const SteamNetworkingIPAddr &serverAddress)
{
    sockets = SteamNetworkingSockets();
    char address[SteamNetworkingIPAddr::k_cchMaxString];
    serverAddress.ToString(address, sizeof(address), true);
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusChangedCallback);
}

void Client::pollConnectionChange(){
    callbackInstance = this;
    sockets->RunCallbacks();
}
