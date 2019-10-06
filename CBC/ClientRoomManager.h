#pragma once

#include "ClientRoom.h"
#include <map>

class CClientRoomManager
{
	//房间集合类型，以ID和房间对象指针配对
	typedef std::map<int, CClientRoom*> ROOMCOLL;

public:
	CClientRoomManager(void);
	~CClientRoomManager(void);

public:
	/*
	函数名：通过房间ID号获取房间对象
	功能：略
	说明：
	参数：无
	返回值：无
	*/
	CClientRoom* GetRoom(const int nID);

	/*
	函数名：获取上（下）一个房间
	功能：略
	说明：根据当前迭代器获取上（下）一个房间
	参数：无
	返回值：无
	*/
	CClientRoom* GetPrevRoom(int& nID);
	CClientRoom* GetNextRoom(int& nID);

	/*
	函数名：添加一个房间
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CClientRoom* AddRoom(const int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum);

	/*
	函数名：清空所有房间
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void Clear(void);

	//重置内部迭代器
	void ResetInnerIt(void) {m_itCur = m_roomColl.begin();}

private:
	ROOMCOLL m_roomColl;
	ROOMCOLL::iterator m_itCur;//当前迭代器
};
