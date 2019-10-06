#include "StdAfx.h"
#include "ClientRoomManager.h"

CClientRoomManager::CClientRoomManager(void)
{
	m_itCur = m_roomColl.begin();
}

CClientRoomManager::~CClientRoomManager(void)
{
	Clear();
}

CClientRoom * CClientRoomManager::GetRoom(const int nID)
{
	//查找是否有该ID号的房间
	ROOMCOLL::iterator it = m_roomColl.find(nID);
	if (it != m_roomColl.end())
		return it->second;

	//不存在该ID的房间
	return BLUENULL;
}

CClientRoom* CClientRoomManager::GetPrevRoom(int& nID)
{
	if (m_itCur == m_roomColl.begin())
		return BLUENULL;

	m_itCur--;
	CClientRoom* pRet = m_itCur->second;
	nID = m_itCur->first;
	return pRet;
}

CClientRoom* CClientRoomManager::GetNextRoom(int& nID)
{
	if (m_itCur == m_roomColl.end())
		return BLUENULL;

	CClientRoom* pRet = m_itCur->second;
	nID = m_itCur->first;
	m_itCur++;
	return pRet;
}

CClientRoom* CClientRoomManager::AddRoom(const int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum)
{
	if (GetRoom(nID) != BLUENULL)
		return BLUENULL;

	CClientRoom* pRoom = new CClientRoom();
	pRoom->SetName(strName);
	pRoom->m_nCurClientNum = nCurClientNum;
	pRoom->m_nMaxClientNum = nMaxClientNum;
	m_roomColl.insert(std::make_pair(nID, pRoom));
	return pRoom;
}

void CClientRoomManager::Clear(void)
{
	//删除所有房间
	ROOMCOLL::iterator it = m_roomColl.begin(), itEnd = m_roomColl.end();
	while (it != itEnd)
	{
		delete it->second;
		it++;
	}
	m_roomColl.clear();
	m_itCur = m_roomColl.begin();
}