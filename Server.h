#ifndef SERVER_H
#define SERVER_H

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

class Server
{
    public:
        void run(uint16 nPort);
    
    private:
        HSteamListenSocket listenSocket;
	    HSteamNetPollGroup pollGroup;
	    ISteamNetworkingSockets *sockets;

};

#endif
