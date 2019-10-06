#include "stdafx.h"
#include "CrazyBubbleGameCore.h"
#include <math.h>

#define BOTTOM_SHOW_BUBBLE_OFFSET     60         //��ױ߶��پ����������

CBubblePool::CBubblePool(BLUEDWORD dwPreAllocateBubbleNumber /* = 256 */)
{
	BLUEASSERT(dwPreAllocateBubbleNumber > 0);
	m_BubbleColl.reserve(dwPreAllocateBubbleNumber);
}

CBubblePool::~CBubblePool(void)
{
	//ɾ�����ݼ���
	for (BUBBLECOLL::size_type i = 0; i < m_BubbleColl.size(); i++)
		if (m_BubbleColl[i])
			delete m_BubbleColl[i];

	m_BubbleColl.clear();
}

CBubble* CBubblePool::CreateBubble(void)
{
	//����������һ��û����ʹ�õ�����
	for (BUBBLECOLL::size_type i = 0; i < m_BubbleColl.size(); i++)
	{
		//�����λ��û���������ݣ�������󷵻�
		if (m_BubbleColl[i] == BLUENULL)
		{
			m_BubbleColl[i] = new _Bubble;
			//����ǰҪ����Ϊ����ʹ�ã����������ٴα����뷵��
			m_BubbleColl[i]->bIsUsing = true;
			return m_BubbleColl[i];
		}

		//������ݴ����Ҹ����ݲ���ʹ�����򷵻ظ�����
		if (!m_BubbleColl[i]->bIsUsing)
		{
			//����ǰҪ����Ϊ����ʹ�ã����������ٴα����뷵��
			m_BubbleColl[i]->bIsUsing = true;
			return m_BubbleColl[i];
		}
	}

	//�������ʹ���У���������β�����һ���µ�
	_Bubble* pBubble = new _Bubble;
	pBubble->bIsUsing = true;
	m_BubbleColl.push_back(pBubble);
	return static_cast<CBubble*>(pBubble);
}

void CBubblePool::DestroyBubble(CBubble* pBubble)
{
	BLUEASSERT(pBubble);
	BLUEASSERT((static_cast<_Bubble*>(pBubble))->bIsUsing == true);

	(static_cast<_Bubble*>(pBubble))->bIsUsing = false;
}

CCrazyBubbleGameCore::CCrazyBubbleGameCore(
	const int nSeed,
	const BLUEDWORD dwBubbleRadius,
	const BLUEDWORD dwRows,
	const BLUEDWORD dwCols,
	const BLUEDWORD dwBubbleMoveSpeed,
	const BLUEDWORD dwBadBubbleMoveSpeed,
	ICrazyBubbleGameCoreListener* pListener,
	const BLUEDWORD dwInitRows
	) :
m_pRandom(BLUENULL),
m_dwBubbleRadius(dwBubbleRadius),
m_dwBubbleDiameter(m_dwBubbleRadius * 2),
m_dwRowHeight((BLUEDWORD)(::sqrt(3.0f) * (float)m_dwBubbleRadius)),
m_dwBubbleCollidedConst(4 * m_dwBubbleRadius * m_dwBubbleRadius),
m_dwRows(dwRows),
m_dwCols(dwCols),
m_dwBubbleMetrixSize(m_dwRows * m_dwCols),
m_dwWidth(m_dwCols * m_dwBubbleDiameter),
m_dwHeight(m_dwRows * m_dwRowHeight),
m_dwBubbleMaxMoveSpeed(dwBubbleRadius),//����ƶ����룬������ڸþ������n���ƶ�
m_dwBubbleMoveSpeed(dwBubbleMoveSpeed),//Ĭ��ÿ���ƶ�����Ϊ�뾶��С
m_dwBadBubbleMoveSpeed(dwBadBubbleMoveSpeed),//Ĭ����m_dwBubbleMoveSpeed + 10
m_nCurrentShootAngle(90),
m_dwCurrentBadBubbleNumber(0),
m_dwCurrentBadBubbleShootColIndex(0),
m_pListener(pListener)
{
	BLUEASSERT(m_dwBubbleRadius > 0);
	BLUEASSERT(m_dwRows > 0);
	BLUEASSERT(m_dwCols > 0);
	BLUEASSERT(m_pListener);

	if (m_dwBubbleMoveSpeed <= 0)
		m_dwBubbleMoveSpeed = m_dwBubbleRadius;

	if (m_dwBadBubbleMoveSpeed <= 0)
		m_dwBadBubbleMoveSpeed = m_dwBubbleMoveSpeed + 10;

	m_BubbleMetrix = new CBubble*[m_dwRows * m_dwCols];
	m_sameColorBobbleColl.reserve(10);//֪ͨ����һ����ͬɫ�ʵ����ݲ��ᳬ��10��

	Reset(nSeed, dwInitRows);
}

CCrazyBubbleGameCore::~CCrazyBubbleGameCore(void)
{
	BLUEASSERT(m_BubbleMetrix);
	delete m_pRandom;
	delete m_BubbleMetrix;
}

