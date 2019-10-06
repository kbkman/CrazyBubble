#pragma once

#include "BLUENetWorkModelHelper.h"
#include "ServerGameDesk.h"
#include "ServerGameDeskManager.h"
#include "ServerUser.h"
#include "ServerUserManager.h"
#include "CommunicateProtocol.h"

/**
类名：疯狂泡泡龙服务核心类
功能：略
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCrazyBubbleServiceCore : public IBLUESNWMMsgProcessorCallBack
{
public:
	CCrazyBubbleServiceCore(void);
	~CCrazyBubbleServiceCore(void);

private:
	//实现IBLUESNWMMsgProcessorCallBack接口
	virtual void OnSNWSessionCreate(IBLUEServerNetWorkSession* pSNWSession);
	virtual void OnSNWSessionClose(IBLUEServerNetWorkSession* pSNWSession);
	virtual void OnSNWSessionRecvData(IBLUEServerNetWorkSession* pSNWSession, CBLUENetWorkIODataPackage* pSessionIOData);
	virtual void OnSNWSessionError(IBLUEServerNetWorkSession* pSNWSession);

private:
	void ServerUserRecvDataProcess_InConnectState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserRecvDataProcess_InLoginState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserRecvDataProcess_InRoomState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserRecvDataProcess_InGameState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserRecvDataProcess_InDeathState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);

	void ServerUserOnLogin(CServerUser& serverUser, const BLUEString& strName, const BLUEString& strPassword, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnRoomList(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnEnterRoom(CServerUser& serverUser, int nRoomID, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnQuitRoom(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnPerpareGame(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameOver(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameShootBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameShootBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameEraseBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameAddLineBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameUnknowBubbleColor(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameThrowShit(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameAdjustToolOrder(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolThrowBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolEraseBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolThrowLineBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolChangeToolMode(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolRemoveTool(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolUnknowBubbleColor(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);
	void ServerUserOnGameToolThrowShit(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod);

private:
	void DelUser(IBLUEServerNetWorkSession* pSNWSession);

private:
	/*
	变量名：IO包管理器
	功能：略
	说明：无
	*/
	CBLUENetWorkIODataPackageManager m_nwIODPM;

	/*
	变量名：服务器用户管理器
	功能：略
	说明：无
	*/
	CServerUserManager m_serverUserManager;

	/*
	变量名：服务器游戏桌管理器
	功能：略
	说明：无
	*/
	CServerGameDeskManager m_gameDeskManager;

};
