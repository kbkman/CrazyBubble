#pragma once

#include "BLUEDef.h"

/**
类名：房间类
*/
class CRoom
{
public:
	CRoom(void);
	virtual ~CRoom(void);

public:
	//房间名称
	const BLUEString& GetName(void) const {return m_strName;}
	void SetName(const BLUEString& strName) {m_strName = strName;}
	//房间人数
	int GetCurClientNum(void) const {return m_nCurClientNum;}
	int GetMaxClientNum(void) const {return m_nMaxClientNum;}

protected:
	BLUEString m_strName;//房间名称
	int m_nCurClientNum;//当前房间内人数
	int m_nMaxClientNum;//房间可以容纳总人数
};