void CCrazyBubbleGameCore::Reset(const int nSeed, const BLUEDWORD dwInitRows)
{
	BLUEASSERT(m_pListener);

	//�����������
	if (m_pRandom) delete m_pRandom;
	m_pRandom = new CBLUERandom(nSeed);

	//�������
	::memset(m_BubbleMetrix, BLUENULL, sizeof(CBubble*) * (m_dwRows * m_dwCols));

	//��������
	m_activeBubbleColl.clear();

	//�̶��������
	m_fixedBubbleColl.clear();

	//��ǰ������������0
	m_dwCurrentBadBubbleNumber = 0;

	//��ǰ�����ݳ�����Ϊ0
	m_dwCurrentBadBubbleShootColIndex = 0;

	//������������
	m_pCurrentShootingBubble = BLUENULL;
	m_pCurrentShootBubble = m_bubblePool.CreateBubble();
	m_pCurrentShootBubble->m_color = RandomBubbleColor();
	m_pListener->CBGCBubbleCreate(m_pCurrentShootBubble);
	m_pNextShootBubble = m_bubblePool.CreateBubble();
	m_pNextShootBubble->m_color = RandomBubbleColor();
	m_pListener->CBGCBubbleCreate(m_pNextShootBubble);

	//��ʼdwInitRows��
	BLUEASSERT(dwInitRows <= m_dwRows);
	for (BLUEDWORD dwRowIndex = 0; dwRowIndex < dwInitRows; dwRowIndex++)
	{
		for (BLUEDWORD dwColIndex = 0; dwColIndex < m_dwCols; dwColIndex++)
		{
			//ż�е����һ�����ݲ����ܴ���
			if ((dwRowIndex & 1) == 1 && dwColIndex == m_dwCols - 1)
				continue;

			CBubble* pBubble = m_bubblePool.CreateBubble();
			pBubble->m_color = RandomBubbleColor();
			pBubble->m_bIsBadBubble = 0;
			pBubble->m_nMark = 0;
			m_pListener->CBGCBubbleCreate(pBubble);
			FitBubbleInMetrix(pBubble, dwRowIndex, dwColIndex);
		}
	}

	//��Ϸʧ�ܱ������
	m_bIsGameOver = false;
	m_bNotifyGameOver = false;
}

void CCrazyBubbleGameCore::CoerceRefresh(void)
{
	BLUEASSERT(m_pListener);
	m_pListener->CBGCStartDraw();

	//�������й̶�����
	for (FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin(), itEnd = m_fixedBubbleColl.end(); it != itEnd; it++)
		m_pListener->CBGCDrawBubble(*it);

	//�������л����
	for (ACTIVEBUBBLECOLL::iterator it = m_activeBubbleColl.begin(), itEnd = m_activeBubbleColl.end(); it != itEnd; it++)
		m_pListener->CBGCDrawBubble(*it);

	BLUEASSERT(m_pCurrentShootBubble);
	BLUEASSERT(m_pNextShootBubble);
	m_pListener->CBGCDrawCurrentShootBubble(m_pCurrentShootBubble);
	m_pListener->CBGCDrawNextShootBubble(m_pNextShootBubble);

	m_pListener->CBGCEndDraw();
}

bool CCrazyBubbleGameCore::ShootBubble(void)
{
	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return false;

	//�����ǰ����������������л�����Ҳ�㣩�Ļ��������ٷ��䣬Ҫ���������������
	//�����ɲ����ٴ����
	if (m_pCurrentShootingBubble == BLUENULL && m_dwCurrentBadBubbleNumber == 0)
	{
		m_pCurrentShootBubble->m_nPosX = (m_dwWidth >> 1) - m_dwBubbleRadius;
		m_pCurrentShootBubble->m_nPosY = m_dwHeight + BOTTOM_SHOW_BUBBLE_OFFSET;
		m_pCurrentShootBubble->m_nAngle = m_nCurrentShootAngle;
		m_pCurrentShootBubble->m_fOffsetY = 0.0f;
		m_pCurrentShootBubble->m_fOffsetX = 0.0f;
		m_pCurrentShootBubble->m_nMark = 0;
		m_pCurrentShootBubble->m_dwMoveSpeed = m_dwBubbleMoveSpeed;
		m_pCurrentShootBubble->m_bIsBadBubble = 0;
		m_activeBubbleColl.push_back(m_pCurrentShootBubble);
		m_pCurrentShootingBubble = m_pCurrentShootBubble;

		//�������������¼�
		BLUEASSERT(m_pListener);
		m_pListener->CBGCBubbleShoot(m_pCurrentShootingBubble);

		m_pCurrentShootBubble = m_pNextShootBubble;
		m_pNextShootBubble = m_bubblePool.CreateBubble();
		m_pNextShootBubble->m_color = RandomBubbleColor();
		m_pListener->CBGCBubbleCreate(m_pNextShootBubble);

		return true;
	}

	return false;
}

