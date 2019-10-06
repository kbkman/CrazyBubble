#pragma once
#include "BLUEDirectX\engine2d.h"

/**
类名：星星动画
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCAniStar : public CDDrawSprite
{
public:
	CCBCAniStar(void);
	~CCBCAniStar(void);

public:
	void ResetEnd(const CBLUEPoint& ptEnd, int nMoveOffset);
	virtual bool Actor(const CDirectDrawSurface7& surfaceCanvas);

private:
	CBLUEPoint m_ptEnd;//星星终点
	float m_fCurPosX;//x方向当前位置（与GetPosX不同，它记录的是浮点）
	float m_fCurPosY;//y方向当前位置（与GetPosY不同，它记录的是浮点）
	float m_fOffsetX;//x方向每次移动距离
	float m_fOffsetY;//y方向每次移动距离
	int m_nActorNum;//活动次数
};

/**
类名：爆炸动画
功能：用于消泡泡时显示的动画
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCAniBomb : public CDDrawSprite
{
public:
	virtual bool Actor(const CDirectDrawSurface7& surfaceCanvas);
};

/**
类名：便便动画
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCAniShit : public CDDrawSprite
{
public:
	CCBCAniShit(void);

public:
	virtual bool Actor(const CDirectDrawSurface7& surfaceCanvas);

private:
	int m_nWaitTime;
};