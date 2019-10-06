#pragma once

#include "BLUENetWorkModelHelper.h"
#include "Room.h"
#include "ServerUser.h"
#include <vector>

#define DESK_MAX_USERNUMBER                6    //每个游戏桌最多人数

class CServerUser;
/**
类名：游戏桌
功能：略
说明：每个游戏都必须在一个游戏桌上进行
版本：1.0.0.1
作者：BLUE
*/

//错误列表：不出错返回值均返回>=0的整数
#define SGD_ERROR_INGAME             (-1)           //房间已经在游戏中
#define SGD_ERROR_HASUSER           (-2)           //已经存在该用户
#define SGD_ERROR_MAXUSER           (-3)           //客满
#define SGD_ERROR_NOUSER             (-4)           //无该用户
#define SGD_ERROR_HASINGAME        (-5)          //已经在游戏中
#define SGD_ERROR_LESSUSER          (-6)           //用户太少
#define SGD_ERROR_LESSPREPARE    (-7)           //不是所有用户准备好

class CServerGameDesk : public CRoom
{
	typedef std::vector<CServerUser*> ROOMSERVERUSERCOLL;
	friend class CServerGameDeskManager;

public:
	/*
	枚举名：游戏桌的状态
	功能：无
	说明：无
	*/
	enum STATE {
		normal,           //普通
		ingame           //游戏中
	};

private:
	CServerGameDesk(
		CBLUENetWorkIODataPackageManager& nwIODPM,
		const BLUEString& strName,
		BLUEDWORD dwInitRows = 3,
		int nBringToolRate = 10);
	~CServerGameDesk(void);

public:
	/*
	函数名：用户的最大ID号
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	int GetMaxServerUserID(void) const {return DESK_MAX_USERNUMBER;}

	/*
	函数名：根据ID号获取用户对象
	功能：略
	说明：不检测ID号有效性
	参数：无
	返回值：无
	*/
	CServerUser* GetServerUser(int nID) const {return m_userColl[nID];}

	/*
	函数名：根据ID号获取用户对象
	功能：略
	说明：检测ID号有效性
	参数：无
	返回值：无
	*/
	CServerUser* GetServerUserSafe(int nID) const
	{
		if (nID >= 0 && nID < DESK_MAX_USERNUMBER)
			return m_userColl[nID];
		return BLUENULL;
	}

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
		for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
			if (m_userColl[i])
				fn(m_userColl[i], (int)i);
	}

	/*
	函数名：获取游戏桌当前的状态
	功能：略
	说明：游戏桌当前状态有普通状态，和游戏中状态两种
	参数：无
	返回值：游戏桌当前的状态
	*/
	STATE GetState(void) const {return m_state;}

	/*
	函数名：游戏桌上是否有该用户
	功能：略
	说明：无
	参数：用户对象指针
	返回值：无
	*/
	bool HasUser(const CServerUser* pUser);

	/*
	函数名：获取当前房间内的人数
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	int GetUserNumber(void) const;

	/*
	函数名：添加一个玩家
	功能：略
	说明：无
	参数：无
	返回值：如果大于等于0，则表示添加玩家成功，返回值为用户在房间内的ID号，
	　　　　小于0表示出错，具体查阅SGD_ERROR_   宏
	*/
	int AddUser(CServerUser* pUser);

	/*
	函数名：移除一个玩家
	功能：略
	说明：玩家离开游戏桌，并非真正删除一个玩家对象
	参数：无
	返回值：如果大于等于0，则表示删除玩家成功，返回值为用户在房间内的ID号，
	　　　　小于0表示出错，具体查阅SGD_ERROR_   宏
	*/
	int DelUser(CServerUser* pUser);

	/*
	函数名：游戏是否已经结束
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool TestOverGame(void);


	/*
	函数名：游戏桌开始游戏
	功能：略
	说明：无
	参数：无
	返回值：>=0表示开始游戏成功
	*/
	int StartGame(void);

	/*
	函数名：给游戏桌中所有用户发送信息
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod);

	/*
	函数名：给游戏桌中所有用户发送信息（除了ID为nExcludeID的用户）
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const int nExcludeID);

	/*
	函数名：给游戏桌中所有用户发送信息（除了pExcludeServerUser的用户）
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const CServerUser* pExcludeServerUser);

private:
	/*
	变量名：IO包管理器
	功能：略
	说明：无
	*/
	CBLUENetWorkIODataPackageManager& m_nwIODPM;

	/*
	变量名：用户集合
	功能：保存每个游戏桌上的所有用户
	说明：略
	*/
	ROOMSERVERUSERCOLL m_userColl;


	/*
	变量名：游戏桌当前状态
	功能：略
	说明：略
	*/
	STATE m_state;

	/*
	变量名：游戏核心相关
	功能：略
	说明：无
	*/
	BLUEDWORD m_dwInitRows;//开始游戏时的初始行数
	int m_nBringToolRate;//游戏携带道具概率
};
