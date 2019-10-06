#pragma once

#include "User.h"
#include "ServerGameDesk.h"
#include "BLUENetWorkModelHelper.h"

class CServerGameDesk;

/*
类名：服务器端用户类
说明：在服务端用于保存和操作标识游戏中的用户
*/
class CServerUser : public CUser
{
	friend class CServerUserManager;
	friend class CServerGameDesk;

public:
	/*
	枚举名：用户状态枚举
	功能：略
	说明：无
	*/
	enum SERVERUSERSTATE
	{
		connect,          //用户已经连接
		login,               //用户已经登陆（必须先连接后才能登陆）
		room,               //用户已经在房间内（必须先登陆）
		game,              //用户已经在游戏中（必须先在房间内）
		death              //用户已经死亡
	};


private:
	//只有友元CServerUserManager类对象才可以创建和销毁本类对象
	CServerUser(IBLUEServerNetWorkSession* pSNWSession);
	virtual ~CServerUser(void);

public:
	SERVERUSERSTATE GetState(void) const {return m_state;}
	void SetState(SERVERUSERSTATE state)
	{
		BLUEASSERT(
			state == connect ||
			state == login ||
			state == room ||
			state == game ||
			state == death
			);
		m_state = state;
	}
	bool IsPrepareGame(void) const {return m_bPerpareGame;}
	CServerGameDesk* GetRoom(void) const {return m_pRoom;}
	void SetGameOver(bool bOver) {m_bIsGameOver = bOver;}
	bool IsGameOver(void) const {return m_bIsGameOver;}

	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod) const
	{
		BLUEASSERT(m_pSNWSession);
		m_pSNWSession->SendData(&iod);
	}

private:
	/*
	变量名：用户状态
	功能：略
	说明：无
	*/
	SERVERUSERSTATE m_state;

	/*
	变量名：会话指针
	功能：用于保存该用户对应的网络会话
	说明：无
	*/
	IBLUEServerNetWorkSession* const m_pSNWSession;

	/*
	变量名：用户所在的房间指针
	功能：略
	说明：只有在状态为room和game时才不为BLUENULL
	*/
	CServerGameDesk* m_pRoom;

	/*
	变量名：是否已经游戏失败（该失败不表示真的输了）
	功能：略
	说明：无
	*/
	bool m_bIsGameOver;
};
