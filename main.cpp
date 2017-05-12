#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <ConnMgr.h>
#include <sys/select.h>
#include <AppMgr.h>
#include <system_error>

int
main()
{
	std::cout << "Starting up server " << std::endl;
		//create threads and wait for socket connections	
	//

	try
	{
		
		AppMgr appMgr;
		ConnMgr appConns;
		appConns.initializeListeners();

		for(;;)
		{
				//get the socket fd and push it into the worker thread select loop
			auto appIdConnMp = appConns.waitAndGetConnections();
			appMgr.setNewClientConnections(appIdConnMp);
			
		}

	}
	catch (std::system_error& error)
	{
		std::cout << "System Error: " << error.code() << " - " << error.what() << std::endl;
	}
	catch(std::runtime_error& error)
	{
		std::cout << "Run time Error: " <<  error.what() << std::endl;

	}
	catch(std::exception& error)
	{
		std::cout << "Caught standard exception " << error.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Caught unknown exception " << std::endl;
	}
}
