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
		static std::unique_ptr<char> createArchvItmMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& templList, int buflen);
		static std::unique_ptr<char> createItemMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& list, int buflen);
		static bool createPicMetaDataMsg(char *pMsg, int &len, long shareId, const std::string& name, const std::string& frndLst);
		static bool createEasyGrocDevTknMsg(char *pMsg, int& len, long shareId, const std::string& devId, const std::string& devTkn);
		static bool createShareLstMsg(char *pMsg, int& len, long shareId, const std::string& name, const std::string& val, int maxlen);
		

};

#endif
