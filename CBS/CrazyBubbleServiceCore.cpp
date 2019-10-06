#include "StdAfx.h"
#include "ToolKit.h"
#include "CrazyBubbleServiceCore.h"

CCrazyBubbleServiceCore::CCrazyBubbleServiceCore(void) :
m_nwIODPM(CBCPHEAD),
m_gameDeskManager(m_nwIODPM)
{
}

CCrazyBubbleServiceCore::~CCrazyBubbleServiceCore(void)
{
}

void CCrazyBubbleServiceCore::OnSNWSessionCreate(IBLUEServerNetWorkSession* pSNWSession)
{
	BLUEASSERT(pSNWSession);
	BLUETRACE(_BLUET("新会话创建\n"));

	CServerUser* pNewServerUser = m_serverUserManager.CreateServerUser(pSNWSession);
	pSNWSession->SetUserKey((void*)pNewServerUser);

	//创建一个IO包开始接受数据
	CBLUENetWorkIODataPackage* pIODataPackage = m_nwIODPM.CreateLoadNWIOData();
	pSNWSession->RecvData(pIODataPackage);
	pIODataPackage->Release();
}

void CCrazyBubbleServiceCore::OnSNWSessionClose(IBLUEServerNetWorkSession* pSNWSession)
{
	BLUETRACE(_BLUET("会话关闭\n"));
	DelUser(pSNWSession);
}

void CCrazyBubbleServiceCore::OnSNWSessionError(IBLUEServerNetWorkSession* pSNWSession)
{
	BLUETRACE(_BLUET("会话非正常结束\n"));
	DelUser(pSNWSession);
}

void CCrazyBubbleServiceCore::OnSNWSessionRecvData(IBLUEServerNetWorkSession* pSNWSession, CBLUENetWorkIODataPackage* pSessionIOData)
{
	BLUEASSERT(pSNWSession);
	BLUEASSERT(pSessionIOData);
	CServerUser* pServerUser = (CServerUser*)pSNWSession->GetUserKey();
	BLUEASSERT(pServerUser);

	//信息头必须是CBCPHEAD
	if (pSessionIOData->GetVer() != CBCPHEAD)
		return;

	//根据用户的状态不同调用不同的处理函数
	switch (pServerUser->GetState())
	{
	case CServerUser::connect:
		ServerUserRecvDataProcess_InConnectState(*pServerUser, *pSessionIOData);
		break;

	case CServerUser::login:
		ServerUserRecvDataProcess_InLoginState(*pServerUser, *pSessionIOData);
		break;

	case CServerUser::room:
		ServerUserRecvDataProcess_InRoomState(*pServerUser, *pSessionIOData);
		break;

	case CServerUser::game:
		ServerUserRecvDataProcess_InGameState(*pServerUser, *pSessionIOData);
		break;

	case CServerUser::death:
		ServerUserRecvDataProcess_InDeathState(*pServerUser, *pSessionIOData);
		break;

	default:
		BLUEASSERT(0);
	}

	//BLUETRACE(_BLUET("收到数据，头=%d，命令=%d，参数1=%d，参数2=%d，扩展长度=%d\n"),
	//	pSessionIOData->GetVer(),
	//	pSessionIOData->GetOrder(),
	//	pSessionIOData->GetFParam(),
	//	pSessionIOData->GetSParam(),
	//	pSessionIOData->GetExtendedLength());

	//创建一个IO包，继续接收
	CBLUENetWorkIODataPackage* pIODataPackage = m_nwIODPM.CreateLoadNWIOData();
	pSNWSession->RecvData(pIODataPackage);
	pIODataPackage->Release();

}

void CCrazyBubbleServiceCore::ServerUserRecvDataProcess_InConnectState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::connect);

	switch (iod.GetOrder())
	{
	case CPORDER_LOGIN:
		{
			BLUEString strName, strPassword;
			iod >> strName >> strPassword;
			ServerUserOnLogin(serverUser, strName, strPassword, iod);
		}
		break;
	}


}

void CCrazyBubbleServiceCore::ServerUserRecvDataProcess_InLoginState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::login);

	switch (iod.GetOrder())
	{
	case CPORDER_ROOMLIST:
		ServerUserOnRoomList(serverUser, iod);
		break;
	case CPORDER_ENTERROOM:
		ServerUserOnEnterRoom(serverUser, iod.GetFParam(), iod);
		break;

	}
}

