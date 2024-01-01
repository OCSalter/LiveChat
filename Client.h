#ifndef CLIENT_H
#define CLIENT_H

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <queue>
#include <string>

class Client
{
    public:
        void run(const SteamNetworkingIPAddr &serverAddress);
        void sendMessageFromClient(std::string messageText);
        bool reciveMessageFromClient(SteamNetworkingMessage_t **outMessage);
    private:
        HSteamNetConnection connection;
	    ISteamNetworkingSockets *sockets;
        std::queue<ISteamNetworkingMessage> messageBuffer;
        bool active = true;

        void processLiveChat();
        void pollConnectionChange();
        void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);
        void pollMessages();
        bool reciveMessage(ISteamNetworkingMessage *incommingMessage);

        static Client *callbackInstance;
        static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *info)
        {
            callbackInstance -> onConnectionStatusChanged(info);
        }
};

Client *Client::callbackInstance = nullptr;

#endif