bool CCrazyBubbleGameCore::ShootBadBubble(const BLUEDWORD dwNumber)
{
	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return false;

	//�����ǰ����������������л�����Ҳ�㣩�Ļ��������ٷ��䣬Ҫ���������������
	if (m_pCurrentShootingBubble == BLUENULL && m_dwCurrentBadBubbleNumber == 0)
	{
		BLUEASSERT(m_pRandom);
		BLUEASSERT(m_pListener);
		for (BLUEDWORD i = 0; i < dwNumber; i++)
		{
			CBubble* pBadBubble = m_bubblePool.CreateBubble();
			if (m_dwCurrentBadBubbleShootColIndex >= m_dwCols)
				m_dwCurrentBadBubbleShootColIndex = 0;
			pBadBubble->m_nPosX = m_dwCurrentBadBubbleShootColIndex * m_dwBubbleDiameter;
			pBadBubble->m_nPosY = m_dwHeight + i * m_dwBubbleDiameter;
			pBadBubble->m_nAngle = 90;
			pBadBubble->m_fOffsetY = 0.0f;
			pBadBubble->m_fOffsetX = 0.0f;
			pBadBubble->m_nMark = 0;
			pBadBubble->m_dwMoveSpeed = m_dwBadBubbleMoveSpeed;
			pBadBubble->m_bIsBadBubble = 1;
			pBadBubble->m_color = RandomBubbleColor();
			m_dwCurrentBadBubbleNumber++;
			m_dwCurrentBadBubbleShootColIndex++;
			m_pListener->CBGCBubbleCreate(pBadBubble);
			m_activeBubbleColl.push_back(pBadBubble);
		}
		return true;
	}
	return false;
}

bool CCrazyBubbleGameCore::EraseBubble(const BLUEDWORD dwNumber)
{
	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return false;

	//�����ǰ����������������л�����Ҳ�㣩�Ļ�����������
	if (m_pCurrentShootingBubble == BLUENULL && m_dwCurrentBadBubbleNumber == 0)
	{
		//������Ҫ����ײ����ұ���������������
		BLUEDWORD dwRowIndex = m_dwRows - 1;
		BLUEDWORD dwColIndex = m_dwCols - 1;
		BLUEDWORD dwHasEraseNumber = 0;
		while (dwHasEraseNumber < dwNumber)
		{
			//�õ����������Ƿ�������
			CBubble*& pRefBubble = GetBubbleInMetrix(dwRowIndex, dwColIndex);

			//���������������
			if (pRefBubble)
			{
				m_fixedBubbleColl.remove(pRefBubble);
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleErase(pRefBubble);
				m_bubblePool.DestroyBubble(pRefBubble);
				pRefBubble = BLUENULL;//������BLUENULL
				dwHasEraseNumber++;//����������1
			}

			//�������ͷ��������һ�У����ٴ��ҿ�ʼ�������
			if (dwColIndex == 0)
			{
				//�������Ҳ��ͷ�����
				if (dwRowIndex == 0) break;
				dwColIndex = m_dwCols - 1;
				dwRowIndex--;
			}
			else//����û�е�ͷ���������
			{
				dwColIndex--;
			}
		}

		//���������ݻ����
		InvaildBubbleProcess();

		//����������Ҫǿ��ˢ��һ�λ���
		CoerceRefresh();

		//���سɹ�
		return true;
	}

	return false;
}

bool CCrazyBubbleGameCore::AddLineBadBubble(void)
{
	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return false;

	//�����ǰ����������������л�����Ҳ�㣩�Ļ�����������
	if (m_pCurrentShootingBubble == BLUENULL && m_dwCurrentBadBubbleNumber == 0)
	{
		//�����һ�п�ʼ���ϱ����������ݣ�������������������
		for (BLUEDWORD dwRowIndex = m_dwRows - 1; ((int&)dwRowIndex) >= 0; dwRowIndex--)
		{
			for (BLUEDWORD dwColIndex = 0; dwColIndex < m_dwCols; dwColIndex++)
			{
				CBubble*& pRefBubble = GetBubbleInMetrix(dwRowIndex, dwColIndex);
				//����ø�������ݣ�������������
				if (pRefBubble)
				{
					BLUEDWORD dwNewRowIndex = dwRowIndex + 2;//�µ�������
					//����������Ѿ��������������
					if (dwNewRowIndex >= m_dwRows)
					{
						m_fixedBubbleColl.remove(pRefBubble);
						GetPosFromMetrix(dwNewRowIndex, dwColIndex, pRefBubble->m_nPosX, pRefBubble->m_nPosY);
						m_activeBubbleColl.push_back(pRefBubble);
						pRefBubble = BLUENULL;
						NotifyGameOver();
					}
					else
					{
						m_fixedBubbleColl.remove(pRefBubble);
						FitBubbleInMetrix(pRefBubble, dwNewRowIndex, dwColIndex);
						pRefBubble = BLUENULL;
					}
				}
			}
		}

		//����ͷ���������
		for (BLUEDWORD dwRowIndex = 0; dwRowIndex < 2; dwRowIndex++)
		{
			for (BLUEDWORD dwColIndex = 0; dwColIndex < m_dwCols; dwColIndex++)
			{
				if ((dwRowIndex & 1) == 1 && dwColIndex == m_dwCols - 1)
					continue;

				CBubble* pBubble = m_bubblePool.CreateBubble();
				FitBubbleInMetrix(pBubble, dwRowIndex, dwColIndex);
				pBubble->m_nMark = 0;
				pBubble->m_dwMoveSpeed = m_dwBubbleMoveSpeed;
				pBubble->m_bIsBadBubble = 0;
				pBubble->m_color = RandomBubbleColor();
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleCreate(pBubble);
			}
		}


		//����������Ҫǿ��ˢ��һ�λ���
		CoerceRefresh();

		//���سɹ�
		return true;
	}


	return false;
}

