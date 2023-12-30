#include<iostream>

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

int main()
{
    const SteamNetworkingMicroseconds microSeconds =  SteamNetworkingUtils()->GetLocalTimestamp();
    
    std::cout << microSeconds << std::endl;
    
    std::cout << "Hello World" << std::endl;
    return 0;
}