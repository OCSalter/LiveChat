#include "Server.h"
#include<iostream>

void Server::run(uint16 nPort){
    sockets = SteamNetworkingSockets();

    SteamNetworkingIPAddr serverLocalAddress;
	serverLocalAddress.Clear();
	serverLocalAddress.m_port = nPort;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusChangedCallback);
    listenSocket = sockets->CreateListenSocketIP(serverLocalAddress, 1, &opt);

    if(listenSocket == k_HSteamListenSocket_Invalid)
    {
        std::cout << "FAILED TO CREATE LISTEN SOCKET ON PORT: " << nPort << std::endl;
        return;
    }
    pollGroup = sockets->CreatePollGroup();
    if(pollGroup == k_HSteamNetPollGroup_Invalid)
    {
        std::cout << "FAILED TO LISTEN TO POLL GROUP ON PORT: " << nPort << std::endl;
        return;
    }

    std::cout <<"Listening on port: " << nPort << std::endl;

    bool active = true;
    while(true)
    {
        // TODO: Server polling
        active = false;
    }

    std::cout << "Closing Connections" << std::endl;

    for (auto client : clientMap)
    {
        std::cout << client.first << " Server is shutting down..." << std::endl;

        sockets->CloseConnection(client.first, 0, "Server Shutdown", true);
    }

    clientMap.clear();

    sockets->CloseListenSocket(listenSocket);
    listenSocket = k_HSteamListenSocket_Invalid;
    sockets->DestroyPollGroup(pollGroup);
    pollGroup = k_HSteamNetPollGroup_Invalid;
}