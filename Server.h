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
        void localUserInput(std::string cmd);
        
    private:
        bool active;
        HSteamListenSocket listenSocket;
	    HSteamNetPollGroup pollGroup;
	    ISteamNetworkingSockets *sockets;

        std::map<HSteamNetConnection, std::string> clientMap;

        void sendStringToClient(HSteamNetConnection conn, const char *str);
        void sendStringToAll(const char *std, HSteamNetConnection except = k_HSteamNetConnection_Invalid);
        void pollIncomingMessages();
        void setClientName(HSteamNetConnection conn, const char *name);
        void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);
        void pollConnectionStateChanges();

        static Server *callbackInstance;
        static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *info)
        {
            callbackInstance -> onConnectionStatusChanged(info);
        }

};

Server *Server::callbackInstance = nullptr;

#endif