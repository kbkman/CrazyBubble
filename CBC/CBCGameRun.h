#pragma once

#include "BLUEDirectX/game2d.h"
#include "ResManager.h"
#include "CBCGUIProcessor.h"
#include "CBCNetWorkProcessor.h"
#include "CBCAudioProcessor.h"
#include "ClientRoomManager.h"
#include "ClientUserManager.h"
#include <vector>
#include "CrazyBubbleGame.h"

/**
类名：游戏运行类
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCGameRun :
	public CDXGameRun2D,
	private ICBCGUIProcessorListener,
	private ICBCNetWorkProcessorListener,
	private ICrazyBubbleGameMachineListener
{
	typedef std::vector<CCrazyBubbleGame*> GAMECOLL;

	//游戏运行状态
	enum GAMERUNSTATE
	{
		logo,                             //logo界面
		login,                            //登陆界面
		outroom,                      //房间外界面
		inroom                          //房间内界面
	};

public:
	CCBCGameRun(void);
	~CCBCGameRun(void);

private:
	virtual HRESULT GameInit(void);
	virtual HRESULT GameMain(void);
	virtual HRESULT GameShutDown(void);

	/*
	函数名：网络处理
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void NetWorkProcess(void) {BLUEASSERT(m_pNWProcessor);m_pNWProcessor->RunOne();}

	/*
	函数名：输入（键鼠处理）
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void InputProcess(void);

	/*
	函数名：渲染
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void Render(void);

	/*
	函数名：更新窗口标题文件
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void UpdateWindowCaptionText(void);

	/*
	函数名：从外部ID获取内部ID
	功能：略
	说明：参阅CCrazyBubbleGame::m_nExternID
	参数：无
	返回值：返回-1表示不存在该用户
	*/
	int GetIDFromExternID(const int nExternID);

	/*
	函数名：获取自己在房间中的ID号
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	int GetID(void) const {BLUEASSERT(m_pNWProcessor);return m_pNWProcessor->GetInRoomID();}
	
	/*
	函数名：上（下）一页房间列表
	功能：略
	说明：一页只显示4条房间信息
	参数：无
	返回值：无
	*/
	void PrevRLPage(void);
	void NextRLPage(void);

	/*
	函数名：添加星星动画
	功能：略
	说明：无
	参数：nSrcID 从哪个ID窗口上发出
	　　　nDestID 送到哪个ID窗口上
	返回值：无
	*/
	void AddStarAni(int nSrcID, int nDestID);

	/*
	函数名：
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	virtual bool OnPreMainWndProc(HWND, UINT, WPARAM, LPARAM);

	//UI相关回调
	virtual void OnGUIPChangeUIType(int uiType);
	virtual void OnGUIPLConnectServer(const BLUEString& strIP, BLUEWORD nPort, const BLUEString& strUserName, const BLUEString& strPassWord);
	virtual void OnGUIPLPrevRLPage(void);
	virtual void OnGUIPLNextRLPage(void);
	virtual void OnGUIPLEnterRoom(int nRoomID);
	virtual void OnGUIPLPrepareGame(void);
	virtual void OnGUIPLQuitRoom(void);
	virtual void OnGUIPLHelp(void);
	virtual void OnGUIPLAbout(void);

	//网络相关回调
	virtual void OnNWPLError(const int nErrorCode);
	virtual void OnNWPLLoginServer(const BLUEString& strName);
	virtual void OnNWPLRefreshRoomListBegin(void);
	virtual void OnNWPLRoomInfo(int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum);
	virtual void OnNWPLRefreshRoomListEnd(void);
	virtual void OnNWPLEnterRoom(const int nRoomID, const int nID);
	virtual void OnNWPLQuitRoom(void);
	virtual void OnNWPLRoomUser(const BLUEString& strName, const int nID);
	virtual void OnNWPLUserEnterRoom(const BLUEString& strName, const int nID);
	virtual void OnNWPLUserLeaveRoom(const int nID);
	virtual void OnNWPLPrepareGame(const int nID);
	virtual void OnNWPLStartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate);
	virtual void OnNWPLStopGame(bool bWin);
	virtual void OnNWPLGameShootBubble(const int nID, const int nAngle);
	virtual void OnNWPLGameShootBadBubble(const int nID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameEraseBubble(const int nID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameAddLineBadBubble(const int nID);
	virtual void OnNWPLGameUnknowBubbleColor(int nID);
	virtual void OnNWPLGameThrowShit(int nID);
	virtual void OnNWPLGameAdjustToolOrder(int nID);
	virtual void OnNWPLGameToolThrowBadBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameToolEraseBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameToolThrowLineBadBubble(const int nSrcID, const int nDestID);
	virtual void OnNWPLGameToolChangeToolMode(const int nID);
	virtual void OnNWPLGameToolRemoveTool(const int nID);
	virtual void OnNWPLGameToolUnknowBubbleColor(int nSrcID, int nDestID);
	virtual void OnNWPLGameToolThrowShit(int nSrcID, int nDestID);

	//游戏机相关回调
	virtual void OnCBGMEventGameOver(void* pCallBackParam);
	virtual void OnCBGMEventShootBubble(void* pCallBackParam, const int nAngle);
	virtual void OnCBGMEventShootBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber);
	virtual void OnCBGMEventEraseBubble(void* pCallBackParam, const BLUEDWORD dwNumber);
	virtual void OnCBGMEventAddLineBadBubble(void* pCallBackParam);
	virtual void OnCBGMEventUnknowBubbleColor(void* pCallBackParam);
	virtual void OnCBGMEventThrowShit(void* pCallBackParam);
	virtual void OnCBGMEventChangeToolMode(void* pCallBackParam);
	virtual void OnCBGMEventRemoveTool(void* pCallBackParam);
	virtual void OnCBGMEventAdjustToolOrder(void* pCallBackParam);
	virtual void OnCBGMEventShootTimerOver(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForAddBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber);
	virtual void OnCBGMEventUsingToolForLineBadBubble(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForAddOneLineBadBubble(void * pCallBackParam);
	virtual void OnCBGMEventUsingToolForSubOneLineBadBubble(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForUnknowBubbleColor(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForThrowShit(void* pCallBackParam);

private:
	/*
	变量名：游戏运行状态
	功能：略
	说明：无
	*/
	GAMERUNSTATE m_state;

	/*
	函数名：当前选择的用户，使用道具的对象ID
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	int m_nSelID;

	/*
	变量名：自己的用户名
	功能：略
	说明：无
	*/
	BLUEString m_strSelfUserName;

	/*
	变量名：所有管理器
	功能：略
	说明：无
	*/
	CResManager m_resManager;//资源管理器
	CClientRoomManager m_rManager;//房间管理器
	//CClientUserManager m_uManager;//用户管理器

	/*
	变量名：所有处理器
	功能：略
	说明：无
	*/
	CCBCGUIProcessor* m_pGUIProcessor;//用户UI处理器
	CCBCAudioProcessor* m_pAudioProcessor;//音频处理器
	CCBCNetWorkProcessor*m_pNWProcessor;//网络处理器
	CCBCAnimationProcessor m_aniProcessor;//动画处理器


	/*
	变量名：游戏机集合
	功能：略
	说明：无
	*/
	GAMECOLL m_gameColl;
};
