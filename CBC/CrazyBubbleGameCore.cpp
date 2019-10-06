#include "stdafx.h"
#include "CrazyBubbleGameCore.h"
#include <math.h>

#define BOTTOM_SHOW_BUBBLE_OFFSET     60         //离底边多少距离出现泡泡

CBubblePool::CBubblePool(BLUEDWORD dwPreAllocateBubbleNumber /* = 256 */)
{
	BLUEASSERT(dwPreAllocateBubbleNumber > 0);
	m_BubbleColl.reserve(dwPreAllocateBubbleNumber);
}

CBubblePool::~CBubblePool(void)
{
	//删除泡泡集合
	for (BUBBLECOLL::size_type i = 0; i < m_BubbleColl.size(); i++)
		if (m_BubbleColl[i])
			delete m_BubbleColl[i];

	m_BubbleColl.clear();
}

CBubble* CBubblePool::CreateBubble(void)
{
	//从数组中找一个没有在使用的泡泡
	for (BUBBLECOLL::size_type i = 0; i < m_BubbleColl.size(); i++)
	{
		//如果该位置没有申请泡泡，则申请后返回
		if (m_BubbleColl[i] == BLUENULL)
		{
			m_BubbleColl[i] = new _Bubble;
			//返回前要设置为正在使用，这样不会再次被申请返回
			m_BubbleColl[i]->bIsUsing = true;
			return m_BubbleColl[i];
		}

		//如果泡泡存在且该泡泡不在使用中则返回该泡泡
		if (!m_BubbleColl[i]->bIsUsing)
		{
			//返回前要设置为正在使用，这样不会再次被申请返回
			m_BubbleColl[i]->bIsUsing = true;
			return m_BubbleColl[i];
		}
	}

	//如果都在使用中，则在数组尾部添加一个新的
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
m_dwBubbleMaxMoveSpeed(dwBubbleRadius),//最多移动距离，如果大于该距离则分n次移动
m_dwBubbleMoveSpeed(dwBubbleMoveSpeed),//默认每次移动距离为半径大小
m_dwBadBubbleMoveSpeed(dwBadBubbleMoveSpeed),//默认是m_dwBubbleMoveSpeed + 10
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
	m_sameColorBobbleColl.reserve(10);//通知连在一起相同色彩的泡泡不会超出10个

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

	//生成随机数器
	if (m_pRandom) delete m_pRandom;
	m_pRandom = new CBLUERandom(nSeed);

	//矩阵清空
	::memset(m_BubbleMetrix, BLUENULL, sizeof(CBubble*) * (m_dwRows * m_dwCols));

	//活动泡泡清空
	m_activeBubbleColl.clear();

	//固定泡泡清空
	m_fixedBubbleColl.clear();

	//当前坏泡泡数量置0
	m_dwCurrentBadBubbleNumber = 0;

	//当前坏泡泡出现列为0
	m_dwCurrentBadBubbleShootColIndex = 0;

	//发射泡泡重置
	m_pCurrentShootingBubble = BLUENULL;
	m_pCurrentShootBubble = m_bubblePool.CreateBubble();
	m_pCurrentShootBubble->m_color = RandomBubbleColor();
	m_pListener->CBGCBubbleCreate(m_pCurrentShootBubble);
	m_pNextShootBubble = m_bubblePool.CreateBubble();
	m_pNextShootBubble->m_color = RandomBubbleColor();
	m_pListener->CBGCBubbleCreate(m_pNextShootBubble);

	//初始dwInitRows行
	BLUEASSERT(dwInitRows <= m_dwRows);
	for (BLUEDWORD dwRowIndex = 0; dwRowIndex < dwInitRows; dwRowIndex++)
	{
		for (BLUEDWORD dwColIndex = 0; dwColIndex < m_dwCols; dwColIndex++)
		{
			//偶行的最后一个泡泡不可能存在
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

	//游戏失败标记重置
	m_bIsGameOver = false;
	m_bNotifyGameOver = false;
}

void CCrazyBubbleGameCore::CoerceRefresh(void)
{
	BLUEASSERT(m_pListener);
	m_pListener->CBGCStartDraw();

	//绘制所有固定泡泡
	for (FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin(), itEnd = m_fixedBubbleColl.end(); it != itEnd; it++)
		m_pListener->CBGCDrawBubble(*it);

	//绘制所有活动泡泡
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
	//游戏已经结束
	if (m_bIsGameOver)
		return false;

	//如果当前有泡泡正在射击（有坏泡泡也算）的话，则不能再发射，要等正在射击的泡泡
	//射击完成才能再次射击
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

		//触发发射泡泡事件
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
	//游戏已经结束
	if (m_bIsGameOver)
		return false;

	//如果当前有泡泡正在射击（有坏泡泡也算）的话，则不能再发射，要等正在射击的泡泡
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
	//游戏已经结束
	if (m_bIsGameOver)
		return false;

	//如果当前有泡泡正在射击（有坏泡泡也算）的话，则不能消除
	if (m_pCurrentShootingBubble == BLUENULL && m_dwCurrentBadBubbleNumber == 0)
	{
		//消泡泡要从最底层最右边向上向左消泡泡
		BLUEDWORD dwRowIndex = m_dwRows - 1;
		BLUEDWORD dwColIndex = m_dwCols - 1;
		BLUEDWORD dwHasEraseNumber = 0;
		while (dwHasEraseNumber < dwNumber)
		{
			//得到该网格内是否有泡泡
			CBubble*& pRefBubble = GetBubbleInMetrix(dwRowIndex, dwColIndex);

			//如果有泡泡则消除
			if (pRefBubble)
			{
				m_fixedBubbleColl.remove(pRefBubble);
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleErase(pRefBubble);
				m_bubblePool.DestroyBubble(pRefBubble);
				pRefBubble = BLUENULL;//网格置BLUENULL
				dwHasEraseNumber++;//消除数量加1
			}

			//如果向左到头，则向上一行，并再从右开始消除泡铴
			if (dwColIndex == 0)
			{
				//如果向上也到头则继续
				if (dwRowIndex == 0) break;
				dwColIndex = m_dwCols - 1;
				dwRowIndex--;
			}
			else//向左没有到头则继续向左
			{
				dwColIndex--;
			}
		}

		//可能有泡泡会掉落
		InvaildBubbleProcess();

		//消除泡泡需要强制刷新一次画面
		CoerceRefresh();

		//返回成功
		return true;
	}

	return false;
}

bool CCrazyBubbleGameCore::AddLineBadBubble(void)
{
	//游戏已经结束
	if (m_bIsGameOver)
		return false;

	//如果当前有泡泡正在射击（有坏泡泡也算）的话，则不能消除
	if (m_pCurrentShootingBubble == BLUENULL && m_dwCurrentBadBubbleNumber == 0)
	{
		//从最后一行开始向上遍历所有泡泡，所有泡泡向下移两行
		for (BLUEDWORD dwRowIndex = m_dwRows - 1; ((int&)dwRowIndex) >= 0; dwRowIndex--)
		{
			for (BLUEDWORD dwColIndex = 0; dwColIndex < m_dwCols; dwColIndex++)
			{
				CBubble*& pRefBubble = GetBubbleInMetrix(dwRowIndex, dwColIndex);
				//如果该格存在泡泡，则将其下移两行
				if (pRefBubble)
				{
					BLUEDWORD dwNewRowIndex = dwRowIndex + 2;//新的行索引
					//如果行索引已经超出最大行索引
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

		//将开头的两行填充
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


		//消除泡泡需要强制刷新一次画面
		CoerceRefresh();

		//返回成功
		return true;
	}


	return false;
}

void CCrazyBubbleGameCore::RunOnce(void)
{
	//游戏已经结束
	if (m_bIsGameOver)
		return;

	//是否通知游戏结束了，如果通知了，则游戏结束并触发结束事件
	if (m_bNotifyGameOver)
	{
		m_bIsGameOver = true;
		BLUEASSERT(m_pListener);
		m_pListener->CBGCGameOver();
		return;
	}

	//如果没有活动泡泡则直接跳出（不刷新，节约CPU资源）
	if (m_activeBubbleColl.empty())
		return;

	//所有活动中的泡泡活动一次
	ACTIVEBUBBLECOLL::iterator it = m_activeBubbleColl.begin();
	ACTIVEBUBBLECOLL::iterator itEnd = m_activeBubbleColl.end();

	while (it != itEnd)
	{
		if (BubbleActive(*it))
		{
			//如果是坏泡泡删除，则坏泡泡数量减一
			if ((*it)->m_bIsBadBubble)
			{
				m_dwCurrentBadBubbleNumber--;
				(*it)->m_bIsBadBubble = 0;
			}

			//已经不再活动，从活动链中删除该泡泡
			//不能直接先erase，不然it++将会无效
			ACTIVEBUBBLECOLL::iterator delIt = it;
			it++;
			m_activeBubbleColl.erase(delIt);
		}
		else
		{
			it++;
		}
	}
	//通知监听者刷新界面
	CoerceRefresh();
}

bool CCrazyBubbleGameCore::BubbleActive(CBubble* pBubble)
{
	BLUEASSERT(pBubble);

	//泡泡如果是向下掉，则只可能垂直掉
	if (pBubble->m_nAngle < 0)
	{
		pBubble->m_nPosY += 20;
		//掉出下边界时销毁泡泡
		if (pBubble->m_nPosY > (int)m_dwHeight + 300)
		{
			m_bubblePool.DestroyBubble(pBubble);
			return true;
		}
		return false;
	}

	BLUEDWORD dwNeedMove = pBubble->m_dwMoveSpeed;//泡泡需要移动的总距离
	while (dwNeedMove > 0)//没有移动完就继续移动
	{
		BLUEDWORD dwCurrentMove;
		//如果要移动的距离大过最大移动距离则只移动最大移动距离（截断）
		(dwNeedMove > m_dwBubbleMaxMoveSpeed) ?
			dwCurrentMove = m_dwBubbleMaxMoveSpeed :
			dwCurrentMove = dwNeedMove;//否则一次移动完
		dwNeedMove -= dwCurrentMove;

		//根据自己的角度泡泡移动到下一个坐标
		pBubble->m_fOffsetY += ::GetSinValue(pBubble->m_nAngle) * dwCurrentMove;
		pBubble->m_nPosY -= (int)pBubble->m_fOffsetY;
		pBubble->m_fOffsetY -= (float)(int)pBubble->m_fOffsetY;

		pBubble->m_fOffsetX += ::GetCosValue(pBubble->m_nAngle) * dwCurrentMove;
		pBubble->m_nPosX += (int)pBubble->m_fOffsetX;
		pBubble->m_fOffsetX -= (float)(int)pBubble->m_fOffsetX;

		//如果泡泡超出右边界，则截止到右边界
		if (pBubble->m_nPosX > (int)(m_dwWidth - m_dwBubbleDiameter))
		{
			pBubble->m_nPosX = (int)(m_dwWidth - m_dwBubbleDiameter);
			pBubble->m_nAngle = 180 - pBubble->m_nAngle;//碰到右边界后要反弹
			pBubble->m_fOffsetX = 0.0f;
			//触发泡泡撞墙事件
			BLUEASSERT(m_pListener);
			m_pListener->CBGCBubbleKickWall(m_pCurrentShootBubble);
		}//如果泡泡超出左边界，则截止到左边界
		else if (pBubble->m_nPosX < 0)
		{
			pBubble->m_nPosX = 0;
			pBubble->m_nAngle = 180 - pBubble->m_nAngle;//碰到左边界后要反弹
			pBubble->m_fOffsetX = 0.0f;
			//触发泡泡撞墙事件
			BLUEASSERT(m_pListener);
			m_pListener->CBGCBubbleKickWall(m_pCurrentShootBubble);
		}

		//如果泡泡超出了上边界，则截止到上边界
		if (pBubble->m_nPosY < 0)
			pBubble->m_nPosY = 0;

		//先判断泡泡是否与其他泡泡相撞
		//只可能与相邻四个格子内的泡泡相撞，不用所有泡泡都去判断
		CBubble* pLeftTopBubble;BLUEDWORD dwLTBRowIndex, dwLTBColIndex;bool bLTBIsExist;//代表当前泡泡左上角所在格的泡泡
		CBubble* pLeftBottomBubble;BLUEDWORD dwLBBRowIndex, dwLBBColIndex;bool bLBBIsExist;//代表当前泡泡左下角所在格的泡泡
		CBubble* pRightTopBubble;BLUEDWORD dwRTBRowIndex, dwRTBColIndex;bool bRTBIsExist;//代表当前泡泡右上角所在格的泡泡
		CBubble* pRightBottomBubble;BLUEDWORD dwRBBRowIndex, dwRBBColIndex;bool bRBBIsExist;//代表当前泡泡右下角所在格的泡泡
		CBubble* pCenterBubble;BLUEDWORD dwCBRowIndex, dwCBColIndex;bool bCBIsExist;//代表当前泡泡中心所在格的泡泡
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
			//如果撞到别的球的时候，球已经超出了底线，则直接认为游戏结束
			if (pBubble->m_nPosY + m_dwBubbleDiameter > m_dwHeight + 10)//ＸＸ有问题解决
			{
				//游戏结束
				NotifyGameOver();
				return false;
			}

			//如果当前的中心位置没有泡泡，则直接放到其中
			if (bCBIsExist && pCenterBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwCBRowIndex, dwCBColIndex);
				//触发泡泡固定事件
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//如果是正在射击中的泡泡，则要把该变量置BLUENULL，不然无法再发射
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//如果不是道具泡泡，则处理相同色
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwCBRowIndex, dwCBColIndex, pBubble);
				return true;
			}

			//如果当前的位置已经有了泡泡，则在四个角中找一个没有的泡泡的格子
			if (bLBBIsExist && pLeftBottomBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwLBBRowIndex, dwLBBColIndex);
				//触发泡泡固定事件
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//如果是正在射击中的泡泡，则要把该变量置BLUENULL，不然无法再发射
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//如果不是道具泡泡，则处理相同色
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwLBBRowIndex, dwLBBColIndex, pBubble);
				return true;
			}
			if (bRBBIsExist && pRightBottomBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwRBBRowIndex, dwRBBColIndex);
				//触发泡泡固定事件
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//如果是正在射击中的泡泡，则要把该变量置BLUENULL，不然无法再发射
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//如果不是道具泡泡，则处理相同色
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwRBBRowIndex, dwRBBColIndex, pBubble);
				return true;
			}
			if (bLTBIsExist && pLeftTopBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwLTBRowIndex, dwLTBColIndex);
				//触发泡泡固定事件
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//如果是正在射击中的泡泡，则要把该变量置BLUENULL，不然无法再发射
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//如果不是道具泡泡，则处理相同色
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwLTBRowIndex, dwLTBColIndex, pBubble);
				return true;
			}
			if (bRTBIsExist && pRightTopBubble == BLUENULL)
			{
				FitBubbleInMetrix(pBubble, dwRTBRowIndex, dwRTBColIndex);
				//触发泡泡固定事件
				BLUEASSERT(m_pListener);
				m_pListener->CBGCBubbleFix(pBubble);
				//如果是正在射击中的泡泡，则要把该变量置BLUENULL，不然无法再发射
				if (m_pCurrentShootingBubble == pBubble)
					m_pCurrentShootingBubble = BLUENULL;
				//如果不是道具泡泡，则处理相同色
				if (!pBubble->m_bIsBadBubble)
					SameColorBubbleProcess(dwRTBRowIndex, dwRTBColIndex, pBubble);
				return true;
			}

			//特殊情况
			BLUETRACE(_BLUET("泡泡碰撞特殊情况\n"));
			return true;
		}

		//如果已经到顶了而且没有碰撞，则直接置顶
		if (pBubble->m_nPosY == 0)
		{
			BLUEASSERT(bCBIsExist && pCenterBubble == BLUENULL);
			FitBubbleInMetrix(pBubble, dwCBRowIndex, dwCBColIndex);
			//触发泡泡固定事件
			BLUEASSERT(m_pListener);
			m_pListener->CBGCBubbleFix(pBubble);
			//如果是正在射击中的泡泡，则要把该变量置BLUENULL，不然无法再发射
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

	//游戏已经结束
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

	//游戏已经结束
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

	//游戏已经结束
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
	if (dwRowIndex >= m_dwRows) BLUETRACE(_BLUET("CCrazyBubbleGameCore::GetPosFromMetrix 行数超出行\n"));
	if (dwColIndex >= m_dwCols) BLUETRACE(_BLUET("CCrazyBubbleGameCore::GetPosFromMetrix 列数超出列\n"));
#endif

	//是否为奇数行
	((dwRowIndex & 1) != 1) ?
		nPosX = dwColIndex * m_dwBubbleDiameter ://奇数行的泡泡横坐标为正常坐标
		nPosX = dwColIndex * m_dwBubbleDiameter + m_dwBubbleRadius;//偶数行的泡泡横坐标需要加上一个半径，这样会卡在上一行两个泡泡中间

	//纵坐标与行数奇偶无关
	nPosY = dwRowIndex * m_dwRowHeight;
}

