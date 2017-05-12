#include <FirebaseConnHdlr.h>

FirebaseConnHdlr::FirebaseConnHdlr()
{
	/* create a context */
	log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
	ctx = xmpp_ctx_new(NULL, log);

	//create a connection	
	conn = xmpp_conn_new(ctx);
}

FirebaseConnHdlr::~FirebaseConnHdlr()
{

}

void
FirebaseConnHdlr::initialize()
{
	xmpp_initialize();
	return;
}
