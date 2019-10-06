#include "StdAfx.h"
#include "ResManager.h"

CResManager::CResManager(void)
{
}

CResManager::~CResManager(void)
{
}

bool CResManager::Load(CDirectDraw7& dd7)
{
	//加载游戏机主背影图片加载
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\bg.bmp"), 0, &m_surfaceBg)))
		return false;
	if (FAILED(m_surfaceBg.SetSrcColorKey(0x00FF00FF)))
		return false;

	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\bgDisable.bmp"), 0, &m_surfaceBgDisable)))
		return false;
	if (FAILED(m_surfaceBgDisable.SetSrcColorKey(0x00FF00FF)))
		return false;
	
	//泡泡图片加载
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\paopao.bmp"), 0, &m_surfaceBubble)))
		return false;
	if (FAILED(m_surfaceBubble.SetSrcColorKey(0x0000FFFF)))
		return false;

	//泡泡发射器图片加载
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\shoot.bmp"), 0, &m_surfaceShoot)))
		return false;

	//龙图片加载
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\long.bmp"), 0, &m_surfaceLong)))
		return false;
	if (FAILED(m_surfaceLong.SetSrcColorKey(0x00FF00FF)))
		return false;

	//道具相关加载
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\toolBg.bmp"), 0, &m_surfaceToolBg)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\ToolBigBubble.bmp"), 0, &m_surfaceToolBigBubble)))
		return false;
	if (FAILED(m_surfaceToolBigBubble.SetSrcColorKey(0)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\ToolSmallBubble.bmp"), 0, &m_surfaceToolSmallBubble)))
		return false;
	if (FAILED(m_surfaceToolSmallBubble.SetSrcColorKey(0)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\Tool.bmp"), 0, &m_surfaceTool)))
		return false;
	if (FAILED(m_surfaceTool.SetSrcColorKey(0x00FF00FF)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\shit.bmp"), 0, &m_surfaceShit)))
		return false;
	if (FAILED(m_surfaceShit.SetSrcColorKey(0x00FF00FF)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\star.bmp"), 0, &m_surfaceStar)))
		return false;
	if (FAILED(m_surfaceStar.SetSrcColorKey(0)))
		return false;

	//模式对话框相关
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\modelDlgBk.bmp"), 0, &m_surfaceModelDlgBk)))
		return false;
	if (FAILED(m_surfaceModelDlgBk.SetSrcColorKey(0x00FF00FF)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\help.bmp"), 0, &m_surfaceHelp)))
		return false;
	if (FAILED(m_surfaceHelp.SetSrcColorKey(0x00FF00FF)))
		return false;
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\about.bmp"), 0, &m_surfaceAbout)))
		return false;
	if (FAILED(m_surfaceAbout.SetSrcColorKey(0x00FF00FF)))
		return false;

	//用户编号
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\userNum.bmp"), 0, &m_surfaceUserNum)))
		return false;
	if (FAILED(m_surfaceUserNum.SetSrcColorKey(0x00FF00FF)))
		return false;

	//动画
	if (FAILED(dd7.CreateImageSurfaceFromFile(_BLUET("res\\bomb.bmp"), 0, &m_surfaceBomb)))
		return false;
	if (FAILED(m_surfaceBomb.SetSrcColorKey(0x00FF00FF)))
		return false;

	m_dwBubbleRadius = 16;
	return true;
}