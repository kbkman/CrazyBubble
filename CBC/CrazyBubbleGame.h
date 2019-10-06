#pragma once
#include "crazybubblegamecore.h"
#include "CBCAnimationProcessor.h"
#include "CrazyBubbleGameToolManager.h"
#include "ResManager.h"
#include "CBCAudioProcessor.h"
#include "ClientUser.h"
#include "BLUEDirectX/engine2d.h"
#include "../Common/ToolKit.h"
#include <queue>

//游戏参数
#define CBG_ROWS                                         11       //泡泡行数
#define CBG_COLS                                           8        //泡泡列数
#define CBG_BUBBLE_RADIUS                         16      //泡泡半径
#define CBG_BUBBLEMOVE_SPEED                  25      //泡泡移动速度
#define CBG_BADBUBBLEMOVESPEED              45      //坏泡泡移动速度
#define CBG_GAMEAREATOTOP                        85      //实际游戏区离画布上端距离
#define CBG_GAMEAREATOLEFT                       28      //实际游戏区离画布左端距离
#define CBG_TOOLAREATOTOP                        10     //实际道具区离画布上端距离
#define CBG_TOOLAREATOLEFT                       309     //实际道具区离画布左端距离

/**
接口名：游戏监听者接口
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class ICrazyBubbleGameMachineListener
{
public:
	//函数名：游戏结束（失败）事件
	virtual void OnCBGMEventGameOver(void* pCallBackParam) = 0;

	//函数名：发射泡泡事件
	virtual void OnCBGMEventShootBubble(void* pCallBackParam, const int nAngle) = 0;

	//函数名：发射坏泡泡事件
	virtual void OnCBGMEventShootBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber) = 0;

	//函数名：消除泡泡事件
	virtual void OnCBGMEventEraseBubble(void* pCallBackParam, const BLUEDWORD dwNumber) = 0;

	//函数名：增加破坏行泡泡
	virtual void OnCBGMEventAddLineBadBubble(void* pCallBackParam) = 0;

	//函数名：隐藏泡泡颜色
	virtual void OnCBGMEventUnknowBubbleColor(void* pCallBackParam) = 0;

	//函数名：受扔便便操作
	virtual void OnCBGMEventThrowShit(void* pCallBackParam) = 0;

	//函数名：切换道具
	virtual void OnCBGMEventAdjustToolOrder(void* pCallBackParam) = 0;

	//函数名：更改泡泡模式
	virtual void OnCBGMEventChangeToolMode(void* pCallBackParam) = 0;

	//函数名：移除了一个道具（即使用了一个道具）
	virtual void OnCBGMEventRemoveTool(void* pCallBackParam) = 0;

	//函数名：发射计器到时
	virtual void OnCBGMEventShootTimerOver(void* pCallBackParam) = 0;

	//函数名：使用了道具（扔坏泡泡）
	virtual void OnCBGMEventUsingToolForAddBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber) = 0;

	//函数名：使用了道具（扔二行坏泡泡）
	virtual void OnCBGMEventUsingToolForLineBadBubble(void* pCallBackParam) = 0;

	//函数名：使用了道具（扔一行坏泡泡）
	virtual void OnCBGMEventUsingToolForAddOneLineBadBubble(void * pCallBackParam) = 0;

	//函数名：使用了道具（减一行泡泡）
	virtual void OnCBGMEventUsingToolForSubOneLineBadBubble(void* pCallBackParam) = 0;

	//函数名：使用了道具（隐藏泡泡颜色）
	virtual void OnCBGMEventUsingToolForUnknowBubbleColor(void* pCallBackParam) = 0;

	//函数名：使用了道具（扔便便）
	virtual void OnCBGMEventUsingToolForThrowShit(void* pCallBackParam) = 0;
};

/**
类名：发射计时器
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCShootTimer
{
public:
	CCBCShootTimer(void) {Reset();}

	//重置发射计时器
	void Reset(void)
	{
		m_nShootTimerLevel = 5;
		m_nShootTimer = 300;
	}

	/*
	函数名：运行计时器
	功能：略
	说明：无
	参数：无
	返回值：返回false表示已经
	*/
	bool RunOnce(void)
	{
		if (--m_nShootTimer <= 0)
		{
			if (--m_nShootTimerLevel <= 0)
			{
				Reset();
				return false;
			}
		}
		return true;
	}

