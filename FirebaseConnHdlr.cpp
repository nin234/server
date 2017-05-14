#include <FirebaseConnHdlr.h>
#include <stdexcept>
#include <iostream>


FirebaseConnHdlr::FirebaseConnHdlr(const std::string& jabberId, const std::string& password)
{
	/* create a context */
	log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
	ctx = xmpp_ctx_new(NULL, log);
    if (ctx == NULL)
    {
        throw std::runtime_error("Failed to create XMPP context");
    }

	
    jid = jabberId;
    pass = password;
    bConnected = false;
   }

FirebaseConnHdlr::~FirebaseConnHdlr()
{
    xmpp_ctx_free(ctx);
    /* final shutdown of the library */
    xmpp_shutdown();
}

void
FirebaseConnHdlr::conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
                  const int error, xmpp_stream_error_t * const stream_error,
                  void * const userdata)
{
    FirebaseConnHdlr *hdlr = (FirebaseConnHdlr *)userdata;
    if (status == XMPP_CONN_CONNECT)
    {
        xmpp_handler_add(conn, message_handler, "google:mobile:data", NULL, NULL, userdata);
        bConnected = true;
    }
    else
    {
        xmpp_conn_release(conn);
        bConnected = false;
    }
}

int
FirebaseConnHdlr::message_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    
    return 1;
}

void
FirebaseConnHdlr::getSendEvents()
{
    if (!bConnected)
    {
        connect();
    }
    xmpp_run_once(ctx, 1);
}

bool
FirebaseConnHdlr::connect()
{
    //create a connection
    conn = xmpp_conn_new(ctx);
    if (conn == NULL)
    {
        std::cout << "Failed to create XMPP connection object" << std::endl;
        return false;
    }
    long flags = 0;
    flags |= XMPP_CONN_FLAG_MANDATORY_TLS;
    xmpp_conn_set_flags(conn, flags);
    xmpp_conn_set_jid(conn, jid);
    xmpp_conn_set_pass(conn, pass);
    if  (xmpp_connect_client(conn, "fcm-xmpp.googleapis.com", 5236, conn_handler, this) == -1)
    {
        std::cout << "Failed to connect to google firebase ccs" << std::endl;
        return false;
    }
    

    return true;
}

void
FirebaseConnHdlr::initialize()
{
	xmpp_initialize();
	return;
}
