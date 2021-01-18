#include <MessageEnqueuer.h>
#include <iostream>

MessageEnqueuer::MessageEnqueuer()
{
	std::cout << "Created MessageEnqueuer " << __FILE__ << ":" << __LINE__ << std::endl;
}

MessageEnqueuer::~MessageEnqueuer()
{

}

void
MessageEnqueuer::enqMsg(std::shared_ptr<MsgObj> pMsg)
{
	pMsgsFdMp[pMsg->getFd()].push_back(pMsg);
	return;
}

std::shared_ptr<MsgObj>
MessageEnqueuer::getNextMsg(int fd)
{
	auto pItr = pMsgsFdMp.find(fd);
	if (pItr != pMsgsFdMp.end())
	{
		if (!pItr->second.empty())
		{
			auto pMsg = pItr->second.front();
			pItr->second.pop_front();
			if (pItr->second.empty())
			{
				pMsgsFdMp.erase(pItr);
			}
			return pMsg;
		}
	}
	return std::shared_ptr<MsgObj>() ;
}
