#include <MessageTranslator.h>
#include <string.h>
#include <Constants.h>

MessageTranslator::MessageTranslator()
{

}

MessageTranslator::~MessageTranslator()
{

}

bool
MessageTranslator::getShareIdReply(char *buf, int* mlen, long shareId)
{
	constexpr int len = 2*sizeof(int) + sizeof(long);
	constexpr int shoffset = 2*sizeof(int);
	*mlen = len;
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = GET_SHARE_ID_RPLY_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	memcpy(buf+shoffset, &shareId, sizeof(long));
	return true;
}

bool
MessageTranslator::getStoreIdReply(char *buf, int* mlen)
{
	constexpr int len = 2*sizeof(int);
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = STORE_TRNSCTN_ID_RPLY_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	return true;
}



bool
MessageTranslator::getReply(char *buf, int* mlen, int msgTyp)
{
	constexpr int len = 2*sizeof(int);
	memcpy(buf, &len, sizeof(int));
	memcpy(buf+sizeof(int), &msgTyp, sizeof(int));
	return true;
}
