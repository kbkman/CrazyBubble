#pragma once

#include "BLUEDirectX/engine2d.h"

/**
类名：资源管理器
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CResManager
{
public:
	CResManager(void);
	~CResManager(void);

public:
	/*
	函数名：加载所有资源
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool Load(CDirectDraw7& dd7);

	/*
	函数名：获取源表面
	功能：略
	说明：无
	参数：无
	返回值：无
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
	变量名：所有源表面
	功能：略
	说明：无
	*/
	CDirectDrawSurface7 m_surfaceBg;//用于绘制背景的源表面
	CDirectDrawSurface7 m_surfaceBgDisable;//用于绘制背景（不可用）源表面
	CDirectDrawSurface7 m_surfaceShoot;//发射器表面源
	CDirectDrawSurface7 m_surfaceBubble;//用于绘制泡泡的源表面
	CDirectDrawSurface7 m_surfaceLong;//龙源表面
	CDirectDrawSurface7 m_surfaceToolBg;//道具背景表面源
	CDirectDrawSurface7 m_surfaceToolBigBubble;//道具大泡泡表面源
	CDirectDrawSurface7 m_surfaceToolSmallBubble;//道具小泡泡表面源
	CDirectDrawSurface7 m_surfaceTool;//道具表面源
	CDirectDrawSurface7 m_surfaceShit;//便便表面源
	CDirectDrawSurface7 m_surfaceUserNum;//用户编号表面源
	CDirectDrawSurface7 m_surfaceBomb;//泡泡爆炸动画源
	CDirectDrawSurface7 m_surfaceStar;//扔道具时星星表面源
	CDirectDrawSurface7 m_surfaceModelDlgBk;//模式对话框默认的背景
	CDirectDrawSurface7 m_surfaceHelp;//帮助表面源
	CDirectDrawSurface7 m_surfaceAbout;//关于表面源

	/*
	变量名：泡泡的半径
	功能：略
	说明：无
	*/
	BLUEDWORD m_dwBubbleRadius;
};
