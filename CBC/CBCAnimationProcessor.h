#pragma once

#include "BLUEDirectX\engine2d.h"
#include "ResManager.h"
#include "CBCAnimation.h"

/**
������
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCBCAnimationProcessor : private IDDSMListener
{
public:
	CCBCAnimationProcessor(const CResManager& resManager);
	~CCBCAnimationProcessor(void);

public:
	//������ж���
	void Clear(void);

	/*
	����������Ӷ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool AddShit(void);//��ӱ�㶯��
	bool AddBomb(int nPosX, int nPosY);//������ݱ�ը����
	bool AddStar(const CBLUEPoint& ptStart, const CBLUEPoint& ptEnd);

	//���������Ƿ��ж���
	bool HasAni(void) const {return m_spriteActorManager.HasSprite();}

	//�����������ж�������
	void ActorAllAni(const CDirectDrawSurface7 surfaceCanvas) {m_spriteActorManager.Actor(surfaceCanvas);}

private:
	virtual void OnDDSMRemoveSprite(CDDrawSprite* pSprite);

private:
	const CResManager& m_resManager;//��Դ������
	CDDSpriteActorManager m_spriteActorManager;//����������
	CDirectDrawSurface7 m_surfaceStar;
	CDirectDrawSurface7 m_surfaceBomb;
	CDirectDrawSurface7 m_surfaceShit;
};
