#pragma once

#include "BLUEServerNetWorkModel.h"
#include <set>
#include "ServerUser.h"

/**
类名：服务器端用户管理者
功能：用于对登录上服务器的用户进行管理
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CServerUserManager
{
	typedef std::set<CServerUser*> SERVERUSERCOLL;

public:
	CServerUserManager(void);
	~CServerUserManager(void);

public:
	CServerUser* CreateServerUser(IBLUEServerNetWorkSession* pSNWSession);
	void DestroyServerUser(CServerUser* pServerUser);

	/*
	函数名：给所有用户发送信息
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod);

	/*
	函数名：给所有用户发送信息（除了pExcludeServerUser的用户）
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const CServerUser* pExcludeServerUser);
	
	/*
	函数名：给所有用户发送信息（除了pExcludeName用户名的用户）
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const BLUEString& pExcludeName);

	/*
	函数名：每个用户执行特定操作
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	template<class FN>
	void ForEach(FN fn)
	{
		SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
		while (it != itEnd)
		{
			fn(*it);
			it++;
		}
	}

	/*
	函数名：根据用户名获取用户对象
	功能：略
	说明：无
	参数：无
	返回值：用户对象指针，如果用户不存在则返回BLUENULL
	*/
	CServerUser* GetUser(const BLUEString& strName);

	/*
	函数名：用户登录
	功能：略
	说明：无
	参数：serverUser 登录对象
	　　　strName 登录用户名
	　　　strPassword 登录密码
	返回值：无
	*/
	bool ServerUserLogin(CServerUser& serverUser, const BLUEString& strName, const BLUEString& strPassword);


private:
	void ClearServerUser(void);

private:
	SERVERUSERCOLL m_serverUserColl;

};
