#include <Distributor.h>
#include <Config.h>

Distributor::Distributor()
{

}

Distributor::~Distributor()
{

}

std::vector<std::string>
Distributor::distribute(LstObj *pLstObj)
{

    std::vector<std::string> shareIds;

    std::vector<std::string> lclShareIds;

    if (m_pTrnsl->getShareIds(pLstObj->getList(), shareIds))
    {
        for (auto shareIdStr : shareIds)
        {
            long nShareid = std::stol(shareIdStr);
            if (nShareid >= Config::Instance().getStartShareId() && nShareid <= Config::Instance().getEndShareId())
            {
                lclShareIds.push_back(shareIdStr);
            }
        }
    }
    return lclShareIds;
}

void
Distributor::setTrnsl(MessageTranslator *pTrnsl)
{
	m_pTrnsl.reset(pTrnsl);
	return;
}
