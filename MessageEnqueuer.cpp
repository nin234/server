#include <MessageEnqueuer.h>

MessageEnqueuer::MessageEnqueuer()
{

}

MessageEnqueuer::~MessageEnqueuer()
{

}

void
MessageEnqueuer::enqMsg(std::unique_ptr<MsgObj, MsgObjDeltr> pMsg)
{
	pMsgsFdMp[pMsg->getFd()].push_back(std::move(pMsg));
	return;
}

std::unique_ptr<MsgObj, MsgObjDeltr>
MessageEnqueuer::getNextMsg(int fd)
{
	auto pItr = pMsgsFdMp.find(fd);
	if (pItr != pMsgsFdMp.end())
	{
		if (!pItr->second.empty())
		{
			std::unique_ptr<MsgObj, MsgObjDeltr> pMsg = std::move(pItr->second.front());
			pItr->second.pop_front();
			if (pItr->second.empty())
			{
				pMsgsFdMp.erase(pItr);
			}
			return pMsg;
		}
		std::unique_ptr<MsgObj, MsgObjDeltr> pNll{};
		return pNil;
	}
	std::unique_ptr<MsgObj, MsgObjDeltr> pNll{};
	return pNil;
}
