#pragma once

#include "BLUEDirectX\engine2d.h"
#include "ResManager.h"
#include "CBCAnimation.h"

/**
类名：
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCAnimationProcessor : private IDDSMListener
{
public:
	CCBCAnimationProcessor(const CResManager& resManager);
	~CCBCAnimationProcessor(void);

public:
	//清除所有动画
	void Clear(void);

	/*
	函数名：添加动画
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool AddShit(void);//添加便便动画
	bool AddBomb(int nPosX, int nPosY);//添加泡泡爆炸动画
	bool AddStar(const CBLUEPoint& ptStart, const CBLUEPoint& ptEnd);

	//函数名：是否有动画
	bool HasAni(void) const {return m_spriteActorManager.HasSprite();}

	//函数名：所有动画运行
	void ActorAllAni(const CDirectDrawSurface7 surfaceCanvas) {m_spriteActorManager.Actor(surfaceCanvas);}

private:
	virtual void OnDDSMRemoveSprite(CDDrawSprite* pSprite);

private:
	const CResManager& m_resManager;//资源管理器
	CDDSpriteActorManager m_spriteActorManager;//精灵活动管理器
	CDirectDrawSurface7 m_surfaceStar;
	CDirectDrawSurface7 m_surfaceBomb;
	CDirectDrawSurface7 m_surfaceShit;
};
