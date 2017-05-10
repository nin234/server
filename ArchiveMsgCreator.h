#ifndef _ARCHIVE_MESSAGE_CREATOR_H_
#define _ARCHIVE_MESSAGE_CREATOR_H_

#include <MessageObjs.h>
#include <memory>

class ArchiveMsgCreator
{
    static bool createCmnTemplAndItemMsg(char *pMsg, int& len, int appId, long shareId, const std::string& name, const std::string& list, int buflen, int msgId);
    static bool createCmnShareTemplAndLstMsg(char *pMsg, int& len, int appId, bool del,   long shareId, long shareIdLst, const std::string& name, int maxlen, int msgId);
    
	public:

		static bool createShareIdMsg(char *pMsg, int& len, long shareId);
		static bool createTrnIdShrIdMsg(char *pMsg, int& len, shrIdTrnId shtrId);
		static std::unique_ptr<char> createFrndLstMsg(char *pMsgStatic, int& len, long shareId, const std::string& frndLst, int buflen);
		static std::unique_ptr<char> createArchvItmMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& templList, int buflen);
		static bool createItemMsg(char *pMsg, int& len, int appId, long shareId, const std::string& name, const std::string& list, int buflen);
        static bool createTemplItemMsg(char *pMsg, int& len, int appId, long shareId, const std::string& name, const std::string& list, int buflen);
		static bool createPicMetaDataMsg(char *pMsg, int &len, long shareId, const std::string& name, const std::string& frndLst);
    static bool createDevTknMsg(char *pMsg, int& len, int appId, long shareId, const std::string& devTkn, const std::string& platform);
		static bool createShareLstMsg(char *pMsg, int& len, int appId, bool del,   long shareId, long shareIdLst, const std::string& name, int maxlen);
        static bool createShareTemplLstMsg(char *pMsg, int& len, int appId, bool del,   long shareId, long shareIdLst, const std::string& name, int maxlen);
		

};

#endif