void CCrazyBubbleServiceCore::ServerUserRecvDataProcess_InRoomState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::room);

	switch (iod.GetOrder())
	{
	case CPORDER_PREPAREGAME:
		ServerUserOnPerpareGame(serverUser, iod);
		break;
	case CPORDER_QUITROOM:
		ServerUserOnQuitRoom(serverUser, iod);
		break;
	}
}

void CCrazyBubbleServiceCore::ServerUserRecvDataProcess_InGameState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);

	switch (iod.GetOrder())
	{
	case CPORDER_GAME:
		{
			//判断用户传在iod中的ID号是不是用户的真实ID号
			//如果不是则忽略本消息
			int nID = iod.GetSParam();
			if (nID < 0 ||
				nID > serverUser.GetRoom()->GetMaxServerUserID() ||
				serverUser.GetRoom()->GetServerUser(nID) != &serverUser)
				return;

			//根据子命令调用处理函数
			switch (iod.GetFParam())
			{
			case CPORDER_GAME_OVER:
				ServerUserOnGameOver(serverUser, iod);
				break;
			case CPORDER_GAME_SHOOTBUBBLE:
				ServerUserOnGameShootBubble(serverUser, iod);
				break;
			case CPORDER_GAME_SHOOTBADBUBBLE:
				ServerUserOnGameShootBadBubble(serverUser, iod);
				break;
			case CPORDER_GAME_ERASEBUBBLE:
				ServerUserOnGameEraseBubble(serverUser, iod);
				break;
			case CPORDER_GAME_ADDLINEBADBUBBLE:
				ServerUserOnGameAddLineBadBubble(serverUser, iod);
				break;
			case CPORDER_GAME_UNKNOWBUBBLECOLOR:
				ServerUserOnGameUnknowBubbleColor(serverUser, iod);
				break;
			case CPORDER_GAME_THROWSHIT:
				ServerUserOnGameThrowShit(serverUser, iod);
				break;
			case CPORDER_GAME_ADJUSTTOOLORDER:
				ServerUserOnGameAdjustToolOrder(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_THROWBADBUBBLE:
				ServerUserOnGameToolThrowBadBubble(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_ERASEBUBBLE:
				ServerUserOnGameToolEraseBubble(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_THROWLINEBADBUBBLE:
				ServerUserOnGameToolThrowLineBadBubble(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_CHANGETOOLMODE:
				ServerUserOnGameToolChangeToolMode(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_REMOVETOOL:
				ServerUserOnGameToolRemoveTool(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_UNKNOWBUBBLECOLOR:
				ServerUserOnGameToolUnknowBubbleColor(serverUser, iod);
				break;
			case CPORDER_GAME_TOOL_THROWSHIT:
				ServerUserOnGameToolThrowShit(serverUser, iod);
				break;
			}
		}
		break;

	case CPORDER_QUITROOM:
		ServerUserOnQuitRoom(serverUser, iod);
		break;
	}
}

void CCrazyBubbleServiceCore::ServerUserRecvDataProcess_InDeathState(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::death);

	switch (iod.GetOrder())
	{
	case CPORDER_LOGIN:
		break;
	}

}

void CCrazyBubbleServiceCore::ServerUserOnLogin(
	CServerUser& serverUser,
	const BLUEString& strName,
	const BLUEString& strPassword, 
	CBLUENetWorkIODataPackage& iod
	)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::connect);
	BLUEASSERT(iod.GetOrder() == CPORDER_LOGIN);
	BLUETRACE(_BLUET("用户登录到服务器，用户名=%s，密码=%s\n"), strName.c_str(), strPassword.c_str());

	if (m_serverUserManager.ServerUserLogin(serverUser, strName, strPassword))
	{
		//通知用户登陆成功
		serverUser.SendNetWorkDate(iod);

		//用户状态为“登陆”
		serverUser.SetState(CServerUser::login);

		//发送网络数据，让用户接受房间列表
		m_gameDeskManager.SendRLToUser(serverUser);
	}
}

void CCrazyBubbleServiceCore::ServerUserOnRoomList(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::login);
	BLUEASSERT(iod.GetOrder() == CPORDER_ROOMLIST);
	m_gameDeskManager.SendRLToUser(serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnEnterRoom(CServerUser& serverUser, int nRoomID, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::login);
	BLUEASSERT(serverUser.GetRoom() == BLUENULL);
	BLUEASSERT(iod.GetOrder() == CPORDER_ENTERROOM);
	BLUETRACE(_BLUET("用户请求进入房间ID=%d\n"), nRoomID);

	CServerGameDesk* pGameDesk = m_gameDeskManager.GetGameDesk(nRoomID);
	if (pGameDesk)
	{
		int nAddUserResult = pGameDesk->AddUser(&serverUser);
		if (nAddUserResult >= 0)//进入房间成功
		{
			//通知用户进入房间成功
			iod.SetFParam(nRoomID);
			iod.SetSParam(nAddUserResult);
			serverUser.SendNetWorkDate(iod);
		}
		else//进入房间失败
		{
			CBLUENetWorkIODataPackage* pIOD;
			pIOD = m_nwIODPM.CreateStoreNWIOData();
			pIOD->SetOrder(CPORDER_ERROR);
			switch (nAddUserResult)
			{
			case SGD_ERROR_INGAME:
				pIOD->SetFParam(CPORDER_ERROR_TYPE_GAMESTART);
				break;
			case SGD_ERROR_HASUSER:
				pIOD->SetFParam(CPORDER_ERROR_TYPE_HASINROOM);
				break;
			case SGD_ERROR_MAXUSER:
				pIOD->SetFParam(CPORDER_ERROR_TYPE_ROOMMAXUSER);
				break;
			}
			serverUser.SendNetWorkDate(*pIOD);
			pIOD->Release();
		}
	}
}

void CCrazyBubbleServiceCore::ServerUserOnQuitRoom(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::room || serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_QUITROOM);
	BLUETRACE(_BLUET("用户请求离开房间\n"));

	CServerGameDesk* pGameDesk = serverUser.GetRoom();

	//从游戏桌上删除用户
	int nID = pGameDesk->DelUser(&serverUser);
	if (nID >= 0)
	{
		//通知该用户你已经离开了房间
		serverUser.SendNetWorkDate(iod);
	}
}

void CCrazyBubbleServiceCore::ServerUserOnPerpareGame(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::room);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_PREPAREGAME);

	//如果用户已经准备过了，则不作处理
	if (serverUser.IsPerpareGame())
		return;

	CServerGameDesk* pDesk = serverUser.GetRoom();
	if (pDesk->GetServerUserSafe(iod.GetFParam()) != &serverUser)
		return;

	//设置用户准备开始了游戏
	serverUser.SetPerpareGame(true);

	//通知房间内所有用户该用户准备好开始游戏了
	pDesk->SendNetWorkDate(iod);

	//尝试开始游戏
	pDesk->StartGame();
}

