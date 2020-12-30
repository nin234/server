#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <unordered_map>
#include <string>
#include <map>

class Config
{
	int OpenHousesThrds;
	int AutoSpreeThrds;
	int EasyGrocThrds;
	int SmartMsgThrds;
	int mq_maxmsg;
	int mq_msgsize;
	bool bSandbox;
	std::unordered_map<std::string, std::string> keyvals;
	bool m_bUseDB;
	bool m_bUseRocksDB;
    int m_nStartSSLPort;
    int m_nShareIdStart;
    int m_nShareIdEnd;
    std::map<std::string, std::pair<std::string, int>> appConns;    
    void loadAppConns();

	public:
		Config();
		virtual ~Config();
		static Config& Instance();
		int getOpenHousesThrds();
		int getAutoSpreeThrds();
		int getEasyGrocThrds();
		int getSmartMsgThrds();
		int getMqMaxMsg();
		int getMqMsgSize();
		bool sandBox();
		bool useDB();
		bool useRocksDB();
        int getStartSSLPort();
        int getStartShareId();
        int getEndShareId();
        std::map<std::string, std::pair<std::string, int>> getAppConns();
        
};
#endif
