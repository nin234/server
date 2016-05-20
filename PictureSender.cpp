#include <PictureSender.h>

PictureSender::PictureSender()
{


}

PictureSender::~PictureSender()
{

}

void
PictureSender::sendPictures()
{
	if (!picNamesShIds.empty())
	{

	}
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
