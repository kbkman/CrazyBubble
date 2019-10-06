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
	//删除所有用户
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
	//查找是否有该用户名的用户对象
	USERCOLL::iterator it = m_userColl.find(strUserName);
	if (it != m_userColl.end())
		return it->second;
	//不存在用户名的用户对象
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