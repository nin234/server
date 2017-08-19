#ifndef _FIREBASE_CONN_HDLR_H_
#define _FIREBASE_CONN_HDLR_H_

#include <strophe.h>
#include <string>
#include <vector>
#include <map>
#include <sys/time.h>
#include <curl/curl.h>

class FirebaseConnHdlr
{
 	xmpp_ctx_t *ctx;	
	xmpp_conn_t *conn;
	xmpp_log_t *log;
    std::string jid;
    std::string pass;
    bool bConnected;
    bool bConnecting;
    int message_id;
    std::map<std::string, std::string> pendingAckTknsMp;
    std::vector<std::string> tknsToSend;
    int backoff;
    time_t backoff_till;
	CURL *curl;

	public:
    FirebaseConnHdlr(const std::string& jabberId, const std::string& password);
		~FirebaseConnHdlr();
		static void initialize();
                                  
    bool connect();
    bool isConnected () {return bConnected;}
    void getSendEvents();
    bool send(const std::vector<std::string>& tokens, const std::string& msg);
};

#endif
