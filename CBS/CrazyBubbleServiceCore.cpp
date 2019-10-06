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
	BLUETRACE(_BLUET("�»Ự����\n"));

	CServerUser* pNewServerUser = m_serverUserManager.CreateServerUser(pSNWSession);
	pSNWSession->SetUserKey((void*)pNewServerUser);

	//����һ��IO����ʼ��������
	CBLUENetWorkIODataPackage* pIODataPackage = m_nwIODPM.CreateLoadNWIOData();
	pSNWSession->RecvData(pIODataPackage);
	pIODataPackage->Release();
}

void CCrazyBubbleServiceCore::OnSNWSessionClose(IBLUEServerNetWorkSession* pSNWSession)
{
	BLUETRACE(_BLUET("�Ự�ر�\n"));
	DelUser(pSNWSession);
}

void CCrazyBubbleServiceCore::OnSNWSessionError(IBLUEServerNetWorkSession* pSNWSession)
{
	BLUETRACE(_BLUET("�Ự����������\n"));
	DelUser(pSNWSession);
}

void CCrazyBubbleServiceCore::OnSNWSessionRecvData(IBLUEServerNetWorkSession* pSNWSession, CBLUENetWorkIODataPackage* pSessionIOData)
{
	BLUEASSERT(pSNWSession);
	BLUEASSERT(pSessionIOData);
	CServerUser* pServerUser = (CServerUser*)pSNWSession->GetUserKey();
	BLUEASSERT(pServerUser);

	//��Ϣͷ������CBCPHEAD
	if (pSessionIOData->GetVer() != CBCPHEAD)
		return;

	//�����û���״̬��ͬ���ò�ͬ�Ĵ�����
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

	//BLUETRACE(_BLUET("�յ����ݣ�ͷ=%d������=%d������1=%d������2=%d����չ����=%d\n"),
	//	pSessionIOData->GetVer(),
	//	pSessionIOData->GetOrder(),
	//	pSessionIOData->GetFParam(),
	//	pSessionIOData->GetSParam(),
	//	pSessionIOData->GetExtendedLength());

	//����һ��IO������������
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
			//�ж��û�����iod�е�ID���ǲ����û�����ʵID��
			//�����������Ա���Ϣ
			int nID = iod.GetSParam();
			if (nID < 0 ||
				nID > serverUser.GetRoom()->GetMaxServerUserID() ||
				serverUser.GetRoom()->GetServerUser(nID) != &serverUser)
				return;

			//������������ô�����
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
	BLUETRACE(_BLUET("�û���¼�����������û���=%s������=%s\n"), strName.c_str(), strPassword.c_str());

	if (m_serverUserManager.ServerUserLogin(serverUser, strName, strPassword))
	{
		//֪ͨ�û���½�ɹ�
		serverUser.SendNetWorkDate(iod);

		//�û�״̬Ϊ����½��
		serverUser.SetState(CServerUser::login);

		//�����������ݣ����û����ܷ����б�
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
	BLUETRACE(_BLUET("�û�������뷿��ID=%d\n"), nRoomID);

	CServerGameDesk* pGameDesk = m_gameDeskManager.GetGameDesk(nRoomID);
	if (pGameDesk)
	{
		int nAddUserResult = pGameDesk->AddUser(&serverUser);
		if (nAddUserResult >= 0)//���뷿��ɹ�
		{
			//֪ͨ�û����뷿��ɹ�
			iod.SetFParam(nRoomID);
			iod.SetSParam(nAddUserResult);
			serverUser.SendNetWorkDate(iod);
		}
		else//���뷿��ʧ��
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
	BLUETRACE(_BLUET("�û������뿪����\n"));

	CServerGameDesk* pGameDesk = serverUser.GetRoom();

	//����Ϸ����ɾ���û�
	int nID = pGameDesk->DelUser(&serverUser);
	if (nID >= 0)
	{
		//֪ͨ���û����Ѿ��뿪�˷���
		serverUser.SendNetWorkDate(iod);
	}
}

void CCrazyBubbleServiceCore::ServerUserOnPerpareGame(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::room);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_PREPAREGAME);

	//����û��Ѿ�׼�����ˣ���������
	if (serverUser.IsPerpareGame())
		return;

	CServerGameDesk* pDesk = serverUser.GetRoom();
	if (pDesk->GetServerUserSafe(iod.GetFParam()) != &serverUser)
		return;

	//�����û�׼����ʼ����Ϸ
	serverUser.SetPerpareGame(true);

	//֪ͨ�����������û����û�׼���ÿ�ʼ��Ϸ��
	pDesk->SendNetWorkDate(iod);

	//���Կ�ʼ��Ϸ
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
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameShootBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_SHOOTBADBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameEraseBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_ERASEBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameUnknowBubbleColor(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_UNKNOWBUBBLECOLOR);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameThrowShit( CServerUser& serverUser, CBLUENetWorkIODataPackage& iod )
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_THROWSHIT);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameAdjustToolOrder(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_ADJUSTTOOLORDER);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameAddLineBadBubble(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_ADDLINEBADBUBBLE);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
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
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
	pDesk->SendNetWorkDate(iod, &serverUser);
}

void CCrazyBubbleServiceCore::ServerUserOnGameToolRemoveTool(CServerUser& serverUser, CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(serverUser.GetState() == CServerUser::game);
	BLUEASSERT(serverUser.GetRoom());
	BLUEASSERT(iod.GetOrder() == CPORDER_GAME);
	BLUEASSERT(iod.GetFParam() == CPORDER_GAME_TOOL_REMOVETOOL);

	CServerGameDesk* pDesk = serverUser.GetRoom();
	//�������Ѿ����˸ö��������Բ��ø��������ٷ���һ�Σ�����ͻ��˻����·��͸ö���
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
	//����û����ڷ�����Ҫ�뿪����
	BLUEASSERT(pUser);
	CServerGameDesk* pRoom = pUser->GetRoom();
	//�������ڷ�����
	if (pRoom)
	{
		//�ȼ�¼��Ϸ���Ƿ�����Ϸ�У�ɾ���û�������Ϸ���Ƿ��ܼ�����Ϸ���Ա�֪ͨ����
		//��Ϸ�����û���Ϸ������
		bool bIsInGame = (pRoom->GetState() == CServerGameDesk::ingame);

		//ɾ���û�
		pRoom->DelUser(pUser);
	}

	//ɾ���û�
	m_serverUserManager.DestroyServerUser(pUser);

	//ɾ���Ự
	pSNWSession->SetUserKey(BLUENULL);
	pSNWSession->Delete();
}