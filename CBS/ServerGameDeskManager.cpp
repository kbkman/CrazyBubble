#include "StdAfx.h"
#include "CommunicateProtocol.h"
#include "ServerGameDeskManager.h"

CServerGameDeskManager::CServerGameDeskManager(CBLUENetWorkIODataPackageManager& nwIODPM) :
m_nwIODPM(nwIODPM)
{
	m_gameDeskColl.insert(std::make_pair(1, new CServerGameDesk(m_nwIODPM, _BLUET("普通房间一"))));
	m_gameDeskColl.insert(std::make_pair(2, new CServerGameDesk(m_nwIODPM, _BLUET("无道具房间"), 3, 0)));
	m_gameDeskColl.insert(std::make_pair(3, new CServerGameDesk(m_nwIODPM, _BLUET("全道具房间"), 3, 100)));
	m_gameDeskColl.insert(std::make_pair(4, new CServerGameDesk(m_nwIODPM, _BLUET("高难度挑战房间"), 8, 10)));
	m_gameDeskColl.insert(std::make_pair(5, new CServerGameDesk(m_nwIODPM, _BLUET("普通房间二"))));
}

CServerGameDeskManager::~CServerGameDeskManager(void)
{
	//删除所有房间
	GAMEDESKCOLL::iterator it = m_gameDeskColl.begin(), itEnd = m_gameDeskColl.end();
	while (it != itEnd)
	{
		delete it->second;
		it++;
	}
}

CServerGameDesk* CServerGameDeskManager::GetGameDesk(const int nID)
{
	//查找是否有该ID号的房间
	GAMEDESKCOLL::iterator it = m_gameDeskColl.find(nID);
	if (it != m_gameDeskColl.end())
		return it->second;
	//不存在该ID的房间
	return BLUENULL;
}

void CServerGameDeskManager::SendRLToUser(const CServerUser& serverUser)
{
	CBLUENetWorkIODataPackage* pIOD;

	//发送RLBEGIN消息
	pIOD = m_nwIODPM.CreateStoreNWIOData();
	pIOD->SetOrder(CPORDER_RLBEGIN);
	serverUser.SendNetWorkDate(*pIOD);
	pIOD->Release();

	//发送每个房间消息
	GAMEDESKCOLL::iterator it = m_gameDeskColl.begin(), itEnd = m_gameDeskColl.end();
	while (it != itEnd)
	{
		pIOD = m_nwIODPM.CreateStoreNWIOData();
		pIOD->SetBaseData(CPORDER_ROOMINFO, it->first, it->second->GetCurClientNum());
		(*pIOD) << it->second->GetMaxClientNum() << it->second->GetName();
		serverUser.SendNetWorkDate(*pIOD);
		pIOD->Release();
		it++;
	}

	//发送RLEND消息
	pIOD = m_nwIODPM.CreateStoreNWIOData();
	pIOD->SetOrder(CPORDER_RLEND);
	serverUser.SendNetWorkDate(*pIOD);
	pIOD->Release();
}