void CCrazyBubbleGameCore::RunOnce(void)
{
	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return;

	//�Ƿ�֪ͨ��Ϸ�����ˣ����֪ͨ�ˣ�����Ϸ���������������¼�
	if (m_bNotifyGameOver)
	{
		m_bIsGameOver = true;
		BLUEASSERT(m_pListener);
		m_pListener->CBGCGameOver();
		return;
	}

	//���û�л������ֱ����������ˢ�£���ԼCPU��Դ��
	if (m_activeBubbleColl.empty())
		return;

	//���л�е����ݻһ��
	ACTIVEBUBBLECOLL::iterator it = m_activeBubbleColl.begin();
	ACTIVEBUBBLECOLL::iterator itEnd = m_activeBubbleColl.end();

	while (it != itEnd)
	{
		if (BubbleActive(*it))
		{
			//����ǻ�����ɾ����������������һ
			if ((*it)->m_bIsBadBubble)
			{
				m_dwCurrentBadBubbleNumber--;
				(*it)->m_bIsBadBubble = 0;
			}

			//�Ѿ����ٻ���ӻ����ɾ��������
			//����ֱ����erase����Ȼit++������Ч
			ACTIVEBUBBLECOLL::iterator delIt = it;
			it++;
			m_activeBubbleColl.erase(delIt);
		}
		else
		{
			it++;
		}
	}
	//֪ͨ������ˢ�½���
	CoerceRefresh();
}

