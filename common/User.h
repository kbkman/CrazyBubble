#pragma once

#include "BLUEDef.h"

/**
类名：用户类
*/
class CUser
{
public:
	CUser(void);
	virtual ~CUser(void);


public:
	/*
	函数名：获取用户名
	*/
	const BLUEString& GetName(void) const {return m_strName;}

	/*
	变量名：是否准备好开始游戏
	功能：略
	说明：无
	*/
	bool IsPerpareGame(void) const {return m_bPerpareGame;}

	/*
	函数名：设置是否准备好开始游戏
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SetPerpareGame(bool bPerpare) {m_bPerpareGame = bPerpare;}


protected:
	/*
	成员名：用户名
	说明：用户的登录名
	*/
	BLUEString m_strName;

	/*
	变量名：是否准备好开始游戏
	功能：略
	说明：无
	*/
	bool m_bPerpareGame;
};
