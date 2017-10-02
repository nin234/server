#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <ConnMgr.h>
#include <sys/select.h>
#include <AppMgr.h>
#include <system_error>
#include <ArchiveMgr.h>
#include <sys/time.h>
#include <sys/resource.h>

int
main()
{
	struct rlimit lm;
	if (!getrlimit(RLIMIT_NOFILE, &lm))
	{
		std::cout << "No of open files " << lm.rlim_cur << " " << lm.rlim_max << " " << __FILE__ << ":" <<__LINE__ << std::endl;
	}
	std::cout << "Starting up server " << std::endl;
		//create threads and wait for socket connections	
	//

	try
	{
		
		AppMgr appMgr;
		std::cout << "Constructed AppMgr " << " " << __FILE__ << ":"<< __LINE__ << std::endl;
		ConnMgr appConns;
		std::cout << "Constructed ConnMgr " << " " << __FILE__ << ":"<< __LINE__ << std::endl;
		appConns.initializeListeners();
		std::cout << "Initialized listeners " << " " << __FILE__ << ":"<< __LINE__ << std::endl;
		pthread_t tid;
		if (pthread_create(&tid, NULL, &ArchiveMgr::entry, &ArchiveMgr::Instance()) != 0)
		{
			std::cout << "Failed to create ArchiveMgr message processor thread exiting ... " << __FILE__ << ":" << __LINE__ << std::endl;
		}

		for(;;)
		{
				//get the socket fd and push it into the worker thread select loop
			auto appIdConnMp = appConns.waitAndGetConnections();
			appMgr.setNewClientConnections(appIdConnMp);
			
		}

	}
	catch (std::system_error& error)
	{
		std::cout << "Caught exception System Error: " << error.code() << " - " << error.what() << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