bool CCrazyBubbleGameCore::BubbleActive(CBubble* pBubble)
{
	BLUEASSERT(pBubble);

	//������������µ�����ֻ���ܴ�ֱ��
	if (pBubble->m_nAngle < 0)
	{
		pBubble->m_nPosY += 20;
		//�����±߽�ʱ��������
		if (pBubble->m_nPosY > (int)m_dwHeight + 300)
		{
			m_bubblePool.DestroyBubble(pBubble);
			return true;
		}
		return false;
	}

	BLUEDWORD dwNeedMove = pBubble->m_dwMoveSpeed;//������Ҫ�ƶ����ܾ���
	while (dwNeedMove > 0)//û���ƶ���ͼ����ƶ�
	{
		BLUEDWORD dwCurrentMove;
		//���Ҫ�ƶ��ľ���������ƶ�������ֻ�ƶ�����ƶ����루�ضϣ�
		(dwNeedMove > m_dwBubbleMaxMoveSpeed) ?
			dwCurrentMove = m_dwBubbleMaxMoveSpeed :
			dwCurrentMove = dwNeedMove;//����һ���ƶ���
		dwNeedMove -= dwCurrentMove;

		//�����Լ��ĽǶ������ƶ�����һ������
		pBubble->m_fOffsetY += ::GetSinValue(pBubble->m_nAngle) * dwCurrentMove;
		pBubble->m_nPosY -= (int)pBubble->m_fOffsetY;
		pBubble->m_fOffsetY -= (float)(int)pBubble->m_fOffsetY;

		pBubble->m_fOffsetX += ::GetCosValue(pBubble->m_nAngle) * dwCurrentMove;
		pBubble->m_nPosX += (int)pBubble->m_fOffsetX;
		pBubble->m_fOffsetX -= (float)(int)pBubble->m_fOffsetX;

		//������ݳ����ұ߽磬���ֹ���ұ߽�
		if (pBubble->m_nPosX > (int)(m_dwWidth - m_dwBubbleDiameter))
		{
			pBubble->m_nPosX = (int)(m_dwWidth - m_dwBubbleDiameter);
			pBubble->m_nAngle = 180 - pBubble->m_nAngle;//�����ұ߽��Ҫ����
			pBubble->m_fOffsetX = 0.0f;
			//��������ײǽ�¼�
			BLUEASSERT(m_pListener);
			m_pListener->CBGCBubbleKickWall(m_pCurrentShootBubble);
		}//������ݳ�����߽磬���ֹ����߽�
		else if (pBubble->m_nPosX < 0)
		{
			pBubble->m_nPosX = 0;
			pBubble->m_nAngle = 180 - pBubble->m_nAngle;//������߽��Ҫ����
			pBubble->m_fOffsetX = 0.0f;
			//��������ײǽ�¼�
			BLUEASSERT(m_pListener);
			m_pListener->CBGCBubbleKickWall(m_pCurrentShootBubble);
		}

		//������ݳ������ϱ߽磬���ֹ���ϱ߽�
		if (pBubble->m_nPosY < 0)
			pBubble->m_nPosY = 0;

		//���ж������Ƿ�������������ײ
		//ֻ�����������ĸ������ڵ�������ײ�������������ݶ�ȥ�ж�
		CBubble* pLeftTopBubble;BLUEDWORD dwLTBRowIndex, dwLTBColIndex;bool bLTBIsExist;//����ǰ�������Ͻ����ڸ������
		CBubble* pLeftBottomBubble;BLUEDWORD dwLBBRowIndex, dwLBBColIndex;bool bLBBIsExist;//����ǰ�������½����ڸ������
		CBubble* pRightTopBubble;BLUEDWORD dwRTBRowIndex, dwRTBColIndex;bool bRTBIsExist;//����ǰ�������Ͻ����ڸ������
		CBubble* pRightBottomBubble;BLUEDWORD dwRBBRowIndex, dwRBBColIndex;bool bRBBIsExist;//����ǰ�������½����ڸ������
		CBubble* pCenterBubble;BLUEDWORD dwCBRowIndex, dwCBColIndex;bool bCBIsExist;//����ǰ�����������ڸ������
		bLTBIsExist = GetMetrixFromPos(pBubble->m_nPosX, pBubble->m_nPosY, dwLTBRowIndex, dwLTBColIndex, pLeftTopBubble);
		bLBBIsExist = GetMetrixFromPos(pBubble->m_nPosX, pBubble->m_nPosY + m_dwBubbleDiameter, dwLBBRowIndex, dwLBBColIndex, pLeftBottomBubble);
		bRTBIsExist = GetMetrixFromPos(pBubble->m_nPosX + m_dwBubbleDiameter, pBubble->m_nPosY, dwRTBRowIndex, dwRTBColIndex, pRightTopBubble);
		bRBBIsExist = GetMetrixFromPos(pBubble->m_nPosX + m_dwBubbleDiameter, pBubble->m_nPosY + m_dwBubbleDiameter, dwRBBRowIndex, dwRBBColIndex, pRightBottomBubble);
		bCBIsExist = GetMetrixFromPos(pBubble->m_nPosX + m_dwBubbleRadius, pBubble->m_nPosY + m_dwBubbleRadius, dwCBRowIndex, dwCBColIndex, pCenterBubble);

		if (IsBubbleCollided(pLeftTopBubble, pBubble) ||
			IsBubbleCollided(pLeftBottomBubble, pBubble) ||
			IsBubbleCollided(pRightTopBubble, pBubble) ||
			IsBubbleCollided(pRightBottomBubble, pBubble))
		{
			//���ײ��������ʱ�����Ѿ������˵��ߣ���ֱ����Ϊ��Ϸ����
			if (pBubble->m_nPosY + m_dwBubbleDiameter > m_dwHeight + 10)//�أ���������
			{
				//��Ϸ����
				NotifyGameOver();
				return false;
			}

			//�����ǰ������λ��û�����ݣ���ֱ�ӷŵ�����
			if (bCBIsExist && pCenterBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwCBRowIndex, dwCBColIndex);
				//�������ݹ̶��¼�
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//�������������е����ݣ���Ҫ�Ѹñ�����BLUENULL����Ȼ�޷��ٷ���
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//������ǵ������ݣ�������ͬɫ
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwCBRowIndex, dwCBColIndex, pBubble);
				return true;
			}

			//�����ǰ��λ���Ѿ��������ݣ������ĸ�������һ��û�е����ݵĸ���
			if (bLBBIsExist && pLeftBottomBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwLBBRowIndex, dwLBBColIndex);
				//�������ݹ̶��¼�
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//�������������е����ݣ���Ҫ�Ѹñ�����BLUENULL����Ȼ�޷��ٷ���
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//������ǵ������ݣ�������ͬɫ
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwLBBRowIndex, dwLBBColIndex, pBubble);
				return true;
			}
			if (bRBBIsExist && pRightBottomBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwRBBRowIndex, dwRBBColIndex);
				//�������ݹ̶��¼�
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//�������������е����ݣ���Ҫ�Ѹñ�����BLUENULL����Ȼ�޷��ٷ���
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//������ǵ������ݣ�������ͬɫ
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwRBBRowIndex, dwRBBColIndex, pBubble);
				return true;
			}
			if (bLTBIsExist && pLeftTopBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwLTBRowIndex, dwLTBColIndex);
				//�������ݹ̶��¼�
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//�������������е����ݣ���Ҫ�Ѹñ�����BLUENULL����Ȼ�޷��ٷ���
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//������ǵ������ݣ�������ͬɫ
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwLTBRowIndex, dwLTBColIndex, pBubble);
				return true;
			}
			if (bRTBIsExist && pRightTopBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwRTBRowIndex, dwRTBColIndex);
				//�������ݹ̶��¼�
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//�������������е����ݣ���Ҫ�Ѹñ�����BLUENULL����Ȼ�޷��ٷ���
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//������ǵ������ݣ�������ͬɫ
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwRTBRowIndex, dwRTBColIndex, pBubble);
				return true;
			}

			//�������
			BLUETRACE(_BLUET("������ײ�������\n"));
			return true;
		}

		//����Ѿ������˶���û����ײ����ֱ���ö�
		if (pBubble->m_nPosY == 0)
		{
			BLUEASSERT(bCBIsExist && pCenterBubble == BLUENULL);
			FitBubbleInMetrix(pBubble, dwCBRowIndex, dwCBColIndex);
			//�������ݹ̶��¼�
			BLUEASSERT(m_pListener);
			m_pListener->CBGCBubbleFix(pBubble);
			//�������������е����ݣ���Ҫ�Ѹñ�����BLUENULL����Ȼ�޷��ٷ���
			if (m_pCurrentShootingBubble == pBubble)
				m_pCurrentShootingBubble = BLUENULL;
			if (!pBubble->m_bIsBadBubble)
				SameColorBubbleProcess(dwCBRowIndex, dwCBColIndex, pBubble);
			return true;
		}
	}

	return false;
}

