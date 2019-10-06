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
接口名：火拼泡泡游戏核心类监听者
功能：略
说明：使用者实现该接口来处理核心类带来的所有事件通知
版本：1.0.0.1
作者：BLUE
*/
class ICrazyBubbleGameCoreListener
{
public:
	/*
	函数名：游戏结束（失败）
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCGameOver(void) = 0;

	/*
	函数名：创建一个新的泡泡事件
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCBubbleCreate(CBubble* pBubble) = 0;

	/*
	函数名：开始绘制制泡泡界面
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCStartDraw(void) = 0;

	/*
	函数名：结束绘制泡泡界面
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCEndDraw(void) = 0;

	/*
	函数名：绘制一个泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCDrawBubble(const CBubble* pBubble) = 0;

	/*
	函数名：绘制将要发射的泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCDrawCurrentShootBubble(const CBubble* pBubble) = 0;

	/*
	函数名：绘制下一个将要发射的泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCDrawNextShootBubble(const CBubble* pBubble) = 0;

	/*
	函数名：发射器旋转
	功能：略
	说明：无
	参数：angle 角度
	返回值：无
	*/
	virtual void CBGCWheelEmitter(const int nAngle) = 0;

	/*
	函数名：泡泡发射
	功能：略
	说明：当泡泡发射时触发该事件
	参数：无
	返回值：无
	*/
	virtual void CBGCBubbleShoot(const CBubble* pBubble) = 0;

	/*
	函数名：泡泡固定
	功能：略
	说明：当泡泡从活动状态转到固定状态时触发
	参数：无
	返回值：无
	*/
	virtual void CBGCBubbleFix(const CBubble* pBubble) = 0;

	/*
	函数名：泡泡撞墙
	功能：略
	说明：当泡泡撞到边界时触发
	参数：无
	返回值：无
	*/
	virtual void CBGCBubbleKickWall(const CBubble* pBubble) = 0;

	/*
	函数名：消去泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual void CBGCBubbleEraseStart(void) = 0;
	virtual void CBGCBubbleErase(const CBubble* pBubble) = 0;
	virtual void CBGCBubbleEraseEnd(const BLUEDWORD dwEraseNumber) = 0;

	/*
	函数名：泡泡掉落
	功能：略
	说明：有泡泡消去，然后多出的泡泡掉落时触发
	参数：无
	返回值：无
	*/
	virtual void CBGCBubbleDropStart(void) = 0;
	virtual void CBGCBubbleDrop(const CBubble* pBubble) = 0;
	virtual void CBGCBubbleDropEnd(const BLUEDWORD dwDropNumber) = 0;
};

/*
枚举名：泡泡颜色枚举
功能：表示泡泡的不同颜色
说明：无
*/
enum BUBBLECOLOR {
	red = 0,            //红色
	orange = 1,      //橙色
	blue = 2,          //蓝色
	white = 3,         //白色
	black = 4,         //黑色
	purple = 5,       //紫色
	green = 6,        //绿色
	yellow = 7        //黄色
};

/**
类名：泡泡类
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CBubble
{
	//友元类
	friend class CCrazyBubbleGameCore;

public:
	BUBBLECOLOR GetColor(void) const {return m_color;}
	BLUEDWORD GetPosX(void) const {return m_nPosX;}
	BLUEDWORD GetPosY(void) const {return m_nPosY;}

	//获取与设置用户自定义数据
	void SetCustomData(int nCustomData) {m_nCustomData = nCustomData;}
	int GetCurstomData(void) const {return m_nCustomData;}

private:
	BUBBLECOLOR m_color;//泡泡颜色
	int m_nPosX;//当前坐标x
	int m_nPosY;//当前坐标y
	int m_nAngle;//当前的发射角度
	float m_fOffsetY;//用于保存每次纵方向移动的增量
	float m_fOffsetX;//用于保存每次横方向移动的增量
	int m_nMark;//标记（额外用于临时保存色彩相同与掉落）
	BLUEDWORD m_dwMoveSpeed;//移动速度
	int m_bIsBadBubble;//是否为道具泡泡
	int m_nCustomData;//用户自定义数据


};

/**
类名：泡泡池
功能：用于内存分配泡泡与解释泡泡内存
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CBubblePool
{
	//内部泡泡结构，在CBubble结构上加了一个是否使用中选项，用于分配泡泡
	struct _Bubble :public CBubble {bool bIsUsing;};
	typedef std::vector<_Bubble*> BUBBLECOLL;

public:
	/*
	函数名：构造
	功能：略
	说明：无
	参数：dwPreAllocateBubbleNumber 预分配的泡泡数量
	返回值：无
	*/
	CBubblePool(BLUEDWORD dwPreAllocateBubbleNumber = 256);
	~CBubblePool(void);

	/*
	函数名：创建一个新的泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CBubble* CreateBubble(void);

	/*
	函数名：销毁一个泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void DestroyBubble(CBubble* pBubble);

private:
	/*
	变量名：泡泡集合
	功能：略
	说明：无
	*/
	BUBBLECOLL m_BubbleColl;

};

