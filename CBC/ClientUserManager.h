#pragma once

#include "ClientUser.h"
#include <map>

class CClientUserManager
{
	//用户集合，以用户的用户名为键
	typedef std::map<BLUEString, CClientUser*> USERCOLL;

public:
	CClientUserManager(void);
	~CClientUserManager(void);

public:
	/*
	函数名：清空用户
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void Clear(void);

	/*
	函数名：根据用户名获取用户对象
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CClientUser* GetUser(const BLUEString& strUserName);

	/*
	函数名：添加一个用户对象
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CClientUser* AddUser(const BLUEString& strUserName);

private:
	USERCOLL m_userColl;
};
