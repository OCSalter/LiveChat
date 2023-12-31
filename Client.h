#ifndef CLIENT_H
#define CLIENT_H

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

class Client
{
    public:
        void run(const SteamNetworkingIPAddr &serverAddress);

    private:
        HSteamNetConnection connection;
	    ISteamNetworkingSockets *sockets;

        void pollConnectionChange();
        void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);


        static Client *callbackInstance;
        static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *info)
        {
            callbackInstance -> onConnectionStatusChanged(info);
        }
};

Client *Client::callbackInstance = nullptr;

#endif
