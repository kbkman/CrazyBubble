#include "StdAfx.h"
#include "ServerUserManager.h"

CServerUserManager::CServerUserManager(void)
{
}

CServerUserManager::~CServerUserManager(void)
{
	ClearServerUser();
}

CServerUser* CServerUserManager::CreateServerUser(IBLUEServerNetWorkSession* pSNWSession)
{
	CServerUser* pNewServerUser = new CServerUser(pSNWSession);
	m_serverUserColl.insert(pNewServerUser);
	return pNewServerUser;
}

void CServerUserManager::DestroyServerUser(CServerUser* pServerUser)
{
	BLUEASSERT(pServerUser);
	m_serverUserColl.erase(pServerUser);
	delete pServerUser;
}

void CServerUserManager::ClearServerUser(void)
{
	SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
	while (it != itEnd)
	{
		delete *it;
		it++;
	}
}

CServerUser* CServerUserManager::GetUser(const BLUEString& strName)
{
	SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
	BLUETRACE(_BLUET("collsize=%d\n"), m_serverUserColl.size());
	while (it != itEnd)
	{
		BLUEString str = (*it)->GetName();
		if ((*it)->GetName() == strName)
			return *it;
		it++;
	}

	return BLUENULL;
}

bool CServerUserManager::ServerUserLogin(CServerUser& serverUser, const BLUEString& strName, const BLUEString& strPassword)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::connect);

	//如果该用户名的用户没有登陆，则可以登陆
	if (GetUser(strName) == BLUENULL)
	{
		serverUser.m_strName = strName;
		serverUser.SetState(CServerUser::login);
		return true;
	}

	//否则视为重复登录
	return false;
}

void CServerUserManager::SendNetWorkDate(CBLUENetWorkIODataPackage& iod)
{
	SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
	while (it != itEnd)
	{
		//只能登陆的用户才会收到消息
		if ((*it)->GetState() != CServerUser::connect)
			(*it)->SendNetWorkDate(iod);
		it++;
	}
}

void CServerUserManager::SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const CServerUser* pExcludeServerUser)
{
	SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
	while (it != itEnd)
	{
		//只能登陆的用户才会收到消息
		if ((*it)->GetState() != CServerUser::connect && (*it) != pExcludeServerUser)
			(*it)->SendNetWorkDate(iod);
		it++;
	}
}

void CServerUserManager::SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const BLUEString& pExcludeName)
{
	SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
	while (it != itEnd)
	{
		//只能登陆的用户才会收到消息
		if ((*it)->GetState() != CServerUser::connect && (*it)->GetName() != pExcludeName)
			(*it)->SendNetWorkDate(iod);
		it++;
	}
}