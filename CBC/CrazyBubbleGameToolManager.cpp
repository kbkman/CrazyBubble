#include "StdAfx.h"
#include "CrazyBubbleGameToolManager.h"

#define BUBBLE_BIG_INSMALL_NUM            5   //������=���ٸ�С����

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
	//�����������뱳��һ���ߴ磩
	DWORD dwCanvasWidth, dwCanvasHeight;
	m_surfaceToolBg.GetBaseDesc(dwCanvasWidth, dwCanvasHeight);
	//����������1����Ȼvista��win7�������⣨�в�֪����ʲô���⣩
	dd7.CreateGeneralSurface(++dwCanvasWidth, ++dwCanvasHeight, 0, &m_surfaceCanvas);

	//�������߾���
	m_spriteTool.Create(surfaceTool, 32, 32);

	//Ĭ��ʹ�����ݹ���
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

	//Ĭ��ʹ�����ݹ���
	UsingTool(false);
}

void CCrazyBubbleGameToolManager::AddThrowBubble(const BLUEDWORD dwNumber)
{
	BLUEDWORD dwAddBigBubbleNumber;
	BLUEDWORD dwAddSmallBubbleNumber;

	//������������Ӽ��������ݺͼ���С���
	dwAddBigBubbleNumber = dwNumber / BUBBLE_BIG_INSMALL_NUM;
	dwAddSmallBubbleNumber = dwNumber % BUBBLE_BIG_INSMALL_NUM;

	//�ֱ����
	m_dwBigBubbleNumber += dwAddBigBubbleNumber;
	m_dwSmallBubbleNumber += dwAddSmallBubbleNumber;

	//���С�������������γ�һ��������ʱ��������ݼ�һ��
	//С���ݼ�ȥһ�������ݵ�����
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
	//�Ƿ��д����ݣ��еĻ���ֱ����һ��������
	if (m_dwBigBubbleNumber > 0)
	{
		m_dwBigBubbleNumber--;
		DrawCanvas();
		return BUBBLE_BIG_INSMALL_NUM;
	}

	//�Ƿ���С���ݣ����������һ��С����
	if (m_dwSmallBubbleNumber > 0)
	{
		m_dwSmallBubbleNumber--;
		DrawCanvas();
		return 1;
	}

	//��û���򷵻�0
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
	//���Ʊ���
	m_surfaceToolBg.Blt(m_surfaceCanvas);

	//���Ƶ�������
	RECT rc = {4, 26, 0, 0};
	//���ƴ�����
	DWORD dwBigBubbleWidth, dwBigBubbleHeight;
	m_surfaceToolBigBubble.GetBaseDesc(dwBigBubbleWidth, dwBigBubbleHeight);
	rc.right = rc.left + dwBigBubbleWidth;
	for (BLUEDWORD i = 0; i < m_dwBigBubbleNumber; i++)
	{
		rc.top += dwBigBubbleHeight;
		rc.bottom = rc.top + dwBigBubbleHeight;
		m_surfaceCanvas->Blt(&rc, m_surfaceToolBigBubble, NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	}
	//����С����
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

	//���Ƶ���
	int nCurDrawToolTop = 295;
	TOOLQUEUE::container_type::const_iterator it = m_toolQueue._Get_container().begin();
	TOOLQUEUE::container_type::const_iterator itEnd = m_toolQueue._Get_container().end();
	for (;it != itEnd;it++, nCurDrawToolTop += m_spriteTool.GetCellHeight())
	{
		m_spriteTool.SetFrame((int)*it);
		m_spriteTool.Paint(m_surfaceCanvas, 1, nCurDrawToolTop);
	}
}