private:
	/*
	变量名：泡泡发射计器用变量
	功能：略
	说明：无
	*/
	int m_nShootTimerLevel;
	int m_nShootTimer;
};

class CCrazyBubbleGame : private ICrazyBubbleGameCoreListener
{
	/*
	枚举名：
	功能：无
	说明：无
	*/
	enum GAMEACTIONTYPE
	{
		shootbubble,                       //射击泡泡
		shootbadbubble,                 //发射坏泡泡
		erasebubble,                       //消除泡泡
		addlinebadbubble,              //添加两行坏泡泡
		removetool,                        //删除道具
		changetoolmode,               //切换道具模式（泡泡模式/道具模式）
		unknowbubblecolor,          //隐藏泡泡颜色
		throwshit,                          //添加便便障碍
		adjusttoolorder                 //切换道具
	};

	/**
	结构名：
	功能：略
	说明：无
	版本：1.0.0.1
	作者：BLUE
	*/
	struct GAMEACTION
	{
		GAMEACTIONTYPE actionType;        //动作类型(参阅GAMEACTIONTYPE)
		int pf;                                              //参数1
		int ps;                                             //参数2
	};

	//游戏动作集合定义
	typedef std::queue<GAMEACTION> GAMEACTIONCOLL;

public:
	/*
	枚举名：游戏机状态枚举
	功能：无
	说明：无
	*/
	enum GAMEMACHINESTATE
	{
		nosituser,                      //无用户座入
		hassituser,                    //有用户座入
		preparegame,               //准备好开始游戏
		ingame,                         //游戏进行中
		gameover,                     //游戏结束（失败）
		gamesuccess                 //游戏结束（胜利）
	};

public:
	/*
	函数名：构造一个泡泡龙游戏
	功能：略
	说明：无
	参数：dd7 ddraw主接口
	　　　sfSrcBg 背景表面
	　　　sfSrcBubble 存放泡泡的表面
	　　　sfSrcShoot 存放发射枪的表面
	返回值：无
	*/
	CCrazyBubbleGame(
		ICrazyBubbleGameMachineListener* pListener,
		CDirectDraw7& dd7,
		const CResManager& resManager,
		CCBCAudioProcessor* pAudioProcessor
		);
	~CCrazyBubbleGame(void);

public:
	/*
	函数名：游戏坐入玩家
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool SitUser(const BLUEString& strName, const int nID);

	/*
	函数名：玩家离座
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool LeaveUser(void);

	/*
	函数名：获取当前坐在游戏机上的玩家对象
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CClientUser* GetSitUser(void) const {return m_pUser;}

	/*
	函数名：运行一次
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CDirectDrawSurface7& RunOne(void);

	/*
	函数名：开始游戏
	功能：略
	说明：无
	参数：nSeed 随机数种子
	　　　dwInitRows 初始行数
	　　　nBringToolRate 携带道具的概率,0到100
	返回值：无
	*/
	bool StartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate);

	/*
	函数名：结束游戏
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool StopGame(void);

	/*
	函数名：获取游戏机状态
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	GAMEMACHINESTATE GetState(void) const {return m_state;}

	/*
	函数名：设置回调参数
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SetCallBackParam(void* pCallBackParam) {m_pCallBackParam = pCallBackParam;}

	/*
	函数名：获取回调参数
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void* GetCallBackParam(void) const {return m_pCallBackParam;}

	/*
	函数名：是否播放声音
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool IsPlaySound(void) const {return m_bPlaySound;}

	/*
	函数名：设置是否播放声音
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void SetPlaySound(const bool bPlaySound) {m_bPlaySound = bPlaySound;}

	/*
	函数名：获取当前发射泡泡的角度
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	int GetCurrentShootAngle(void) const
	{
		BLUEASSERT(m_pGameCore);
		return m_pGameCore->GetCurrentShootAngle();
	}

	/*
	函数名：更改道具模式，是扔泡泡模式还是扔道具模式
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void ChangeToolMode(void)
	{
		BLUEASSERT(m_pGameCore);
		BLUEASSERT(m_pListener);
		m_toolManager.UsingTool(!m_toolManager.IsUsingTool());
		m_pGameCore->CoerceRefresh();
		/*
		if (IsPlaySound())
			m_pAudioProcessor->PlayEffect(MUSICLINE_CHANGETOOL);
		*/
		m_pListener->OnCBGMEventChangeToolMode(m_pCallBackParam);
	}

	/*
	函数名：使用道具
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void UseTool(void);

	/*
	函数名：发射泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool ShootBubble(void)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->ShootBubble())
		{
			m_shootTimer.Reset();
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventShootBubble(m_pCallBackParam, GetCurrentShootAngle());
			//每发射一个泡泡，隐藏泡泡个数减一
			if (m_nUnknowBubbleColorNum > 0)
				m_nUnknowBubbleColorNum--;
			return true;
		}
		return false;
	}

	/*
	函数名：发射坏泡泡
	功能：略
	说明：无
	参数：dwNumber 坏泡泡个数
	返回值：无
	*/
	bool ShootBadBubble(const BLUEDWORD dwNumber)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->ShootBadBubble(dwNumber))
		{
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventShootBadBubble(m_pCallBackParam, dwNumber);
			return true;
		}
		return false;
	}

	/*
	函数名：消除泡泡
	功能：略
	说明：无
	参数：dwNumber消除的个数
	返回值：无
	*/
	bool EraseBubble(const BLUEDWORD dwNumber)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->EraseBubble(dwNumber))
		{
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventEraseBubble(m_pCallBackParam, dwNumber);
			return true;
		}
		return false;
	}

	/*
	函数名：增加破坏行泡泡
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool AddLineBadBubble(void)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->AddLineBadBubble())
		{
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventAddLineBadBubble(m_pCallBackParam);
			return true;
		}
		return false;
	}

	void UnknowBubbleColor(void)
	{
		BLUEASSERT(m_pGameCore);
		BLUEASSERT(m_pListener);
		m_nUnknowBubbleColorNum += 4;
		m_pGameCore->CoerceRefresh();
		m_pListener->OnCBGMEventUnknowBubbleColor(m_pCallBackParam);
	}

	void ThrowShit(void)
	{
		//便便声音（无论如何都要播放，不受IsPlaySound限制）
		/*
		BLUEASSERT(m_pAudioProcessor);
		m_pAudioProcessor->PlayEffect(MUSICLINE_SHIT);
		*/

		//添加便便动画
		m_aniManager.AddShit();

		//触发事件
		BLUEASSERT(m_pListener);
		m_pListener->OnCBGMEventThrowShit(m_pCallBackParam);
	}

	void AdjustToolOrder(void)
	{
		if (m_toolManager.AdjustToolOrder())
		{
			BLUEASSERT(m_pGameCore);
			BLUEASSERT(m_pListener);
			m_pGameCore->CoerceRefresh();
			m_pListener->OnCBGMEventAdjustToolOrder(m_pCallBackParam);
		}
	}

	//转换相关
	void WheelEmitter(const int nAngle) {BLUEASSERT(m_pGameCore);m_pGameCore->WheelEmitter(nAngle);}
	void WheelEmitterDeasil(void) {BLUEASSERT(m_pGameCore);m_spriteLong.NextFrame();m_pGameCore->WheelEmitterDeasil();}
	void WheelEmitterWiddershins(void) {BLUEASSERT(m_pGameCore);m_spriteLong.PrevFrame();m_pGameCore->WheelEmitterWiddershins();}

	//动作相关
	void ActionShootBubble(const int nAngle);
	void ActionShootBadBubble(const BLUEDWORD dwNumber);
	void ActionEraseBubble(const BLUEDWORD dwNumber);
	void ActionAddLineBadBubble(void);
	void ActionChangeToolMode(void);
	void ActionRemoveTool(void);
	void ActionUnknowBubbleColor(void);//泡泡颜色无法看见
	void ActionThrowShit(void);//被扔到便便
	void ActionAdjustToolOrder(void);//切换道具顺序

	//游戏核心事件
	virtual void CBGCGameOver(void);
	virtual void CBGCBubbleCreate(CBubble* pBubble);
	virtual void CBGCStartDraw(void);
	virtual void CBGCEndDraw(void);
	virtual void CBGCDrawBubble(const CBubble* pBubble);
	virtual void CBGCDrawCurrentShootBubble(const CBubble* pBubble);
	virtual void CBGCDrawNextShootBubble(const CBubble* pBubble);
	virtual void CBGCWheelEmitter(const int nAngle);
	virtual void CBGCBubbleShoot(const CBubble* pBubble);
	virtual void CBGCBubbleFix(const CBubble* pBubble);
	virtual void CBGCBubbleKickWall(const CBubble* pBubble);
	virtual void CBGCBubbleDropStart(void);
	virtual void CBGCBubbleDrop(const CBubble* pBubble);
	virtual void CBGCBubbleDropEnd(const BLUEDWORD dwDropNumber);
	virtual void CBGCBubbleEraseStart(void);
	virtual void CBGCBubbleErase(const CBubble* pBubble);
	virtual void CBGCBubbleEraseEnd(const BLUEDWORD dwEraseNumber);

