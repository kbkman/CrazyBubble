#pragma once

#include "BLUEAdv.h"
#include "BLUEClientNetWorkModel.h"
#include "BLUENetWorkModelHelper.h"

/**
接口名：疯狂泡泡龙客户端网络处理器监听者接口
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class ICBCNetWorkProcessorListener
{
public:
	virtual void OnNWPLError(const int nErrorCode) = 0;//出错信息
	virtual void OnNWPLLoginServer(const BLUEString& strName) = 0;//登陆到服务器
	virtual void OnNWPLRefreshRoomListBegin(void) = 0;//开始刷新房间列表
	virtual void OnNWPLRoomInfo(int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum) = 0;//房间信息
	virtual void OnNWPLRefreshRoomListEnd(void) = 0;//结束刷新房间列表
	virtual void OnNWPLEnterRoom(const int nRoomID, const int nID) = 0;//进入了ID为nRoomID的房间，自己的ID号为nID
	virtual void OnNWPLQuitRoom(void) = 0;//离开了房间
	virtual void OnNWPLRoomUser(const BLUEString& strName, const int nID) = 0;//房间内已存在的用户
	virtual void OnNWPLUserEnterRoom(const BLUEString& strName, const int nID) = 0;//有用户进入了房间，他的用户名为strName，在房间里的ID号为nID
	virtual void OnNWPLUserLeaveRoom(const int nID) = 0;//有用户离开了房间，在房间里的ID号为nID
	virtual void OnNWPLPrepareGame(const int nID) = 0;//有用户准备好开始游戏了，nID为该用户的ID号
	virtual void OnNWPLStartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate) = 0;//游戏开始，nRandom为游戏初始的随机数种子
	virtual void OnNWPLStopGame(bool bWin) = 0;//游戏结束,bWin表示是否胜利
	virtual void OnNWPLGameShootBubble(const int nID, const int nAngle) = 0;//ID号为nID的用户发射了一个泡泡，该泡泡角度为nAngle
	virtual void OnNWPLGameShootBadBubble(const int nID, const BLUEDWORD dwNumber) = 0;//ID号为nID的用户发射了坏泡泡
	virtual void OnNWPLGameEraseBubble(const int nID, const BLUEDWORD dwNumber) = 0;//ID号为nID的用户消除了泡泡
	virtual void OnNWPLGameAddLineBadBubble(const int nID) = 0;//ID号为nID的用户添加两行坏泡泡
	virtual void OnNWPLGameUnknowBubbleColor(int nID) = 0;//ID号为nID的用户隐藏泡泡颜色
	virtual void OnNWPLGameThrowShit(int nID) = 0;//ID号为nID的用户受扔便便
	virtual void OnNWPLGameAdjustToolOrder(int nID) = 0;//ID号为nID的用户切换道具顺序
	virtual void OnNWPLGameToolThrowBadBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber) = 0;//ID号nSrcID的用户向ID号为nDestID的用户扔了dwNumber个泡泡
	virtual void OnNWPLGameToolEraseBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber) = 0;//ID号nSrcID的用户帮ID号为nDestID的用户擦除了dwNumber个泡泡
	virtual void OnNWPLGameToolThrowLineBadBubble(const int nSrcID, const int nDestID) = 0;//ID号nSrcID的用户向ID号为nDestID的用户扔了两行泡泡
	virtual void OnNWPLGameToolChangeToolMode(const int nID) = 0;//ID号为nID的用户更换了道具模式
	virtual void OnNWPLGameToolRemoveTool(const int nID) = 0;//ID号为nID的用户移除了道具（即使用了道具）
	virtual void OnNWPLGameToolUnknowBubbleColor(int nSrcID, int nDestID) = 0;//ID号nSrcID的用户向ID号为nDestID的用户隐藏泡泡
	virtual void OnNWPLGameToolThrowShit(int nSrcID, int nDestID) = 0;//ID号nSrcID的用户向ID号为nDestID的用户扔了便便
};

//网络出错列表
#define NWP_ERROR_TYPE_GAMESTART          1      //游戏已经开始
#define NWP_ERROR_TYPE_HASINROOM         2      //已经进入了该房间
#define NWP_ERROR_TYPE_ROOMMAXUSER    3      //房间客满

/**
类名：疯狂泡泡龙客户端网络处理器
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCNetWorkProcessor : private IBLUECNWMMsgProcessorCallBack
{
public:
	/*
	枚举名：返回枚举
	功能：略
	说明：无
	*/
	enum CBCNWPRESULT
	{
		noerror,                      //无出错
		nwiniterror,                //网络初始化失败
		connecterror,             //连接失败
		connectederror,         //已经连接
		othererror                  //其他错误
	};

	/*
	枚举名：网络状态
	功能：略
	说明：无
	*/
	enum NETSTATE
	{
		disconnect,             //未连接
		connect,                 //已连接
		login,                      //已登陆
		room,                     //在房间内
		game                     //在游戏中
	};

