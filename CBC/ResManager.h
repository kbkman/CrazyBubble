#pragma once

#include "BLUEDirectX/engine2d.h"

/**
��������Դ������
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CResManager
{
public:
	CResManager(void);
	~CResManager(void);

public:
	/*
	������������������Դ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool Load(CDirectDraw7& dd7);

	/*
	����������ȡԴ����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	const CDirectDrawSurface7& GetSfSrcBg(void) const {return m_surfaceBg;}
	const CDirectDrawSurface7& GetSfSrcBgDisable(void) const {return m_surfaceBgDisable;}
	const CDirectDrawSurface7& GetSfSrcBubble(void) const {return m_surfaceBubble;}
	const CDirectDrawSurface7& GetSfSrcShoot(void) const {return m_surfaceShoot;}
	const CDirectDrawSurface7& GetSfSrcToolBg(void) const {return m_surfaceToolBg;}
	const CDirectDrawSurface7& GetSfSrcToolBigBubble(void) const {return m_surfaceToolBigBubble;}
	const CDirectDrawSurface7& GetSfSrcToolSmallBubble(void) const {return m_surfaceToolSmallBubble;}
	const CDirectDrawSurface7& GetSfSrcTool(void) const {return m_surfaceTool;}
	const CDirectDrawSurface7& GetSfSrcShit(void) const {return m_surfaceShit;}
	const CDirectDrawSurface7& GetSfSrcLong(void) const {return m_surfaceLong;}
	const CDirectDrawSurface7& GetSfSrcUserNum(void) const {return m_surfaceUserNum;}
	const CDirectDrawSurface7& GetSfSrcBomb(void) const {return m_surfaceBomb;}
	const CDirectDrawSurface7& GetSfSrcStar(void) const {return m_surfaceStar;}
	const CDirectDrawSurface7& GetSfSrcModelDlgBk(void) const {return m_surfaceModelDlgBk;}
	const CDirectDrawSurface7& GetSfSrcHelp(void) const {return m_surfaceHelp;}
	const CDirectDrawSurface7& GetSfSrcAbout(void) const {return m_surfaceAbout;}


	BLUEDWORD GetBubbleRadius(void) const {return m_dwBubbleRadius;}

private:
	/*
	������������Դ����
	���ܣ���
	˵������
	*/
	CDirectDrawSurface7 m_surfaceBg;//���ڻ��Ʊ�����Դ����
	CDirectDrawSurface7 m_surfaceBgDisable;//���ڻ��Ʊ����������ã�Դ����
	CDirectDrawSurface7 m_surfaceShoot;//����������Դ
	CDirectDrawSurface7 m_surfaceBubble;//���ڻ������ݵ�Դ����
	CDirectDrawSurface7 m_surfaceLong;//��Դ����
	CDirectDrawSurface7 m_surfaceToolBg;//���߱�������Դ
	CDirectDrawSurface7 m_surfaceToolBigBubble;//���ߴ����ݱ���Դ
	CDirectDrawSurface7 m_surfaceToolSmallBubble;//����С���ݱ���Դ
	CDirectDrawSurface7 m_surfaceTool;//���߱���Դ
	CDirectDrawSurface7 m_surfaceShit;//������Դ
	CDirectDrawSurface7 m_surfaceUserNum;//�û���ű���Դ
	CDirectDrawSurface7 m_surfaceBomb;//���ݱ�ը����Դ
	CDirectDrawSurface7 m_surfaceStar;//�ӵ���ʱ���Ǳ���Դ
	CDirectDrawSurface7 m_surfaceModelDlgBk;//ģʽ�Ի���Ĭ�ϵı���
	CDirectDrawSurface7 m_surfaceHelp;//��������Դ
	CDirectDrawSurface7 m_surfaceAbout;//���ڱ���Դ

	/*
	�����������ݵİ뾶
	���ܣ���
	˵������
	*/
	BLUEDWORD m_dwBubbleRadius;
};
