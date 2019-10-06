#pragma once
#include "BLUEDef.h"
#include "BLUEDebug.h"
#include "BLUERandom.h"
#include <vector>
#include <list>
#include <algorithm>
#include "../Common/ToolKit.h"

class CBubble;
class CBubblePool;
class CCrazyBubbleGameCore;
class ICrazyBubbleGameCoreListener;

/**
�ӿ�������ƴ������Ϸ�����������
���ܣ���
˵����ʹ����ʵ�ָýӿ����������������������¼�֪ͨ
�汾��1.0.0.1
���ߣ�BLUE
*/
class ICrazyBubbleGameCoreListener
{
public:
	/*
	����������Ϸ������ʧ�ܣ�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCGameOver(void) = 0;

	/*
	������������һ���µ������¼�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCBubbleCreate(CBubble* pBubble) = 0;

	/*
	����������ʼ���������ݽ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCStartDraw(void) = 0;

	/*
	�������������������ݽ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCEndDraw(void) = 0;

	/*
	������������һ������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCDrawBubble(const CBubble* pBubble) = 0;

	/*
	�����������ƽ�Ҫ���������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCDrawCurrentShootBubble(const CBubble* pBubble) = 0;

	/*
	��������������һ����Ҫ���������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCDrawNextShootBubble(const CBubble* pBubble) = 0;

	/*
	����������������ת
	���ܣ���
	˵������
	������angle �Ƕ�
	����ֵ����
	*/
	virtual void CBGCWheelEmitter(const int nAngle) = 0;

	/*
	�����������ݷ���
	���ܣ���
	˵���������ݷ���ʱ�������¼�
	��������
	����ֵ����
	*/
	virtual void CBGCBubbleShoot(const CBubble* pBubble) = 0;

	/*
	�����������ݹ̶�
	���ܣ���
	˵���������ݴӻ״̬ת���̶�״̬ʱ����
	��������
	����ֵ����
	*/
	virtual void CBGCBubbleFix(const CBubble* pBubble) = 0;

	/*
	������������ײǽ
	���ܣ���
	˵����������ײ���߽�ʱ����
	��������
	����ֵ����
	*/
	virtual void CBGCBubbleKickWall(const CBubble* pBubble) = 0;

	/*
	����������ȥ����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual void CBGCBubbleEraseStart(void) = 0;
	virtual void CBGCBubbleErase(const CBubble* pBubble) = 0;
	virtual void CBGCBubbleEraseEnd(const BLUEDWORD dwEraseNumber) = 0;

	/*
	�����������ݵ���
	���ܣ���
	˵������������ȥ��Ȼ���������ݵ���ʱ����
	��������
	����ֵ����
	*/
	virtual void CBGCBubbleDropStart(void) = 0;
	virtual void CBGCBubbleDrop(const CBubble* pBubble) = 0;
	virtual void CBGCBubbleDropEnd(const BLUEDWORD dwDropNumber) = 0;
};

/*
ö������������ɫö��
���ܣ���ʾ���ݵĲ�ͬ��ɫ
˵������
*/
enum BUBBLECOLOR {
	red = 0,            //��ɫ
	orange = 1,      //��ɫ
	blue = 2,          //��ɫ
	white = 3,         //��ɫ
	black = 4,         //��ɫ
	purple = 5,       //��ɫ
	green = 6,        //��ɫ
	yellow = 7        //��ɫ
};