public:
	CCBCNetWorkProcessor(ICBCNetWorkProcessorListener* pListener);
	~CCBCNetWorkProcessor(void);

public:
	/*
	函数名：获取当前网络状态
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	NETSTATE GetState(void) const {return m_state;}

	/*
	函数名：获取自己在房间内的ID号
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	int GetInRoomID(void) const {return m_nInRoomID;}

	/*
	变量名：处理器运行一次
	功能：略
	说明：该函数用于游戏中轮询
	*/
	void RunOne(void);

	/*
	函数名：连接服务器
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CBCNWPRESULT Connect(const BLUEStringA& strIP, const BLUEWORD nConnectPort);

	/*
	函数名：关闭连接
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void Close(void);

public:
	void LoginServer(const BLUEString& strName, const BLUEString& strPassword);//登陆
	void RoomList(void);//请求房间列表信息
	void EnterRoom(const int nRoomID);//进房
	void QuitRoom(void);//离开当前房间
	void PrepareGame(void);//准备好开始游戏
	void GameOver(void);//游戏信息：游戏失败
	void GameShootBubble(const int nAngle);//游戏信息：发射泡泡
	void GameShootBadBubble(const BLUEDWORD dwNumber);//游戏信息：发射坏泡泡
	void GameEraseBubble(const BLUEDWORD dwNumber);//游戏信息：消除泡泡
	void GameAddLineBadBubble(void);//游戏信息：增加两行泡坏坏
	void GameUnknowBubbleColor(void);//游戏信息：隐藏泡泡颜色
	void GameThrowShit(void);//游戏信息：受到扔便便
	void GameAdjustToolOrder(void);//游戏信息：切换道具顺序
	void GameToolThrowBadBubble(const int nID, const BLUEDWORD dwNumber);//游戏道具信息：向某用户扔坏泡泡，数量为dwNumber
	void GameToolEraseBubble(const int nID, const BLUEDWORD dwNumber);//游戏道具信息：替某用户擦除泡泡，数量为dwNumber
	void GameToolThrowLineBadBubble(const int nID);//游戏道具信息：向某用户扔两行坏泡泡
	void GameToolChangeToolMode(void);//游戏道具信息：切换道具模式（泡泡模式\道具模式）
	void GameToolRemoveTool(void);//游戏道具信息：移除道具（用户使用了道具）
	void GameToolUnknowBubbleColor(int nID);//游戏道具信息：隐藏泡泡颜色
	void GameToolThrowShit(int nID);//游戏道具信息：向某用户扔便便

private:
	virtual void OnCNWRecvData(CBLUENetWorkIODataPackage& iod);
	virtual void OnCNWError(int nErrorCode);

private:
	/*
	变量名：网络状态
	功能：略
	说明：无
	*/
	NETSTATE m_state;

	/*
	变量名：所在房间的ID号
	功能：略
	说明：无
	*/
	int m_nRoomID;

	/*
	变量名：在房间里的ID号
	功能：略
	说明：无
	*/
	int m_nInRoomID;

	/*
	变量名：监听者对象
	功能：略
	说明：无
	*/
	ICBCNetWorkProcessorListener* m_pListener;

	/*
	变量名：客户端网络模型
	功能：略
	说明：无
	*/
	IBLUEClientNetWorkModel* m_pCNWM;

	/*
	变量名：安全消息队列
	功能：略
	说明：无
	*/
	IBLUEMTSafeMessageQueue* m_pMSMQ;

	/*
	变量名：网络IO包管理器
	功能：略
	说明：无
	*/
	CBLUENetWorkIODataPackageManager m_nwIODPM;

};
