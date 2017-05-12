#ifndef _FIREBASE_CONN_HDLR_H_
#define _FIREBASE_CONN_HDLR_H_

#include <strophe.h>

class FirebaseConnHdlr
{
 	xmpp_ctx_t *ctx;	
	xmpp_conn_t *conn;
	xmpp_log_t *log;

	public:
		FirebaseConnHdlr();
		~FirebaseConnHdlr();
		static void initialize();
};

#endif
