#include<iostream>

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include"UserInput.cpp"
#include <future>
#include <cstdarg>

std::mutex mutexUserInputQueue;
std::queue< std::string > queueUserInput;

std::thread *s_pThreadUserInput = nullptr;

SteamNetworkingMicroseconds g_logTimeZero;


bool av = true;
void LocalUserInput_Init()
{
	s_pThreadUserInput = new std::thread( []()
	{
		while ( av )
		{
			char szLine[ 4000 ];
			if ( !fgets( szLine, sizeof(szLine), stdin ) )
			{
				if ( av )
					return;
				av = false;
				
				break;
			}

			mutexUserInputQueue.lock();
			queueUserInput.push( std::string( szLine ) );
			mutexUserInputQueue.unlock();
		}
	} );
}

bool LocalUserInput_GetNext( std::string &result )
{
	bool got_input = false;
	mutexUserInputQueue.lock();
	while ( !queueUserInput.empty() && !got_input )
	{
		result = queueUserInput.front();
		queueUserInput.pop();
		got_input = !result.empty();
	}
	mutexUserInputQueue.unlock();
	return got_input;
}

int main()
{
    bool *active = &av;
    std::string cmd;

	LocalUserInput_Init();

    while(*active)
    {
        while ( *active && LocalUserInput_GetNext( cmd ))
		{
            std::cout << "me: "<< cmd;
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Bye Bye World" << std::endl;
    return 0;
}
