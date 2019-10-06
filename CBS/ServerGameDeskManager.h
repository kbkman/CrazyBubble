#pragma once

#include "ServerGameDesk.h"
#include <map>

class CServerGameDeskManager
{
	//游戏桌集合类型，以ID和游戏桌对象指针配对
	typedef std::map<int, CServerGameDesk*> GAMEDESKCOLL;

public:
	typedef GAMEDESKCOLL::iterator GAMEDESKITERATOR;

public:
	CServerGameDeskManager(CBLUENetWorkIODataPackageManager& nwIODPM);
	~CServerGameDeskManager(void);

public:
	/*
	函数名：通过游戏桌ID号获取游戏桌对象
	功能：略
	说明：
	参数：无
	返回值：无
	*/
	CServerGameDesk* GetGameDesk(const int nID);

	/*
	函数名：外部遍历所有游戏桌的三个函数
	功能：略
	说明：
	参数：无
	返回值：无
	*/
	GAMEDESKITERATOR GetGameDeskIterator(void) {return m_gameDeskColl.begin();}
	bool IsEndGameDeskIterator(const GAMEDESKITERATOR& it) const {return it == m_gameDeskColl.end();}
	CServerGameDesk* GetNextGameDesk(GAMEDESKITERATOR& it, int& nID)
	{
		CServerGameDesk* pServerGameDesk = it->second;
		nID = it->first;
		it++;
		return pServerGameDesk;
	}

	/*
	函数名：每个游戏桌执行特定操作
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	template<class FN>
	void ForEach(FN fn)
	{
		GAMEDESKITERATOR it = m_gameDeskColl.begin(), itEnd = m_gameDeskColl.end();
		while (it != itEnd)
		{
			fn(it->first, it->second);
			it++;
		}
	}

	/*
	函数名：给一个用户发送房间列表
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SendRLToUser(const CServerUser& serverUser);

private:
	GAMEDESKCOLL m_gameDeskColl;
	CBLUENetWorkIODataPackageManager& m_nwIODPM;
};
