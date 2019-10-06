#include "StdAfx.h"
#include "CBCAnimationProcessor.h"

CCBCAnimationProcessor::CCBCAnimationProcessor(const CResManager& resManager) :
m_resManager(resManager)
{
	m_spriteActorManager.SetListener(this);
}

CCBCAnimationProcessor::~CCBCAnimationProcessor(void)
{
}

void CCBCAnimationProcessor::OnDDSMRemoveSprite(CDDrawSprite* pSprite)
{
	BLUEASSERT(pSprite);
	delete pSprite;
}

void CCBCAnimationProcessor::Clear(void)
{
	m_spriteActorManager.Clear();
}

bool CCBCAnimationProcessor::AddShit(void)
{
	CCBCAniShit* pShit = new CCBCAniShit();
	if (pShit->Create(m_resManager.GetSfSrcShit()))
	{
		m_spriteActorManager.Add(*pShit);
		return true;
	}

	delete pShit;
	return false;
}

bool CCBCAnimationProcessor::AddBomb(int nPosX, int nPosY)
{
	CCBCAniBomb* pBomb = new CCBCAniBomb();
	if (pBomb->Create(m_resManager.GetSfSrcBomb(), 36, 36))
	{
		pBomb->SetPos(nPosX, nPosY);
		m_spriteActorManager.Add(*pBomb);
		return true;
	}

	delete pBomb;
	return false;
}

bool CCBCAnimationProcessor::AddStar(const CBLUEPoint& ptStart, const CBLUEPoint& ptEnd)
{
	CCBCAniStar* pStar = new CCBCAniStar();
	if (pStar->Create(m_resManager.GetSfSrcStar(), 50, 50))
	{
		pStar->SetPos(ptStart.m_nX, ptStart.m_nY);
		pStar->ResetEnd(ptEnd, 15);
		m_spriteActorManager.Add(*pStar);
		return true;
	}

	delete pStar;
	return false;
}