#include "Client.h"
#include<iostream>
#include <thread>
#include <mutex>
#include <cassert>


void Client::run(const SteamNetworkingIPAddr &serverAddress)
{
    sockets = SteamNetworkingSockets();
    char address[SteamNetworkingIPAddr::k_cchMaxString];
    serverAddress.ToString(address, sizeof(address), true);
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusChangedCallback);
    connection = sockets->ConnectByIPAddress(serverAddress, 1, &opt);
    if(connection == k_HSteamNetConnection_Invalid){
        std::cout << "ERROR: Invalid connection creation" << std::endl;
        return;
    }

    processLiveChat();
}

void Client::sendMessageFromClient(std::string messageText){
    sockets->SendMessageToConnection(connection, messageText.c_str(), (uint32)messageText.length(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Client::processLiveChat()
{
    while(active){
        pollMessages();
        pollConnectionChange();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Client::pollConnectionChange()
{
    callbackInstance = this;
    sockets->RunCallbacks();
}

void Client::pollMessages()
{
    ISteamNetworkingMessage *incommingMessage = nullptr;
    while(active && reciveMessage(incommingMessage)){
        messageBuffer.push(*incommingMessage);
        fwrite(incommingMessage->m_pData, 1, incommingMessage->m_cbSize, stdout);
        fputc( '\n', stdout );
    }
    incommingMessage->Release();
}

bool Client::reciveMessage(ISteamNetworkingMessage *messageResult)
{
    switch(sockets->ReceiveMessagesOnConnection(connection, &messageResult, 1)){
        case 0:
            return false;
            break;
        case 1:
            return true;
            break;
        default:
            std::cout << "FATAL ERROR: Error reading received messages" << std::endl;
            active = false;
            return false;
    }
}

void Client::onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info){
    
    assert(info->m_hConn == connection || k_HSteamNetConnection_Invalid );
    
    switch(info->m_info.m_eState){
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:

            active = false;

            if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)
			{
				printf("Cannot connect to remote host.  (%s)", info->m_info.m_szEndDebug);
			}
			else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
			{
				printf("Lost connection to host.  (%s)", info->m_info.m_szEndDebug);
			}
			else
			{
			    printf("Disconnected from host.  (%s)", info->m_info.m_szEndDebug);
			}
            sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
            connection = k_HSteamNetConnection_Invalid;
            break;
        case k_ESteamNetworkingConnectionState_Connected:
			printf("Connected to server OK");
			break;
        default:
            break;
    }
}