/**
������������
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CBubble
{
	//��Ԫ��
	friend class CCrazyBubbleGameCore;

public:
	BUBBLECOLOR GetColor(void) const {return m_color;}
	BLUEDWORD GetPosX(void) const {return m_nPosX;}
	BLUEDWORD GetPosY(void) const {return m_nPosY;}

	//��ȡ�������û��Զ�������
	void SetCustomData(int nCustomData) {m_nCustomData = nCustomData;}
	int GetCurstomData(void) const {return m_nCustomData;}

private:
	BUBBLECOLOR m_color;//������ɫ
	int m_nPosX;//��ǰ����x
	int m_nPosY;//��ǰ����y
	int m_nAngle;//��ǰ�ķ���Ƕ�
	float m_fOffsetY;//���ڱ���ÿ���ݷ����ƶ�������
	float m_fOffsetX;//���ڱ���ÿ�κ᷽���ƶ�������
	int m_nMark;//��ǣ�����������ʱ����ɫ����ͬ����䣩
	BLUEDWORD m_dwMoveSpeed;//�ƶ��ٶ�
	int m_bIsBadBubble;//�Ƿ�Ϊ��������
	int m_nCustomData;//�û��Զ�������


};

/**
���������ݳ�
���ܣ������ڴ������������������ڴ�
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CBubblePool
{
	//�ڲ����ݽṹ����CBubble�ṹ�ϼ���һ���Ƿ�ʹ����ѡ����ڷ�������
	struct _Bubble :public CBubble {bool bIsUsing;};
	typedef std::vector<_Bubble*> BUBBLECOLL;

public:
	/*
	������������
	���ܣ���
	˵������
	������dwPreAllocateBubbleNumber Ԥ�������������
	����ֵ����
	*/
	CBubblePool(BLUEDWORD dwPreAllocateBubbleNumber = 256);
	~CBubblePool(void);

	/*
	������������һ���µ�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CBubble* CreateBubble(void);

	/*
	������������һ������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void DestroyBubble(CBubble* pBubble);

private:
	/*
	�����������ݼ���
	���ܣ���
	˵������
	*/
	BUBBLECOLL m_BubbleColl;

};

