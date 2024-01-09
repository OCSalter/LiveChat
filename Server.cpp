#include "Server.h"
#include<iostream>
#include <cassert>
#include <thread>

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
        pollIncomingMessages();
        pollConnectionStateChanges();
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
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

void Server::sendStringToClient(HSteamNetConnection conn, const char *str)
{
    sockets->SendMessageToConnection(conn, str, (uint32)strlen(str), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Server::sendStringToAll(const char *str, HSteamNetConnection except = k_HSteamNetConnection_Invalid)
{
    for (auto &c: clientMap)
    {
        if(c.first != except)
        {
            sendStringToClient(c.first, str);
        }
    }
}

void Server::pollIncomingMessages()
{
    char temp[1024];

    while(active)
    {
        ISteamNetworkingMessage *incomingMsg = nullptr;

        int numMsgs = sockets->ReceiveMessagesOnPollGroup(pollGroup, &incomingMsg, 1);

        if(numMsgs == 0){
            incomingMsg->Release();
            return;
        }
        if(numMsgs < 0){
            std::cout << "Fatal Error checking recived messages on poll group" << std::endl;
        }

        // parse message data
        auto client = clientMap.find(incomingMsg->m_conn);
        assert(client != clientMap.end());

        std::string cmd;
        cmd.assign((const char *) incomingMsg->m_pData, incomingMsg->m_cbSize);
        const char *message = cmd.c_str();
        
        
        incomingMsg->Release();

        //known commands
        if(strncmp(message, "/name", 5)==0)
        {
            const char *name = message+5;
            while(isspace(*name)){++name;}
            sprintf(temp, "%s is now %s", client->second.c_str(), name);
            sendStringToAll(temp, client->first);

            sprintf(temp, "You are now %s", name);
            sendStringToClient(client->first, temp);

            setClientName(client->first, name);
        }
        else
        {
            sprintf(temp, "%s: %s", client->second.c_str(), message);
            sendStringToAll(temp, client->first);
        }
        return;
    }
}

void Server::setClientName(HSteamNetConnection connection, const char *name)
{
    clientMap[connection] = name;
    sockets->SetConnectionName(connection, name);
}

void Server::onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info)
{
    char temp[1024];

    switch(info->m_info.m_eState)
    {
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            if(info->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
            {
                auto client = clientMap.find(info->m_hConn);
                assert(client!=clientMap.end());

                const char *debugLogAction;
                if(info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
                {
                    debugLogAction = "Problem detected locally";
                    sprintf(temp, "User %s lost connection   (%s)", client->second.c_str(), info->m_info.m_szEndDebug);
                }
                else
                {
                    debugLogAction = "Closed by peer";
                    sprintf(temp, "%s left the chat", client->second.c_str());
                }

                printf("debug action: %s, message: %s\n", debugLogAction, temp);

                clientMap.erase(client);

                sendStringToAll(temp);
            }
            else
            {
                assert( info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting );
            }

            sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting:
        {
            assert(clientMap.find(info->m_hConn) == clientMap.end());

            printf("Conneciton request from %s", info->m_info.m_szConnectionDescription);


            if(sockets->AcceptConnection(info->m_hConn) != k_EResultOK)
            {
                sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
                printf("Connection closed before it could be accepted");
                break;
            }

            if(!sockets->SetConnectionPollGroup(info->m_hConn, pollGroup))
            {
                sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
                printf("Failed to set connection poll group");
                break;
            }

            // Temporary Solution. We'll generate a random name just like they do it in the
            // Valve example that I'm using to learn this stuff for real.....
            // Copy Paste the stuff from there.... :(() don't like doing this)

            char name[ 64 ];
			sprintf( name, "BraveWarrior%d", 10000 + ( rand() % 100000 ) );
			sprintf( temp, "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command '/nick' we shall know thee otherwise.", name ); 
			sendStringToClient( info->m_hConn, temp ); 

            if ( clientMap.empty() )
			{
				sendStringToClient( info->m_hConn, "Thou art utterly alone." ); 
			}
			else
			{
				sprintf( temp, "%d companions greet you:", (int)clientMap.size() ); 
				for ( auto &c: clientMap )
				{
                    sendStringToClient( info->m_hConn, c.second.c_str() );
                } 
			}

            sprintf( temp, "Hark!  A stranger hath joined this merry host.  For now we shall call them '%s'", name ); 
			sendStringToAll( temp, info->m_hConn ); 
			clientMap[ info->m_hConn ];
			setClientName( info->m_hConn, name );
			break;
        }
        default:
            break;
    }
}