void CCrazyBubbleGameCore::WheelEmitter(const int nAngle)
{
	BLUEASSERT(m_pListener);

	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return;

	if (nAngle < 3)
		m_nCurrentShootAngle = 3;
	else if (nAngle > 177)
		m_nCurrentShootAngle = 177;
	else
		m_nCurrentShootAngle = nAngle;

	m_pListener->CBGCWheelEmitter(m_nCurrentShootAngle);
	CoerceRefresh();
}

void CCrazyBubbleGameCore::WheelEmitterDeasil(void)
{
	BLUEASSERT(m_pListener);

	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return;

	m_nCurrentShootAngle -= 3;
	if (m_nCurrentShootAngle < 3)
		m_nCurrentShootAngle = 3;

	m_pListener->CBGCWheelEmitter(m_nCurrentShootAngle);
	CoerceRefresh();
}

void CCrazyBubbleGameCore::WheelEmitterWiddershins(void)
{
	BLUEASSERT(m_pListener);

	//��Ϸ�Ѿ�����
	if (m_bIsGameOver)
		return;

	m_nCurrentShootAngle += 3;
	if (m_nCurrentShootAngle > 177)
		m_nCurrentShootAngle = 177;

	m_pListener->CBGCWheelEmitter(m_nCurrentShootAngle);
	CoerceRefresh();
}

void CCrazyBubbleGameCore::GetPosFromMetrix(
	const BLUEDWORD dwRowIndex,
	const BLUEDWORD dwColIndex,
	int& nPosX,
	int& nPosY
	)
{
#ifdef _DEBUG
	if (dwRowIndex >= m_dwRows) BLUETRACE(_BLUET("CCrazyBubbleGameCore::GetPosFromMetrix ����������\n"));
	if (dwColIndex >= m_dwCols) BLUETRACE(_BLUET("CCrazyBubbleGameCore::GetPosFromMetrix ����������\n"));
#endif

	//�Ƿ�Ϊ������
	((dwRowIndex & 1) != 1) ?
		nPosX = dwColIndex * m_dwBubbleDiameter ://�����е����ݺ�����Ϊ��������
		nPosX = dwColIndex * m_dwBubbleDiameter + m_dwBubbleRadius;//ż���е����ݺ�������Ҫ����һ���뾶�������Ῠ����һ�����������м�

	//��������������ż�޹�
	nPosY = dwRowIndex * m_dwRowHeight;
}

bool CCrazyBubbleGameCore::GetMetrixFromPos(
	const int nPosX,
	const int nPosY,
	BLUEDWORD& dwRowIndex,
	BLUEDWORD& dwColIndex
	)
{
	//��������Ѿ����ھ������ˣ��򷵻�false
	if (nPosX >= (int)m_dwWidth ||
		nPosY >= (int)m_dwHeight ||
		nPosY < 0 ||
		nPosX < 0)
		return false;

	//�ó��к�
	dwRowIndex = nPosY / m_dwRowHeight;

	//�����и�����ż���в�һ��
	//������
	if ((dwRowIndex & 1) != 1)
		dwColIndex = nPosX / m_dwBubbleDiameter;
	else//ż����
	{
		//ż���ڵ�һ���������Ҫ�տ�һ���뾶�ľ��룬��������ڲ����и��Ӵ���
		((BLUEDWORD)nPosX < m_dwBubbleRadius) ?
			dwColIndex = 0 :
			dwColIndex = ((BLUEDWORD)nPosX - m_dwBubbleRadius) / m_dwBubbleDiameter;

		//ż������Ҫ�����������٣����������һ��ʵ���ǲ����ڵ�
		if (dwColIndex == m_dwCols - 1)
			dwColIndex--;
	}
	return true;
}

bool CCrazyBubbleGameCore::IsBubbleCollided(const CBubble* pBubbleFirst, const CBubble* pBubbleSecond)
{
	if (!pBubbleFirst || !pBubbleSecond)
		return 0;

	BLUEDWORD dwCollisionValue = 
		(pBubbleFirst->m_nPosX - pBubbleSecond->m_nPosX) * (pBubbleFirst->m_nPosX - pBubbleSecond->m_nPosX) +
		(pBubbleFirst->m_nPosY - pBubbleSecond->m_nPosY) * (pBubbleFirst->m_nPosY - pBubbleSecond->m_nPosY);

	//˵����������ײ
	return (dwCollisionValue < m_dwBubbleCollidedConst);
}

