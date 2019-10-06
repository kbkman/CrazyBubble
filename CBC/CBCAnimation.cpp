#include "StdAfx.h"
#include "CBCAnimation.h"
#include <math.h>
#include "..\Common\ToolKit.h"

CCBCAniStar::CCBCAniStar(void) :
m_fCurPosX(0.0f),
m_fCurPosY(0.0f),
m_fOffsetX(0.0f),
m_fOffsetY(0.0f),
m_nActorNum(0)
{
	m_ptEnd.m_nX = 0;
	m_ptEnd.m_nY = 0;
}

CCBCAniStar::~CCBCAniStar(void)
{
}

void CCBCAniStar::ResetEnd(const CBLUEPoint& ptEnd, int nMoveOffset)
{
	m_fCurPosX = (float)GetPosX();
	m_fCurPosY = (float)GetPosY();

	const float fOffsetX = (float)(ptEnd.m_nX - GetPosX());//œ‡æ‡x
	const float fOffsetY = (float)(ptEnd.m_nY - GetPosY());//œ‡æ‡y

	float fLength = ::pow(fOffsetX * fOffsetX + fOffsetY * fOffsetY, 0.5f);
	m_fOffsetX = nMoveOffset * (fOffsetX / fLength);
	m_fOffsetY = nMoveOffset * (fOffsetY / fLength);

	m_nActorNum = (int)(fLength / nMoveOffset);// + 10;
}

bool CCBCAniStar::Actor(const CDirectDrawSurface7& surfaceCanvas)
{
	if (m_nActorNum-- <= 0)
		return true;

	m_fCurPosX += m_fOffsetX;
	m_fCurPosY += m_fOffsetY;
	SetPos((int)m_fCurPosX, (int)m_fCurPosY);
	BLUEASSERT(surfaceCanvas);
	Paint(surfaceCanvas);
	NextFrame();

	return false;
}
//////////////////////////////////////////////////////////////////////////
////////////////////////CCBCAniBomb////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CCBCAniBomb::Actor(const CDirectDrawSurface7& surfaceCanvas)
{
	BLUEASSERT(surfaceCanvas);

	if (GetCurFrameIndex() >= GetTotalFramesNum() - 1)
		return true;

	BLUEASSERT(surfaceCanvas);
	Paint(surfaceCanvas);

	NextFrame();
	return false;
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////CCBCAniShit/////////////////////////////
//////////////////////////////////////////////////////////////////////////
CCBCAniShit::CCBCAniShit(void) :
m_nWaitTime(250)
{
	SetPos(0, 0);
}

bool CCBCAniShit::Actor(const CDirectDrawSurface7& surfaceCanvas)
{
	if (m_nWaitTime-- < 150)
		MoveY(1);

	BLUEASSERT(surfaceCanvas);
	Paint(surfaceCanvas);

	return GetPosY() > 600;
}