#include <Worker.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <system_error>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <typeinfo>
#include <ShareIdMgr.h>

Worker::Worker():m_pMsgProcessor{}
{

}

Worker::~Worker()
{

}

void*
Worker::entry(void *context)
{

	return reinterpret_cast<Worker*>(context)->main();
}

void*
Worker::main()
{
	
	ShareIdMgr::Instance().initThreadShareId();
	
	try		
	{
		m_pMsgProcessor->process();
	}
	catch(std::system_error& error)
	{
		std::cout << "System Error: " << error.code() << " - " << error.what() << std::endl;
	}
	catch(std::exception& error)
        {
                std::cout << "Caught standard exception " << error.what() << std::endl;
        }
        catch(...)
        {
                std::cout << "Caught unknown exception " << std::endl;
        }       
	return this;
}

bool
Worker::addFd(int fd)
{
	return m_pMsgProcessor->addFd(fd);
}

void 
Worker::setMaxFd(int mfd)
{
	m_pMsgProcessor->setMaxFd(mfd);
	return;
}

void
Worker::setMsgProcessor(MessageProcessor *pMsgProcessor)
{
	m_pMsgProcessor.reset(pMsgProcessor);
	return;
}