bool CCrazyBubbleGameCore::SameColorBubbleProcess(
	const BLUEDWORD dwRowIndex,
	const BLUEDWORD dwColIndex,
	CBubble* pBubble
	)
{
	BLUEASSERT(dwRowIndex < m_dwRows);
	BLUEASSERT(dwColIndex < m_dwCols);
	BLUEASSERT(pBubble);
	BLUEASSERT(GetBubbleInMetrix(dwRowIndex, dwColIndex) == pBubble);
	BLUEASSERT(m_sameColorBobbleColl.empty());

	//ͳ�����������Χ��ͬɫ������
	pBubble->m_nMark = 1;//����Ҫ�ȱ��
	m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwRowIndex, dwColIndex));
	MarkSameColorBubble(dwRowIndex, dwColIndex, pBubble->m_color);

	//�����п�����ȥ��������ȥ
	if (m_sameColorBobbleColl.size() > 2)
	{
		//����������
		BLUEASSERT(m_pListener);
		m_pListener->CBGCBubbleEraseStart();

		//����ȥ�����ݵ���
		for (SAMECOLORBUBBLECOLL::size_type i = 0; i < m_sameColorBobbleColl.size(); i++)
		{
			CBubble*& pRefClearBubble = GetBubbleInMetrix(
				BLUELOWORD(m_sameColorBobbleColl[i]),
				BLUEHIWORD(m_sameColorBobbleColl[i])
				);
			m_fixedBubbleColl.remove(pRefClearBubble);
			m_pListener->CBGCBubbleErase(pRefClearBubble);
			m_bubblePool.DestroyBubble(pRefClearBubble);
			pRefClearBubble = BLUENULL;//��ΪpRefClearBubble���ã�����ֱ�Ӳ�����Ӱ���������еĴ洢
		}

		//���ȫ������������Ӱ���´�ʹ��
		for (FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin(), itEnd = m_fixedBubbleColl.end(); it != itEnd; it++)
			(*it)->m_nMark = 0;

		//����������
		m_pListener->CBGCBubbleEraseEnd((BLUEDWORD)m_sameColorBobbleColl.size());

		//��Ӱ���´�ʹ�ã���ͬɫ����ҲҪ���
		m_sameColorBobbleColl.clear();

		//�����յ�����ȫ������
		InvaildBubbleProcess();
		return true;
	}
	else
	{
		//���ȫ������������Ӱ���´�ʹ��
		for (FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin(), itEnd = m_fixedBubbleColl.end(); it != itEnd; it++)
			(*it)->m_nMark = 0;

		//��Ӱ���´�ʹ�ã���ͬɫ����ҲҪ���
		m_sameColorBobbleColl.clear();
		return false;
	}
}

void CCrazyBubbleGameCore::MarkSameColorBubble(
	const BLUEDWORD dwRowIndex,
	const BLUEDWORD dwColIndex,
	const BUBBLECOLOR color
	)
{
	BLUEASSERT(dwRowIndex < m_dwRows);
	BLUEASSERT(dwColIndex < m_dwCols);

	bool bIsOddRow = ((dwRowIndex & 1) != 1);//�Ƿ�Ϊ������
	CBubble* pBubble;

	//�ó��������ң��������ϣ����������������ݵĸ���λ��
	BLUEDWORD dwLeftBubbleRowIndex = dwRowIndex;
	BLUEDWORD dwLeftBubbleColIndex = dwColIndex - 1;

	BLUEDWORD dwRightBubbleRowIndex = dwRowIndex;
	BLUEDWORD dwRightBubbleColIndex = dwColIndex + 1;

	BLUEDWORD dwLeftTopBubbleRowIndex = dwRowIndex - 1;
	BLUEDWORD dwLeftTopBubbleColIndex = (bIsOddRow) ? dwColIndex - 1 : dwColIndex;

	BLUEDWORD dwRightTopBubbleRowIndex = dwRowIndex - 1;
	BLUEDWORD dwRightTopBubbleColIndex = (bIsOddRow) ? dwColIndex : dwColIndex + 1;

	BLUEDWORD dwLeftBottomBubbleRowIndex = dwRowIndex + 1;
	BLUEDWORD dwLeftBottomBubbleColIndex = (bIsOddRow) ? dwColIndex - 1 : dwColIndex;

	BLUEDWORD dwRightBottomBubbleRowIndex = dwRowIndex + 1;
	BLUEDWORD dwRightBottomBubbleColIndex = (bIsOddRow) ? dwColIndex : dwColIndex + 1;

	pBubble = GetBubbleInMetrixSafe(dwLeftBubbleRowIndex, dwLeftBubbleColIndex);
	if (pBubble && pBubble->m_color == color && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwLeftBubbleRowIndex, dwLeftBubbleColIndex));
		MarkSameColorBubble(dwLeftBubbleRowIndex, dwLeftBubbleColIndex, color);
	}

	pBubble = GetBubbleInMetrixSafe(dwRightBubbleRowIndex, dwRightBubbleColIndex);
	if (pBubble && pBubble->m_color == color && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwRightBubbleRowIndex, dwRightBubbleColIndex));
		MarkSameColorBubble(dwRightBubbleRowIndex, dwRightBubbleColIndex, color);
	}

	pBubble = GetBubbleInMetrixSafe(dwLeftTopBubbleRowIndex, dwLeftTopBubbleColIndex);
	if (pBubble && pBubble->m_color == color && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwLeftTopBubbleRowIndex, dwLeftTopBubbleColIndex));
		MarkSameColorBubble(dwLeftTopBubbleRowIndex, dwLeftTopBubbleColIndex, color);
	}

	pBubble = GetBubbleInMetrixSafe(dwRightTopBubbleRowIndex, dwRightTopBubbleColIndex);
	if (pBubble && pBubble->m_color == color && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwRightTopBubbleRowIndex, dwRightTopBubbleColIndex));
		MarkSameColorBubble(dwRightTopBubbleRowIndex, dwRightTopBubbleColIndex, color);
	}

	pBubble = GetBubbleInMetrixSafe(dwLeftBottomBubbleRowIndex, dwLeftBottomBubbleColIndex);
	if (pBubble && pBubble->m_color == color && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwLeftBottomBubbleRowIndex, dwLeftBottomBubbleColIndex));
		MarkSameColorBubble(dwLeftBottomBubbleRowIndex, dwLeftBottomBubbleColIndex, color);
	}

	pBubble = GetBubbleInMetrixSafe(dwRightBottomBubbleRowIndex, dwRightBottomBubbleColIndex);
	if (pBubble && pBubble->m_color == color && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwRightBottomBubbleRowIndex, dwRightBottomBubbleColIndex));
		MarkSameColorBubble(dwRightBottomBubbleRowIndex, dwRightBottomBubbleColIndex, color);
	}
}

