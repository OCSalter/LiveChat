#ifndef SERVER_H
#define SERVER_H

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <map>
#include <string>


class Server
{
    public:
        void run(uint16 nPort);
        
    private:
        HSteamListenSocket listenSocket;
	    HSteamNetPollGroup pollGroup;
	    ISteamNetworkingSockets *sockets;

        std::map<HSteamNetConnection, std::string> clientMap;

        void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);

        static Server *callbackInstance;
        static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *info)
        {
            callbackInstance -> onConnectionStatusChanged(info);
        }

};

Server *Server::callbackInstance = nullptr;

#endif