/**
类名：火拼泡泡游戏核心类
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCrazyBubbleGameCore
{
	typedef std::list<CBubble*> ACTIVEBUBBLECOLL;
	typedef std::list<CBubble*> FIXEDBUBBLECOLL;
	typedef std::vector<long> SAMECOLORBUBBLECOLL;

public:
	/*
	函数名：构造
	功能：略
	说明：无
	参数：无
	返回值：无
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
	函数名：游戏重置
	功能：略
	说明：游戏GameOver后可以使用该方法重置，这样又可以开始游戏了
	参数：nSeed 随机数种子
	　　　dwInitRows 初始时的行数
	返回值：无
	*/
	void Reset(const int nSeed, const BLUEDWORD dwInitRows);

	/*
	函数名：强制刷新
	功能：强制回调一下刷新，让观察者刷新
	说明：无
	参数：无
	返回值：无
	*/
	void CoerceRefresh(void);

	/*
	函数名：运行一次
	功能：运行时发射或掉落的泡泡才会动
	说明：要定时调用本函数才会动作起来
	参数：无
	返回值：无
	*/
	void RunOnce(void);

	/*
	函数名：发射泡泡
	功能：略
	说明：无
	参数：无
	返回值：true表示发射成功
	*/
	bool ShootBubble(void);

	/*
	函数名：发射坏泡泡
	功能：略
	说明：无
	参数：dwNumber 坏泡泡个数
	返回值：无
	*/
	bool ShootBadBubble(const BLUEDWORD dwNumber);

	/*
	函数名：消除泡泡
	功能：略
	说明：无
	参数：dwNumber消除的个数
	返回值：无
	*/
	bool EraseBubble(const BLUEDWORD dwNumber);

	/*
	函数名：增加破坏行泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool AddLineBadBubble(void);

	/*
	函数名：转动到指定的角度
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void WheelEmitter(const int nAngle);

	/*
	函数名：顺时针转动发射器
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void WheelEmitterDeasil(void);

	/*
	函数名：逆时针转动发射器
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void WheelEmitterWiddershins(void);

private:

	void NotifyGameOver(void) {m_bNotifyGameOver = true;}

	/*
	函数名：生成一个随机泡泡颜色
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	BUBBLECOLOR RandomBubbleColor(void) const {BLUEASSERT(m_pRandom);return (BUBBLECOLOR)m_pRandom->NextInt(8);}

	/*
	函数名：获取矩阵内某格子内泡泡对象
	功能：略
	说明：无
	参数：无
	返回值：如果格子内不存在泡泡则返回BLUENULL
	*/
	CBubble*& GetBubbleInMetrix(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex) const
	{
		BLUEASSERT(dwRowIndex < m_dwRows);
		BLUEASSERT(dwColIndex < m_dwCols);
		return m_BubbleMetrix[dwRowIndex * m_dwCols + dwColIndex];
	}

	/*
	函数名：获取矩阵内某格子内泡泡对象（安全）
	功能：略
	说明：比GetBubbleInMetrix安全，因为他检测参数是否有效
	参数：无
	返回值：如果格子内不存在泡泡或格子参数非法则返回BLUENULL
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
	函数名：将泡泡装入矩阵格子中
	功能：略
	说明：无
	参数：pBubble 要装入的泡泡，装入后pBubble的横纵坐标可能会改变
	　　　dwRowIndex 装入的横索引
	　　　dwColIndex 装入的纵索引
	返回值：无
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
	函数名：将泡泡装入矩阵格子中
	功能：略
	说明：与上面的FitBubbleInMetrix区别在于根据自己当前坐标来填入矩阵正确的格子
	参数：pBubble 要装入的泡泡
	返回值：无
	*/
	//void FitBubbleInMetrix(CBubble* pBubble);

	/*
	函数名：根据矩阵的格子算出其横纵坐标
	功能：略
	说明：无
	参数：dwRowIndex 横索引
	　　　dwColIndex 纵索引
	　　　nPosX 返回的横坐标
	　　　nPosY 返回的纵坐标
	返回值：无
	*/
	void GetPosFromMetrix(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex, int& nPosX, int& nPosY);

	/*
	函数名：根据横纵坐标获取在矩阵的哪个格子里
	功能：略
	说明：与GetPosInMetrix正好相反
	参数：nPosX 横坐标
	　　　nPosY 纵坐标
	　　　dwRowIndex 返回的横索引
	　　　dwColIndex 返回的纵索引
	返回值：返回false表示该坐标没有矩阵格子，返回true表示成功
	*/
	bool GetMetrixFromPos(const int nPosX, const int nPosY, BLUEDWORD& dwRowIndex, BLUEDWORD& dwColIndex);

	/*
	函数名：根据横纵坐标获取在矩阵的哪个格子里，并返回格子中的泡泡对象
	功能：略
	说明：与GetPosInMetrix正好相反
	　　　就算返回false，pBubble也会被填充为BLUENULL
	参数：nPosX 横坐标
	　　　nPosY 纵坐标
	　　　dwRowIndex 返回的横索引
	　　　dwColIndex 返回的纵索引
	　　　pBubble 返回的泡泡对象
	返回值：返回false表示该坐标没有矩阵格子，返回true表示成功
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
	函数名：泡泡活动
	功能：略
	说明：无
	参数：无
	返回值：如果返回true表示活动完成，泡泡不再活动，否则它还将继续活动
	*/
	bool BubbleActive(CBubble* pBubble);

	/*
	函数名：判断两个泡泡是否发生碰撞
	功能：略
	说明：无
	参数：pBubbleFirst 用于测试的第一个泡泡
	　　　pBubbleSecond 用于测试的第二个泡泡
	返回值：
	*/
	bool IsBubbleCollided(const CBubble* pBubbleFirst, const CBubble* pBubbleSecond);

	/*
	函数名：泡泡相同色处理
	功能：将与该格子中泡泡颜色相同的泡泡消去（３个以上包括３个），消去将悬空的泡泡掉落
	说明：无
	参数：dwRowIndex 格子行号
	　　　dwColIndex 格子列号
	　　　pBubble 该格子内的泡泡对象
	返回值：true表示有相同色泡泡被处理，false表示没有
	*/
	bool SameColorBubbleProcess(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex, CBubble* pBubble);

	/*
	函数名：
	功能：
	说明：无
	参数：dwRowIndex 格子行号
	　　　dwColIndex 格子列号
	返回值：无
		  */
	void MarkSameColorBubble(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex, const BUBBLECOLOR color);


	/*
	函数名：处理悬空泡泡（即最终不与边界相连的所有泡泡）
	功能：略
	说明：在有泡泡消掉处理后调用
	参数：无
	返回值：无
	*/
	void InvaildBubbleProcess(void);

	/*
	函数名：
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void MarkNoInvaildBubble(const BLUEDWORD dwRowIndex, const BLUEDWORD dwColIndex);

private:
	/*
	变量名：泡泡的半径
	功能：略
	说明：无
	*/
	const BLUEDWORD m_dwBubbleRadius;

	/*
	变量名：泡泡的直径
	功能：略
	说明：就是m_dwBubbleRadius * 2
	*/
	const BLUEDWORD m_dwBubbleDiameter;

	/*
	变量名：行高
	功能：略
	说明：行高不直接等于球的直径，因为上下两行球之间有相欠就是（直径 - 3^(1/2) * 半径）
	*/
	const BLUEDWORD m_dwRowHeight;

	/*
	变量名：碰撞常量
	功能：略
	说明：如果两泡泡发生碰撞，则他们横坐标之差平方加上纵坐标之差平方小于
	　　　4 * m_dwBubbleRadius * m_dwBubbleRadius，该变量就是保存这个固定值
	*/
	const BLUEDWORD m_dwBubbleCollidedConst;

	/*
	变量名：游戏行数（即一列可以放下几个泡泡）
	功能：略
	说明：无
	*/
	const BLUEDWORD m_dwRows;
	
	/*
	变量名：游戏列数（即一行可以放下几个泡泡）
	功能：略
	说明：无
	*/
	const BLUEDWORD m_dwCols;

	/*
	变量名：泡泡游戏区宽度
	功能：略
	说明：无
	*/
	const BLUEDWORD m_dwWidth;

	/*
	变量名：泡泡游戏区高度
	功能：略
	说明：无
	*/
	const BLUEDWORD m_dwHeight;

	/*
	变量名：游戏监听者接口
	功能：用于接受游戏中发生的事件并作处理
	说明：无
	*/
	ICrazyBubbleGameCoreListener* const m_pListener;

	/*
	变量名：泡泡矩阵
	功能：略
	说明：无
	*/
	CBubble** m_BubbleMetrix;

	/*
	变量名：泡泡矩阵尺寸
	功能：略
	说明：即等于m_dwRows * m_dwCols
	*/
	const BLUEDWORD m_dwBubbleMetrixSize;

	/*
	变量名：泡泡每次移动最大的距离
	功能：略
	说明：与m_dwBubbleMoveSpeed区别在于该参数在内部使用，当m_dwBubbleMoveSpeed大于本
	　　　本参数时，每次移动的距离拆分成几个小的移动距离多次移动
	*/
	const BLUEDWORD m_dwBubbleMaxMoveSpeed;

	/*
	变量名：普通泡泡每次移动的距离
	功能：略
	说明：无
	*/
	BLUEDWORD m_dwBubbleMoveSpeed;

	/*
	变量名：坏泡泡每次移动的距离
	功能：略
	说明：无
	*/
	BLUEDWORD m_dwBadBubbleMoveSpeed;

	/*
	变量名：泡泡池
	功能：用于分配泡泡
	说明：所有泡泡的产生与销毁都由泡泡池来处理
	*/
	CBubblePool m_bubblePool;

	/*
	变量名：当前正在射击中的泡泡（射出去了，正在运动中）
	功能：略
	说明：无
	*/
	CBubble* m_pCurrentShootingBubble;

	/*
	变量名：当前正要射的泡泡
	功能：略
	说明：无
	*/
	CBubble* m_pCurrentShootBubble;

	/*
	变量名：下一个要射击的泡泡
	功能：略
	说明：给玩家提供下一个泡泡的提示
	*/
	CBubble* m_pNextShootBubble;

	/*
	变量名：当前发射角度
	功能：略
	说明：无
	*/
	int m_nCurrentShootAngle;

	/*
	变量名：正在活动中的泡泡集合
	功能：略
	说明：整个游戏中分为两种泡泡，一种是固定泡泡，它们已经被射到固定点位，
	　　　已经座落于泡泡矩阵内，另一种为活动中泡泡，正在发射的泡泡或。。。
	*/
	ACTIVEBUBBLECOLL m_activeBubbleColl;

	/*
	变量名：固定泡泡集合
	功能：略
	说明：整个游戏中分为两种泡泡，一种是固定泡泡，它们已经被射到固定点位，
	　　　已经座落于泡泡矩阵内，另一种为活动中泡泡，正在发射的泡泡或。。。
	   */
	FIXEDBUBBLECOLL m_fixedBubbleColl;

	/*
	变量名：相同色球集合
	功能：略
	说明：仅在消除相同色球时使用
	*/
	SAMECOLORBUBBLECOLL m_sameColorBobbleColl;


	/*
	变量名：标记游戏是否结束，初始时为false
	功能：略
	说明：无
	*/
	bool m_bIsGameOver;

	/*
	变量名：通过游戏结束，如果为true，则再修改m_bIsGameOver标记
	功能：略
	说明：无
	*/
	bool m_bNotifyGameOver;

	/*
	变量名：随机器发生器
	功能：略
	说明：无
	*/
	CBLUERandom* m_pRandom;

	/*
	变量名：当前坏泡泡数量
	功能：略
	说明：坏泡泡一定是活动中的泡泡
	*/
	BLUEDWORD m_dwCurrentBadBubbleNumber;

	/*
	函数名：当前坏泡泡射出时是在第几列
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	BLUEDWORD m_dwCurrentBadBubbleShootColIndex;


};
