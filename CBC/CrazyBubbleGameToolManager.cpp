#include "StdAfx.h"
#include "CrazyBubbleGameToolManager.h"

#define BUBBLE_BIG_INSMALL_NUM            5   //大泡泡=多少个小泡泡

CCrazyBubbleGameToolManager::CCrazyBubbleGameToolManager(
	CDirectDraw7& dd7,
	const CDirectDrawSurface7& surfaceToolBg,
	const CDirectDrawSurface7& surfaceToolBigBubble,
	const CDirectDrawSurface7& surfaceToolSmallBubble,
	const CDirectDrawSurface7& surfaceTool) :
m_dwBigBubbleNumber(0),
m_dwSmallBubbleNumber(0),
m_surfaceToolBg(surfaceToolBg),
m_surfaceToolBigBubble(surfaceToolBigBubble),
m_surfaceToolSmallBubble(surfaceToolSmallBubble)
{
	//创建画布（与背景一样尺寸）
	DWORD dwCanvasWidth, dwCanvasHeight;
	m_surfaceToolBg.GetBaseDesc(dwCanvasWidth, dwCanvasHeight);
	//画布长宽都加1，不然vista和win7上有问题（尚不知道是什么问题）
	dd7.CreateGeneralSurface(++dwCanvasWidth, ++dwCanvasHeight, 0, &m_surfaceCanvas);

	//创建道具精灵
	m_spriteTool.Create(surfaceTool, 32, 32);

	//默认使用泡泡攻击
	UsingTool(false);
}

CCrazyBubbleGameToolManager::~CCrazyBubbleGameToolManager(void)
{
	Clear();
}

void CCrazyBubbleGameToolManager::Clear(void)
{
	m_dwBigBubbleNumber = 0;
	m_dwSmallBubbleNumber = 0;
	while (!m_toolQueue.empty())
		m_toolQueue.pop();

	//默认使用泡泡攻击
	UsingTool(false);
}

void CCrazyBubbleGameToolManager::AddThrowBubble(const BLUEDWORD dwNumber)
{
	BLUEDWORD dwAddBigBubbleNumber;
	BLUEDWORD dwAddSmallBubbleNumber;

	//计算出可以增加几个大泡泡和几个小泡铴
	dwAddBigBubbleNumber = dwNumber / BUBBLE_BIG_INSMALL_NUM;
	dwAddSmallBubbleNumber = dwNumber % BUBBLE_BIG_INSMALL_NUM;

	//分别加上
	m_dwBigBubbleNumber += dwAddBigBubbleNumber;
	m_dwSmallBubbleNumber += dwAddSmallBubbleNumber;

	//如果小泡泡数量可以形成一个大泡泡时，则大泡泡加一个
	//小泡泡减去一个大泡泡的数量
	if (m_dwSmallBubbleNumber >= BUBBLE_BIG_INSMALL_NUM)
	{
		m_dwSmallBubbleNumber -= BUBBLE_BIG_INSMALL_NUM;
		m_dwBigBubbleNumber++;
	}

	DrawCanvas();
}

BLUEDWORD CCrazyBubbleGameToolManager::RemoveThrowBubble(void)
{
	BLUEASSERT(!IsUsingTool());
	//是否有大泡泡，有的话则直接扔一个大泡泡
	if (m_dwBigBubbleNumber > 0)
	{
		m_dwBigBubbleNumber--;
		DrawCanvas();
		return BUBBLE_BIG_INSMALL_NUM;
	}

	//是否有小泡泡，如果有则扔一个小泡泡
	if (m_dwSmallBubbleNumber > 0)
	{
		m_dwSmallBubbleNumber--;
		DrawCanvas();
		return 1;
	}

	//都没有则返回0
	return 0;
}

void CCrazyBubbleGameToolManager::UsingTool(bool b)
{
	if (b)
		m_surfaceCanvas.SetSrcColorKey(0x00000033);
	else
		m_surfaceCanvas.SetSrcColorKey(0);

	m_bIsUsingTool = b;
	DrawCanvas();
}

bool CCrazyBubbleGameToolManager::AdjustToolOrder(void)
{
	if (!m_toolQueue.empty() && IsUsingTool())
	{
		TOOL tool = m_toolQueue.front();
		m_toolQueue.pop();
		m_toolQueue.push(tool);
		DrawCanvas();
		return true;
	}
	return false;
}

void CCrazyBubbleGameToolManager::DrawCanvas(void)
{
	m_surfaceCanvas.FillColor(0);
	//绘制背景
	m_surfaceToolBg.Blt(m_surfaceCanvas);

	//绘制道具泡泡
	RECT rc = {4, 26, 0, 0};
	//绘制大泡泡
	DWORD dwBigBubbleWidth, dwBigBubbleHeight;
	m_surfaceToolBigBubble.GetBaseDesc(dwBigBubbleWidth, dwBigBubbleHeight);
	rc.right = rc.left + dwBigBubbleWidth;
	for (BLUEDWORD i = 0; i < m_dwBigBubbleNumber; i++)
	{
		rc.top += dwBigBubbleHeight;
		rc.bottom = rc.top + dwBigBubbleHeight;
		m_surfaceCanvas->Blt(&rc, m_surfaceToolBigBubble, NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	}
	//绘制小泡泡
	DWORD dwSmallBubbleWidth, dwSmallBubbleHeight;
	m_surfaceToolSmallBubble.GetBaseDesc(dwSmallBubbleWidth, dwSmallBubbleHeight);
	rc.top += dwBigBubbleHeight;
	rc.bottom = rc.top + dwSmallBubbleHeight;
	for (BLUEDWORD i = 0; i < m_dwSmallBubbleNumber; i++)
	{
		rc.left = 4 + i * dwSmallBubbleWidth;
		rc.right = rc.left + dwSmallBubbleWidth;
		m_surfaceCanvas->Blt(&rc, m_surfaceToolSmallBubble, NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	}

	//绘制道具
	int nCurDrawToolTop = 295;
	TOOLQUEUE::container_type::const_iterator it = m_toolQueue._Get_container().begin();
	TOOLQUEUE::container_type::const_iterator itEnd = m_toolQueue._Get_container().end();
	for (;it != itEnd;it++, nCurDrawToolTop += m_spriteTool.GetCellHeight())
	{
		m_spriteTool.SetFrame((int)*it);
		m_spriteTool.Paint(m_surfaceCanvas, 1, nCurDrawToolTop);
	}
}