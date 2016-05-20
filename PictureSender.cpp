#include <PictureSender.h>

PictureSender::PictureSender():m_pTrnsl(NULL)
{


}

PictureSender::~PictureSender()
{

}

void
PictureSender::setTrnsl(MessageTranslator *pTrnsl)
{
	m_pTrnsl.reset(pTrnsl);
	return;
}

void
PictureSender::sendPictures()
{
	for (const auto& picNameShId : picNamesShIds)
	{
		char archbuf[32768];
		int archlen =0 ;
		if (m_pTrnsl->getPicMetaMsg(archbuf, &archlen, 32768, picNameShId))
		{

		}
	}
	picNamesShIds.clear();
	return;
}

bool
PictureSender::shouldEnqueMsg(int fd)
{

	return false;
}

void
PictureSender::insertPicNameShid(const shrIdLstName& shidlst)
{
	picNamesShIds.push_back(shidlst);
	return;
}
