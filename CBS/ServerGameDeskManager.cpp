#include "StdAfx.h"
#include "CommunicateProtocol.h"
#include "ServerGameDeskManager.h"

CServerGameDeskManager::CServerGameDeskManager(CBLUENetWorkIODataPackageManager& nwIODPM) :
m_nwIODPM(nwIODPM)
{
	m_gameDeskColl.insert(std::make_pair(1, new CServerGameDesk(m_nwIODPM, _BLUET("��ͨ����һ"))));
	m_gameDeskColl.insert(std::make_pair(2, new CServerGameDesk(m_nwIODPM, _BLUET("�޵��߷���"), 3, 0)));
	m_gameDeskColl.insert(std::make_pair(3, new CServerGameDesk(m_nwIODPM, _BLUET("ȫ���߷���"), 3, 100)));
	m_gameDeskColl.insert(std::make_pair(4, new CServerGameDesk(m_nwIODPM, _BLUET("���Ѷ���ս����"), 8, 10)));
	m_gameDeskColl.insert(std::make_pair(5, new CServerGameDesk(m_nwIODPM, _BLUET("��ͨ�����"))));
}

CServerGameDeskManager::~CServerGameDeskManager(void)
{
	//ɾ�����з���
	GAMEDESKCOLL::iterator it = m_gameDeskColl.begin(), itEnd = m_gameDeskColl.end();
	while (it != itEnd)
	{
		delete it->second;
		it++;
	}
}

CServerGameDesk* CServerGameDeskManager::GetGameDesk(const int nID)
{
	//�����Ƿ��и�ID�ŵķ���
	GAMEDESKCOLL::iterator it = m_gameDeskColl.find(nID);
	if (it != m_gameDeskColl.end())
		return it->second;
	//�����ڸ�ID�ķ���
	return BLUENULL;
}

void CServerGameDeskManager::SendRLToUser(const CServerUser& serverUser)
{
	CBLUENetWorkIODataPackage* pIOD;

	//����RLBEGIN��Ϣ
	pIOD = m_nwIODPM.CreateStoreNWIOData();
	pIOD->SetOrder(CPORDER_RLBEGIN);
	serverUser.SendNetWorkDate(*pIOD);
	pIOD->Release();

	//����ÿ��������Ϣ
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

	//����RLEND��Ϣ
	pIOD = m_nwIODPM.CreateStoreNWIOData();
	pIOD->SetOrder(CPORDER_RLEND);
	serverUser.SendNetWorkDate(*pIOD);
	pIOD->Release();
}