bool CCrazyBubbleGameCore::GetMetrixFromPos(
	const int nPosX,
	const int nPosY,
	BLUEDWORD& dwRowIndex,
	BLUEDWORD& dwColIndex
	)
{
	//如果坐标已经不在矩阵内了，则返回false
	if (nPosX >= (int)m_dwWidth ||
		nPosY >= (int)m_dwHeight ||
		nPosY < 0 ||
		nPosX < 0)
		return false;

	//得出行号
	dwRowIndex = nPosY / m_dwRowHeight;

	//奇数行格子与偶数行不一样
	//奇数行
	if ((dwRowIndex & 1) != 1)
		dwColIndex = nPosX / m_dwBubbleDiameter;
	else//偶数行
	{
		//偶行内第一格与最左边要空开一个半径的距离，这个距离内不会有格子存在
		((BLUEDWORD)nPosX < m_dwBubbleRadius) ?
			dwColIndex = 0 :
			dwColIndex = ((BLUEDWORD)nPosX - m_dwBubbleRadius) / m_dwBubbleDiameter;

		//偶行列数要比奇行列数少１，所以最后一列实际是不存在的
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

	//说明发生了碰撞
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

	//统计与该泡泡周围相同色的泡泡
	pBubble->m_nMark = 1;//本身要先标记
	m_sameColorBobbleColl.push_back(BLUEMAKELONG(dwRowIndex, dwColIndex));
	MarkSameColorBubble(dwRowIndex, dwColIndex, pBubble->m_color);

	//将所有可以消去的泡泡消去
	if (m_sameColorBobbleColl.size() > 2)
	{
		//触发监听者
		BLUEASSERT(m_pListener);
		m_pListener->CBGCBubbleEraseStart();

		//将消去的泡泡掉落
		for (SAMECOLORBUBBLECOLL::size_type i = 0; i < m_sameColorBobbleColl.size(); i++)
		{
			CBubble*& pRefClearBubble = GetBubbleInMetrix(
				BLUELOWORD(m_sameColorBobbleColl[i]),
				BLUEHIWORD(m_sameColorBobbleColl[i])
				);
			m_fixedBubbleColl.remove(pRefClearBubble);
			m_pListener->CBGCBubbleErase(pRefClearBubble);
			m_bubblePool.DestroyBubble(pRefClearBubble);
			pRefClearBubble = BLUENULL;//因为pRefClearBubble引用，所以直接操作将影响矩阵格子中的存储
		}

		//标记全部擦除，不能影响下次使用
		for (FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin(), itEnd = m_fixedBubbleColl.end(); it != itEnd; it++)
			(*it)->m_nMark = 0;

		//触发监听者
		m_pListener->CBGCBubbleEraseEnd((BLUEDWORD)m_sameColorBobbleColl.size());

		//不影响下次使用，相同色集合也要清空
		m_sameColorBobbleColl.clear();

		//将悬空的泡泡全部掉落
		InvaildBubbleProcess();
		return true;
	}
	else
	{
		//标记全部擦除，不能影响下次使用
		for (FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin(), itEnd = m_fixedBubbleColl.end(); it != itEnd; it++)
			(*it)->m_nMark = 0;

		//不影响下次使用，相同色集合也要清空
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

	bool bIsOddRow = ((dwRowIndex & 1) != 1);//是否为奇数行
	CBubble* pBubble;

	//得出该球左右，左上右上，左下右下六个泡泡的格子位置
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

	//将所有无效的泡泡加回到活动泡泡集合，并且设置为掉落行为
	FIXEDBUBBLECOLL::iterator it = m_fixedBubbleColl.begin();
	FIXEDBUBBLECOLL::iterator itEnd = m_fixedBubbleColl.end();
	FIXEDBUBBLECOLL::iterator delIt;
	BLUEDWORD dwDropBubbleNumber = 0;//掉落泡泡数量

	while (it != itEnd)
	{
		pBubble = (*it);
		//没有被标记过的全是无效泡泡
		if (pBubble->m_nMark == 0)
		{
			//有泡泡要掉，则先触发DropStart事件
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

	//是否要触发Drop事件
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

	bool bIsOddRow = ((dwRowIndex & 1) != 1);//是否为奇数行
	CBubble* pBubble;

	//得出该球左右，左下右下四个泡泡的格子位置（左上右上不用检测了）
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