public:
	/*
	变量名：保存绘制到屏幕上的区域，由外部调用
	功能：略
	说明：无
	*/
	RECT m_drawRc;

	/*
	变量名：外部使用的ID号，玩家使用道具时按照该ID号使用道具
	功能：略
	说明：该ID号与玩家在房间内的ID与不同
	*/
	int m_nExternUseID;

private:
	/*
	函数名：处理动作队列
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void ProcessActionQueue(void);

private:
	/*
	变量名：游戏机状态
	功能：略
	说明：无
	*/
	GAMEMACHINESTATE m_state;

	/*
	变量名：监听者
	功能：略
	说明：无
	*/
	ICrazyBubbleGameMachineListener* m_pListener;

	/*
	变量名：在房间中当前的ID号
	功能：略
	说明：默认为-1，表示没有座位
	*/
	int m_nIDInRoom;

	/*
	变量名：随机数器
	功能：略
	说明：无
	*/
	CBLUERandom* m_pRandom;

	/*
	变量名：游戏核心对象
	功能：略
	说明：无
	*/
	CCrazyBubbleGameCore* m_pGameCore;

	/*
	变量名：玩游戏的用户对象
	功能：略
	说明：无
	*/
	CClientUser* m_pUser;

	/*
	函数名：游戏最终画布
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CDirectDrawSurface7 m_surfaceCanvas;

	/*
	变量名：游戏核心用的画布表面
	功能：用于绘制所有游戏中的内容
	说明：无
	*/
	CDirectDrawSurface7 m_surfaceGameCoreCanvas;

	/*
	变量名：存放玩家名称的源表面
	功能：略
	说明：无
	*/
	CDirectDrawSurface7 m_surfaceUserName;

	/*
	变量名：资源管理器
	功能：略
	说明：无
	*/
	const CResManager& m_resManager;

	//精灵
	CDDrawSprite m_spriteBubble;//泡泡精灵
	CDDrawSprite m_spriteLong;//龙精灵
	CDDrawSprite m_spriteTool;//道具精灵
	CDDrawSprite m_spriteUserNum;//用户编号精灵

	/*
	变量名：用于绘制射击枪的表面
	功能：略
	说明：无
	*/
	CDirectDrawSurface7 m_surfaceShoot;

	/*
	变量名：是否播放声音
	功能：略
	说明：无
	*/
	bool m_bPlaySound;

	/*
	变量名：音频发生器
	功能：略
	说明：无
	*/
	CCBCAudioProcessor* m_pAudioProcessor;

	/*
	变量名：回调参数
	功能：略
	说明：无
	*/
	void* m_pCallBackParam;

	/*
	变量名：动作队列
	功能：略
	说明：无
	*/
	GAMEACTIONCOLL m_actionQueue;

	/*
	变量名：道具管理器
	功能：略
	说明：无
	*/
	CCrazyBubbleGameToolManager m_toolManager;

	/*
	变量名：动画管理器
	功能：略
	说明：无
	*/
	CCBCAnimationProcessor m_aniManager;

	/*
	变量名：携带道具的概率
	功能：略
	说明：无
	*/
	int m_nBringToolRate;

	/*
	变量名：隐藏泡泡个数
	功能：略
	说明：无
	*/
	int m_nUnknowBubbleColorNum;

	/*
	函数名：发射计时器
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CCBCShootTimer m_shootTimer;
};
