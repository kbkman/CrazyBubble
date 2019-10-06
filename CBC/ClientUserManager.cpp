#include "StdAfx.h"
#include "ClientUserManager.h"

CClientUserManager::CClientUserManager(void)
{
}

CClientUserManager::~CClientUserManager(void)
{
	Clear();
}

void CClientUserManager::Clear(void)
{
	//ɾ�������û�
	USERCOLL::iterator it = m_userColl.begin(), itEnd = m_userColl.end();
	while (it != itEnd)
	{
		delete it->second;
		it++;
	}
	m_userColl.clear();
}

CClientUser* CClientUserManager::GetUser(const BLUEString& strUserName)
{
	//�����Ƿ��и��û������û�����
	USERCOLL::iterator it = m_userColl.find(strUserName);
	if (it != m_userColl.end())
		return it->second;
	//�������û������û�����
	return BLUENULL;
}

CClientUser* CClientUserManager::AddUser(const BLUEString& strUserName)
{
	if (GetUser(strUserName) != BLUENULL)
		return BLUENULL;

	CClientUser* pUser = new CClientUser();
	pUser->m_strName = strUserName;
	m_userColl.insert(std::make_pair(strUserName, pUser));
	return pUser;
}