void CCrazyBubbleServiceCore::ServerUserOnGameOver(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_OVER);

	serverUser.SetGameOver(true);
	serverUser.GetRoom()->TestOverGame();
}

void CCrazyBubbleServiceCore::ServerUserOnGameShootBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_SHOOTBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameShootBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_SHOOTBADBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameEraseBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_ERASEBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameUnknowBubbleColor(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_UNKNOWBUBBLECOLOR);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameThrowShit( CServerUser& serverUser, CBLUENetWorkIODataPackage& iod )
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_THROWSHIT);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameAdjustToolOrder(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_ADJUSTTOOLORDER);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameAddLineBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_ADDLINEBADBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolThrowBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_THROWBADBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	pDesk->SendNetWorkDate(iod);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolEraseBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_ERASEBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	pDesk->SendNetWorkDate(iod);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolThrowLineBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_THROWLINEBADBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	pDesk->SendNetWorkDate(iod);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolChangeToolMode(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_CHANGETOOLMODE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolRemoveTool(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_REMOVETOOL);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//发送者已经做了该动作，所以不用给发送者再发送一次，否则客户端会重新发送该动作
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolUnknowBubbleColor(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_UNKNOWBUBBLECOLOR);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	pDesk->SendNetWorkDate(iod);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolThrowShit(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_THROWSHIT);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	pDesk->SendNetWorkDate(iod);
}

void CCrazyBubbleServiceCore::DelUser(IBLUEServerNetWorkSession* pSNWSession)
{
	BLUEASSERT(pSNWSession);

	CServerUser* pUser = (CServerUser*)pSNWSession->GetUserKey();
	//如果用户还在房间内要离开房间
	BLUEASSERT(pUser);
	CServerGameDesk* pRoom = pUser->GetRoom();
	//如果玩家在房间内
	if (pRoom)
	{
		//先记录游戏桌是否在游戏中，删除用户后检测游戏桌是否还能继续游戏，以便通知所有
		//游戏桌内用户游戏结束了
		bool bIsInGame = (pRoom->GetState() == CServerGameDesk::ingame);

		//删除用户
		pRoom->DelUser(pUser);
	}

	//删除用户
	m_serverUserManager.DestroyServerUser(pUser);

	//删除会话
	pSNWSession->SetUserKey(BLUENULL);
	pSNWSession->Delete();
}