/**
��������ƴ������Ϸ������
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCrazyBubbleGameCore
{
	typedef std::list<CBubble*> ACTIVEBUBBLECOLL;
	typedef std::list<CBubble*> FIXEDBUBBLECOLL;
	typedef std::vector<long> SAMECOLORBUBBLECOLL;

public:
	/*
	������������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CCrazyBubbleGameCore(
		const int nSeed,
		const BLUEDWORD dwBubbleRadius,
		const BLUEDWORD dwRows,
		const BLUEDWORD dwCols,
		const BLUEDWORD dwBubbleMoveSpeed,
		const BLUEDWORD dwBadBubbleMoveSpeed,
		ICrazyBubbleGameCoreListener* pListener,
		const BLUEDWORD dwInitRows
		);
	~CCrazyBubbleGameCore(void);

public:
	bool IsGameOver(void) const {return m_bIsGameOver;}
	BLUEDWORD GetWidth(void) const {return m_dwWidth;}
	BLUEDWORD GetHeight(void) const {return m_dwHeight;}
	BLUEDWORD GetBubbleRadius(void) const {return m_dwBubbleRadius;}
	BLUEDWORD GetBubbleDiameter(void) const {return m_dwBubbleDiameter;}
	BLUEDWORD GetBubbleMoveSpeed(void) const {return m_dwBubbleMoveSpeed;}
	BLUEDWORD GetBadBubbleMoveSpeed(void) const {return m_dwBadBubbleMoveSpeed;}
	void SetBubbleMoveSpeed(BLUEDWORD dwSpeed) {if (dwSpeed > 0) m_dwBubbleMoveSpeed = dwSpeed;}
	void SetBadBubbleMoveSpeed(BLUEDWORD dwSpeed) {if (dwSpeed > 0) m_dwBadBubbleMoveSpeed = dwSpeed;}
	BUBBLECOLOR GetCurrentShootBubbleColor(void) const {BLUEASSERT(m_pCurrentShootBubble);return m_pCurrentShootBubble->m_color;}
	BUBBLECOLOR GetNextShootBubbleColor(void) const {BLUEASSERT(m_pNextShootBubble);return m_pNextShootBubble->m_color;}
	int GetCurrentShootBubbleCustomData(void) const {BLUEASSERT(m_pCurrentShootBubble);return m_pCurrentShootBubble->GetCurstomData();}
	int GetNextShootBubbleCustomData(void) const {BLUEASSERT(m_pCurrentShootBubble);return m_pNextShootBubble->GetCurstomData();}
	int GetCurrentShootAngle(void) const {return m_nCurrentShootAngle;}

	/*
	����������Ϸ����
	���ܣ���
	˵������ϷGameOver�����ʹ�ø÷������ã������ֿ��Կ�ʼ��Ϸ��
	������nSeed ���������
	������dwInitRows ��ʼʱ������
	����ֵ����
	*/
	void Reset(const int nSeed, const BLUEDWORD dwInitRows);

	/*
	��������ǿ��ˢ��
	���ܣ�ǿ�ƻص�һ��ˢ�£��ù۲���ˢ��
	˵������
	��������
	����ֵ����
	*/
	void CoerceRefresh(void);

	/*
	������������һ��
	���ܣ�����ʱ������������ݲŻᶯ
	˵����Ҫ��ʱ���ñ������Żᶯ������
	��������
	����ֵ����
	*/
	void RunOnce(void);

	/*
	����������������
	���ܣ���
	˵������
	��������
	����ֵ��true��ʾ����ɹ�
	*/
	bool ShootBubble(void);

	/*
	�����������仵����
	���ܣ���
	˵������
	������dwNumber �����ݸ���
	����ֵ����
	*/
	bool ShootBadBubble(const BLUEDWORD dwNumber);

	/*
	����������������
	���ܣ���
	˵������
	������dwNumber�����ĸ���
	����ֵ����
	*/
	bool EraseBubble(const BLUEDWORD dwNumber);

	/*
	�������������ƻ�������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool AddLineBadBubble(void);

	/*
	��������ת����ָ���ĽǶ�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void WheelEmitter(const int nAngle);

	/*
	��������˳ʱ��ת��������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void WheelEmitterDeasil(void);

	/*
	����������ʱ��ת��������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void WheelEmitterWiddershins(void);

private:

	void NotifyGameOver(void) {m_bNotifyGameOver = true;}

	/*
	������������һ�����������ɫ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	BUBBLECOLOR RandomBubbleColor(void) const {BLUEASSERT(m_pRandom);return (BUBBLECOLOR)m_pRandom->NextInt(8);}

	/*
	����������ȡ������ĳ���������ݶ���
	���ܣ���
	˵������
	��������
	����ֵ����������ڲ����������򷵻�BLUENULL
	*/
	CBubble*& GetBubbleInMetrix(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex) const
	{
		BLUEASSERT(dwRowIndex < m_dwRows);
		BLUEASSERT(dwColIndex < m_dwCols);
		return m_BubbleMetrix[dwRowIndex * m_dwCols + dwColIndex];
	}

	/*
	����������ȡ������ĳ���������ݶ��󣨰�ȫ��
	���ܣ���
	˵������GetBubbleInMetrix��ȫ����Ϊ���������Ƿ���Ч
	��������
	����ֵ����������ڲ��������ݻ���Ӳ����Ƿ��򷵻�BLUENULL
	*/
	CBubble* GetBubbleInMetrixSafe(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex) const
	{
		if (dwRowIndex >= m_dwRows)
			return BLUENULL;

		if (dwColIndex >= m_dwCols)
			return BLUENULL;

		return m_BubbleMetrix[dwRowIndex * m_dwCols + dwColIndex];
	}

	/*
	��������������װ����������
	���ܣ���
	˵������
	������pBubble Ҫװ������ݣ�װ���pBubble�ĺ���������ܻ�ı�
	������dwRowIndex װ��ĺ�����
	������dwColIndex װ���������
	����ֵ����
	*/
	void FitBubbleInMetrix(CBubble* pBubble, const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex)
	{
		BLUEASSERT(pBubble);
		BLUEASSERT(dwRowIndex < m_dwRows);
		BLUEASSERT(dwColIndex < m_dwCols);
		BLUEASSERT(GetBubbleInMetrix(dwRowIndex, dwColIndex) == BLUENULL);
		BLUEASSERT(std::find(m_fixedBubbleColl.begin(), m_fixedBubbleColl.end(), pBubble) == m_fixedBubbleColl.end());

		GetPosFromMetrix(dwRowIndex, dwColIndex, pBubble->m_nPosX, pBubble->m_nPosY);
		GetBubbleInMetrix(dwRowIndex, dwColIndex) = pBubble;
		m_fixedBubbleColl.push_back(pBubble);
	}

	/*
	��������������װ����������
	���ܣ���
	˵�����������FitBubbleInMetrix�������ڸ����Լ���ǰ���������������ȷ�ĸ���
	������pBubble Ҫװ�������
	����ֵ����
	*/
	//void FitBubbleInMetrix(CBubble* pBubble);

	/*
	�����������ݾ���ĸ���������������
	���ܣ���
	˵������
	������dwRowIndex ������
	������dwColIndex ������
	������nPosX ���صĺ�����
	������nPosY ���ص�������
	����ֵ����
	*/
	void GetPosFromMetrix(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex, int& nPosX, int& nPosY);

	/*
	�����������ݺ��������ȡ�ھ�����ĸ�������
	���ܣ���
	˵������GetPosInMetrix�����෴
	������nPosX ������
	������nPosY ������
	������dwRowIndex ���صĺ�����
	������dwColIndex ���ص�������
	����ֵ������false��ʾ������û�о�����ӣ�����true��ʾ�ɹ�
	*/
	bool GetMetrixFromPos(const int nPosX, const int nPosY, BLUEDWORD& dwRowIndex, BLUEDWORD& dwColIndex);

	/*
	�����������ݺ��������ȡ�ھ�����ĸ�����������ظ����е����ݶ���
	���ܣ���
	˵������GetPosInMetrix�����෴
	���������㷵��false��pBubbleҲ�ᱻ���ΪBLUENULL
	������nPosX ������
	������nPosY ������
	������dwRowIndex ���صĺ�����
	������dwColIndex ���ص�������
	������pBubble ���ص����ݶ���
	����ֵ������false��ʾ������û�о�����ӣ�����true��ʾ�ɹ�
	*/
	bool GetMetrixFromPos(const int nPosX, const int nPosY, BLUEDWORD& dwRowIndex, BLUEDWORD& dwColIndex, CBubble*& pBubble)
	{
		if (GetMetrixFromPos(nPosX, nPosY, dwRowIndex, dwColIndex))
		{
				pBubble = GetBubbleInMetrix(dwRowIndex, dwColIndex);
				return true;
		}
		pBubble = BLUENULL;
		return false;
	}

	/*
	�����������ݻ
	���ܣ���
	˵������
	��������
	����ֵ���������true��ʾ���ɣ����ݲ��ٻ�����������������
	*/
	bool BubbleActive(CBubble* pBubble);

	/*
	���������ж����������Ƿ�����ײ
	���ܣ���
	˵������
	������pBubbleFirst ���ڲ��Եĵ�һ������
	������pBubbleSecond ���ڲ��Եĵڶ�������
	����ֵ��
	*/
	bool IsBubbleCollided(const CBubble* pBubbleFirst, const CBubble* pBubbleSecond);

	/*
	��������������ͬɫ����
	���ܣ�����ø�����������ɫ��ͬ��������ȥ���������ϰ�������������ȥ�����յ����ݵ���
	˵������
	������dwRowIndex �����к�
	������dwColIndex �����к�
	������pBubble �ø����ڵ����ݶ���
	����ֵ��true��ʾ����ͬɫ���ݱ�����false��ʾû��
	*/
	bool SameColorBubbleProcess(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex, CBubble* pBubble);

	/*
	��������
	���ܣ�
	˵������
	������dwRowIndex �����к�
	������dwColIndex �����к�
	����ֵ����
		  */
	void MarkSameColorBubble(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex, const BUBBLECOLOR color);


	/*
	�������������������ݣ������ղ���߽��������������ݣ�
	���ܣ���
	˵������������������������
	��������
	����ֵ����
	*/
	void InvaildBubbleProcess(void);

	/*
	��������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void MarkNoInvaildBubble(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex);

private:
	/*
	�����������ݵİ뾶
	���ܣ���
	˵������
	*/
	const BLUEDWORD m_dwBubbleRadius;

	/*
	�����������ݵ�ֱ��
	���ܣ���
	˵��������m_dwBubbleRadius * 2
	*/
	const BLUEDWORD m_dwBubbleDiameter;

	/*
	���������и�
	���ܣ���
	˵�����и߲�ֱ�ӵ������ֱ������Ϊ����������֮������Ƿ���ǣ�ֱ�� - 3^(1/2) * �뾶��
	*/
	const BLUEDWORD m_dwRowHeight;

	/*
	����������ײ����
	���ܣ���
	˵������������ݷ�����ײ�������Ǻ�����֮��ƽ������������֮��ƽ��С��
	������4 * m_dwBubbleRadius * m_dwBubbleRadius���ñ������Ǳ�������̶�ֵ
	*/
	const BLUEDWORD m_dwBubbleCollidedConst;

	/*
	����������Ϸ��������һ�п��Է��¼������ݣ�
	���ܣ���
	˵������
	*/
	const BLUEDWORD m_dwRows;
	
	/*
	����������Ϸ��������һ�п��Է��¼������ݣ�
	���ܣ���
	˵������
	*/
	const BLUEDWORD m_dwCols;

	/*
	��������������Ϸ�����
	���ܣ���
	˵������
	*/
	const BLUEDWORD m_dwWidth;

	/*
	��������������Ϸ���߶�
	���ܣ���
	˵������
	*/
	const BLUEDWORD m_dwHeight;

	/*
	����������Ϸ�����߽ӿ�
	���ܣ����ڽ�����Ϸ�з������¼���������
	˵������
	*/
	ICrazyBubbleGameCoreListener* const m_pListener;

	/*
	�����������ݾ���
	���ܣ���
	˵������
	*/
	CBubble** m_BubbleMetrix;

	/*
	�����������ݾ���ߴ�
	���ܣ���
	˵����������m_dwRows * m_dwCols
	*/
	const BLUEDWORD m_dwBubbleMetrixSize;

	/*
	������������ÿ���ƶ����ľ���
	���ܣ���
	˵������m_dwBubbleMoveSpeed�������ڸò������ڲ�ʹ�ã���m_dwBubbleMoveSpeed���ڱ�
	������������ʱ��ÿ���ƶ��ľ����ֳɼ���С���ƶ��������ƶ�
	*/
	const BLUEDWORD m_dwBubbleMaxMoveSpeed;

	/*
	����������ͨ����ÿ���ƶ��ľ���
	���ܣ���
	˵������
	*/
	BLUEDWORD m_dwBubbleMoveSpeed;

	/*
	��������������ÿ���ƶ��ľ���
	���ܣ���
	˵������
	*/
	BLUEDWORD m_dwBadBubbleMoveSpeed;

	/*
	�����������ݳ�
	���ܣ����ڷ�������
	˵�����������ݵĲ��������ٶ������ݳ�������
	*/
	CBubblePool m_bubblePool;

	/*
	����������ǰ��������е����ݣ����ȥ�ˣ������˶��У�
	���ܣ���
	˵������
	*/
	CBubble* m_pCurrentShootingBubble;

	/*
	����������ǰ��Ҫ�������
	���ܣ���
	˵������
	*/
	CBubble* m_pCurrentShootBubble;

	/*
	����������һ��Ҫ���������
	���ܣ���
	˵����������ṩ��һ�����ݵ���ʾ
	*/
	CBubble* m_pNextShootBubble;

	/*
	����������ǰ����Ƕ�
	���ܣ���
	˵������
	*/
	int m_nCurrentShootAngle;

	/*
	�����������ڻ�е����ݼ���
	���ܣ���
	˵����������Ϸ�з�Ϊ�������ݣ�һ���ǹ̶����ݣ������Ѿ����䵽�̶���λ��
	�������Ѿ����������ݾ����ڣ���һ��Ϊ������ݣ����ڷ�������ݻ򡣡���
	*/
	ACTIVEBUBBLECOLL m_activeBubbleColl;

	/*
	���������̶����ݼ���
	���ܣ���
	˵����������Ϸ�з�Ϊ�������ݣ�һ���ǹ̶����ݣ������Ѿ����䵽�̶���λ��
	�������Ѿ����������ݾ����ڣ���һ��Ϊ������ݣ����ڷ�������ݻ򡣡���
	   */
	FIXEDBUBBLECOLL m_fixedBubbleColl;

	/*
	����������ͬɫ�򼯺�
	���ܣ���
	˵��������������ͬɫ��ʱʹ��
	*/
	SAMECOLORBUBBLECOLL m_sameColorBobbleColl;


	/*
	�������������Ϸ�Ƿ��������ʼʱΪfalse
	���ܣ���
	˵������
	*/
	bool m_bIsGameOver;

	/*
	��������ͨ����Ϸ���������Ϊtrue�������޸�m_bIsGameOver���
	���ܣ���
	˵������
	*/
	bool m_bNotifyGameOver;

	/*
	�������������������
	���ܣ���
	˵������
	*/
	CBLUERandom* m_pRandom;

	/*
	����������ǰ����������
	���ܣ���
	˵����������һ���ǻ�е�����
	*/
	BLUEDWORD m_dwCurrentBadBubbleNumber;

	/*
	����������ǰ���������ʱ���ڵڼ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	BLUEDWORD m_dwCurrentBadBubbleShootColIndex;


};
