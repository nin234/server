#ifndef _ARCHIVE_MESSAGE_CREATOR_H_
#define _ARCHIVE_MESSAGE_CREATOR_H_

#include <MessageObjs.h>
#include <memory>

class ArchiveMsgCreator
{
	public:

		static bool createShareIdMsg(char *pMsg, int& len, long shareId);
		static bool createTrnIdShrIdMsg(char *pMsg, int& len, shrIdTrnId shtrId);
		static std::unique_ptr<char> createFrndLstMsg(char *pMsgStatic, int& len, long shareId, const std::string& frndLst, int buflen);
		static std::unique_ptr<char> createTemplLstMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& templList, int buflen);
		static std::unique_ptr<char> createLstMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& list, int buflen);
		static bool createEasyGrocDevTknMsg(char *pMsg, int& len, long shareId, const std::string& devId, const std::string& devTkn);
		static bool createShareLstMsg(char *pMsg, int& len, long shareId, const std::string& name, const std::string& val, int maxlen);
		

};

#endif