void CCrazyBubbleGameCore::InvaildBubbleProcess(void)
{
	CBubble* pBubble;
	BLUEDWORD dwRowIndex, dwColIndex;
	for (dwColIndex = 0; dwColIndex < m_dwCols; dwColIndex++)
	{
		pBubble = GetBubbleInMetrix(0, dwColIndex);
		if (pBubble)
		{
			pBubble->m_nMark = 1;
			MarkNoInvaildBubble(0, dwColIndex);
		}
	}

	//��������Ч�����ݼӻص�����ݼ��ϣ���������Ϊ������Ϊ
	FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin();
	FIXEDBUBBLECOLL::iterator itEnd = m_fixedBubbleColl.end();
	FIXEDBUBBLECOLL::iterator delIt;
	BLUEDWORD dwDropBubbleNumber = 0;//������������

	while (it != itEnd)
	{
		pBubble = (*it);
		//û�б���ǹ���ȫ����Ч����
		if (pBubble->m_nMark == 0)
		{
			//������Ҫ�������ȴ���DropStart�¼�
			if (dwDropBubbleNumber == 0)
				m_pListener->CBGCBubbleDropStart();

			pBubble->m_nAngle = -1;
			GetMetrixFromPos(pBubble->m_nPosX, pBubble->m_nPosY, dwRowIndex, dwColIndex);
			GetBubbleInMetrix(dwRowIndex, dwColIndex) = BLUENULL;
			delIt = it;
			it++;
			m_pListener->CBGCBubbleDrop(pBubble);
			m_fixedBubbleColl.erase(delIt);
			m_activeBubbleColl.push_back(pBubble);
			dwDropBubbleNumber++;
		}
		else
		{
			pBubble->m_nMark = 0;
			it++;
		}
	}

	//�Ƿ�Ҫ����Drop�¼�
	if (dwDropBubbleNumber > 0)
	{
		BLUEASSERT(m_pListener);
		m_pListener->CBGCBubbleDropEnd(dwDropBubbleNumber);
	}
}

void CCrazyBubbleGameCore::MarkNoInvaildBubble(
	const BLUEDWORD dwRowIndex,
	const BLUEDWORD dwColIndex
	)
{
	BLUEASSERT(dwRowIndex < m_dwRows);
	BLUEASSERT(dwColIndex < m_dwCols);

	bool bIsOddRow = ((dwRowIndex & 1) != 1);//�Ƿ�Ϊ������
	CBubble* pBubble;

	//�ó��������ң����������ĸ����ݵĸ���λ�ã��������ϲ��ü���ˣ�
	BLUEDWORD dwLeftBubbleRowIndex = dwRowIndex;
	BLUEDWORD dwLeftBubbleColIndex = dwColIndex - 1;

	BLUEDWORD dwRightBubbleRowIndex = dwRowIndex;
	BLUEDWORD dwRightBubbleColIndex = dwColIndex + 1;

	BLUEDWORD dwLeftBottomBubbleRowIndex = dwRowIndex + 1;
	BLUEDWORD dwLeftBottomBubbleColIndex = (bIsOddRow) ? dwColIndex - 1 : dwColIndex;

	BLUEDWORD dwRightBottomBubbleRowIndex = dwRowIndex + 1;
	BLUEDWORD dwRightBottomBubbleColIndex = (bIsOddRow) ? dwColIndex : dwColIndex + 1;

	pBubble = GetBubbleInMetrixSafe(dwLeftBubbleRowIndex, dwLeftBubbleColIndex);
	if (pBubble && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		MarkNoInvaildBubble(dwLeftBubbleRowIndex, dwLeftBubbleColIndex);
	}

	pBubble = GetBubbleInMetrixSafe(dwRightBubbleRowIndex, dwRightBubbleColIndex);
	if (pBubble && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		MarkNoInvaildBubble(dwRightBubbleRowIndex, dwRightBubbleColIndex);
	}

	pBubble = GetBubbleInMetrixSafe(dwLeftBottomBubbleRowIndex, dwLeftBottomBubbleColIndex);
	if (pBubble && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		MarkNoInvaildBubble(dwLeftBottomBubbleRowIndex, dwLeftBottomBubbleColIndex);
	}

	pBubble = GetBubbleInMetrixSafe(dwRightBottomBubbleRowIndex, dwRightBottomBubbleColIndex);
	if (pBubble && pBubble->m_nMark == 0)
	{
		pBubble->m_nMark = 1;
		MarkNoInvaildBubble(dwRightBottomBubbleRowIndex, dwRightBottomBubbleColIndex);
	}
}