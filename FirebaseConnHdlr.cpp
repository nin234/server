#include <FirebaseConnHdlr.h>
#include <stdexcept>
#include <iostream>
#include <string.h>


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
    message_id = 0;
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
        hdlr->bConnected = true;
    }
    else
    {
        xmpp_conn_release(conn);
        hdlr->bConnected = false;
    }
}

int
FirebaseConnHdlr::message_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    FirebaseConnHdlr *hdlr = (FirebaseConnHdlr *)userdata;
    if (strcmp(xmpp_stanza_get_name(stanza), "message"))
    {
        std::cout << "Dropping non message stanza" << std::endl;
        return 1;
    }
     const char *pMsgId = xmpp_stanza_get_attribute(stanza, "message_id");
    if (pMsgId == NULL)
    {
	std::cout << "Dropping message with no message_id " << std::endl;
        return 1;
    }
    const char *pMsgTyp = xmpp_stanza_get_attribute(stanza, "message_type");
    if (pMsgTyp == NULL)
    {
	std::cout << "Dropping message with no message_type " << std::endl;
        return 1;
     }
    if (!strcmp(pMsgTyp, "ack"))
    {
        std::cout << "Ack received for message id" << pMsgId << std::endl;
        hdlr->pendingAckTknsMp.erase(pMsgId);
        return 1;
    }

    return 1;
}

bool
FirebaseConnHdlr::send(const std::vector<std::string>& tokens)
{
    if (bConnected)
    {
        for (const std::string& token : tokens)
        {
            xmpp_stanza_t* pStanza = xmpp_stanza_new(ctx);
            xmpp_stanza_set_ns(pStanza, "google:mobile:data");
            std::string msgid = std::to_string(++message_id);
            xmpp_stanza_set_id(pStanza, msgid.c_str());
            xmpp_stanza_set_name(pStanza, "message");
            
            xmpp_stanza_set_attribute(pStanza, "to", token.c_str());
            std::cout << "Sending to token to firebase=" << token << " msgid=" << msgid << std::endl;
            xmpp_send(conn, pStanza);
            pendingAckTknsMp[msgid] = token;
        }

    }
    else
    {
        tknsToSend.insert(tknsToSend.end(), tokens.begin(), tokens.end());
    }
    return true;
}

void
FirebaseConnHdlr::getSendEvents()
{
    if (!bConnected)
    {
        connect();
    }
    xmpp_run_once(ctx, 1);
    if (bConnected)
    {
        if (tknsToSend.size())
        {
            std::vector<std::string> tokens;
            tokens.swap(tknsToSend);
            send(tokens);
        }
    }
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
    xmpp_conn_set_jid(conn, jid.c_str());
    xmpp_conn_set_pass(conn, pass.c_str());
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