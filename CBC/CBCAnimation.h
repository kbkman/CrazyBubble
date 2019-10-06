#pragma once
#include "BLUEDirectX\engine2d.h"

/**
���������Ƕ���
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
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
	CBLUEPoint m_ptEnd;//�����յ�
	float m_fCurPosX;//x����ǰλ�ã���GetPosX��ͬ������¼���Ǹ��㣩
	float m_fCurPosY;//y����ǰλ�ã���GetPosY��ͬ������¼���Ǹ��㣩
	float m_fOffsetX;//x����ÿ���ƶ�����
	float m_fOffsetY;//y����ÿ���ƶ�����
	int m_nActorNum;//�����
};

/**
��������ը����
���ܣ�����������ʱ��ʾ�Ķ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCBCAniBomb : public CDDrawSprite
{
public:
	virtual bool Actor(const CDirectDrawSurface7& surfaceCanvas);
};

/**
��������㶯��
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
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