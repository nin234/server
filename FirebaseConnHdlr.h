#ifndef _FIREBASE_CONN_HDLR_H_
#define _FIREBASE_CONN_HDLR_H_

#include <strophe.h>
#include <string>
#include <vector>

class FirebaseConnHdlr
{
 	xmpp_ctx_t *ctx;	
	xmpp_conn_t *conn;
	xmpp_log_t *log;
    std::string jid;
    std::string pass;
    bool bConnected;

	public:
    FirebaseConnHdlr(const std::string& jabberId, const std::string& password);
		~FirebaseConnHdlr();
		static void initialize();
    static void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
                                  const int error, xmpp_stream_error_t * const stream_error,
                                  void * const userdata);
                                  
    bool connect();
    bool isConnected () {return bConnected;}
    void getSendEvents();
    static int message_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata);
    bool send(const std::vector<std::string>& tokens);
};

#endif
