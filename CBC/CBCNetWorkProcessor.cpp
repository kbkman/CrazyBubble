#include "StdAfx.h"
#include "CBCNetWorkProcessor.h"
#include "../Common/CommunicateProtocol.h"

//根据不同版本加载不同库

#define CBCNW_MSMQ_MSG_TYPE               2    //消息队列使用的消息类型


CCBCNetWorkProcessor::CCBCNetWorkProcessor(ICBCNetWorkProcessorListener* pListener) :
m_state(disconnect),
m_nRoomID(-1),
m_nInRoomID(-1),
m_pMSMQ(BLUENULL),
m_pCNWM(BLUENULL),
m_pListener(pListener),
m_nwIODPM(CBCPHEAD)
{
	BLUEASSERT(m_pListener);
}

CCBCNetWorkProcessor::~CCBCNetWorkProcessor(void)
{
	Close();
}

void CCBCNetWorkProcessor::RunOne(void)
{
	if (m_pMSMQ == BLUENULL)
		return;

	BLUEASSERT(m_pCNWM);

	BLUEDWORD dwMsgType;
	BLUEDWORD fp;
	void* sp;
	void* tp;

	if (m_pMSMQ->PeekOneMessage(dwMsgType, fp, sp, tp))
	{
		BLUEASSERT(dwMsgType == CBCNW_MSMQ_MSG_TYPE);
		::BLUEProcessCNWMMsg(m_pCNWM, fp, sp, tp, this);
	}
}

CCBCNetWorkProcessor::CBCNWPRESULT CCBCNetWorkProcessor::Connect(const BLUEStringA& strIP, const BLUEWORD nConnectPort)
{
	if (m_pMSMQ)
		return connectederror;

	m_pMSMQ = ::BLUEAdvCreateMSMQ();
	m_pCNWM = ::BLUECreateCNWM();

	BLUEASSERT(m_pMSMQ);
	BLUEASSERT(m_pCNWM);

	switch (m_pCNWM->Create(m_pMSMQ, CBCNW_MSMQ_MSG_TYPE, strIP.c_str(), nConnectPort))
	{
	case CNWMRESULT_OK:
		{
			//连接成功，状态变为已连接
			m_state = connect;

			//开始接受数据
			CBLUENetWorkIODataPackage* pIOD = m_nwIODPM.CreateLoadNWIOData();
			m_pCNWM->RecvData(pIOD);
			pIOD->Release();
		}
		return noerror;

	case CNWMRESULT_E_CREATED:
		Close();
		return connectederror;

	case CNWMRESULT_E_CONNECT:
		Close();
		return connecterror;

	case CNWMRESULT_E_NETWORKINIT:
		Close();
		return nwiniterror;

	default:
		Close();
		return othererror;
	}

}

void CCBCNetWorkProcessor::Close(void)
{
	if (m_pCNWM)
	{
		m_pCNWM->Destroy();
		::BLUEDestroyCNWM(m_pCNWM);
		m_pCNWM = BLUENULL;
	}

	if (m_pMSMQ)
	{
		::BLUEAdvDestroyMSMQ(m_pMSMQ);
		m_pMSMQ = BLUENULL;
	}
}

void CCBCNetWorkProcessor::OnCNWRecvData(CBLUENetWorkIODataPackage& iod)
{
	BLUEASSERT(m_pListener);

	//检查信息头
	if (iod.GetVer() != CBCPHEAD)
		return;

	//检查命令
	switch(iod.GetOrder())
	{
	case CPORDER_GAME:
		switch (iod.GetFParam())
		{
		case CPORDER_GAME_SHOOTBUBBLE:
			{
				int nAngle;//发射泡泡的角度
				iod >> nAngle;
				m_pListener->OnNWPLGameShootBubble(iod.GetSParam(), nAngle);
			}
			break;
		case CPORDER_GAME_SHOOTBADBUBBLE:
			{
				int nNumber;
				iod >> nNumber;
				m_pListener->OnNWPLGameShootBadBubble(iod.GetSParam(), (BLUEDWORD)nNumber);
			}
			break;
		case CPORDER_GAME_ERASEBUBBLE:
			{
				int nNumber;
				iod >> nNumber;
				m_pListener->OnNWPLGameEraseBubble(iod.GetSParam(), (BLUEDWORD)nNumber);
			}
			break;
		case CPORDER_GAME_ADDLINEBADBUBBLE:
			m_pListener->OnNWPLGameAddLineBadBubble(iod.GetSParam());
			break;
		case CPORDER_GAME_UNKNOWBUBBLECOLOR:
			m_pListener->OnNWPLGameUnknowBubbleColor(iod.GetSParam());
			break;
		case CPORDER_GAME_THROWSHIT:
			m_pListener->OnNWPLGameThrowShit(iod.GetSParam());
			break;
		case CPORDER_GAME_ADJUSTTOOLORDER:
			m_pListener->OnNWPLGameAdjustToolOrder(iod.GetSParam());
			break;
		case CPORDER_GAME_TOOL_THROWBADBUBBLE:
			{
				int nSrcID, nDestID;
				BLUEDWORD dwNumber;
				nSrcID = iod.GetSParam();
				iod >> nDestID >> (int&)dwNumber;
				m_pListener->OnNWPLGameToolThrowBadBubble(nSrcID, nDestID, dwNumber);
			}
			break;
		case CPORDER_GAME_TOOL_ERASEBUBBLE:
			{
				int nSrcID, nDestID;
				BLUEDWORD dwNumber;
				nSrcID = iod.GetSParam();
				iod >> nDestID >> (int&)dwNumber;
				m_pListener->OnNWPLGameToolEraseBubble(nSrcID, nDestID, dwNumber);
			}
			break;
		case CPORDER_GAME_TOOL_THROWLINEBADBUBBLE:
			{
				int nSrcID, nDestID;
				nSrcID = iod.GetSParam();
				iod >> nDestID;
				m_pListener->OnNWPLGameToolThrowLineBadBubble(nSrcID, nDestID);
			}
			break;
		case CPORDER_GAME_TOOL_CHANGETOOLMODE:
			m_pListener->OnNWPLGameToolChangeToolMode(iod.GetSParam());
			break;
		case CPORDER_GAME_TOOL_REMOVETOOL:
			m_pListener->OnNWPLGameToolRemoveTool(iod.GetSParam());
			break;
		case CPORDER_GAME_TOOL_UNKNOWBUBBLECOLOR:
			{
				int nSrcID, nDestID;
				nSrcID = iod.GetSParam();
				iod >> nDestID;
				m_pListener->OnNWPLGameToolUnknowBubbleColor(nSrcID, nDestID);
			}
			break;
		case CPORDER_GAME_TOOL_THROWSHIT:
			{
				int nSrcID, nDestID;
				nSrcID = iod.GetSParam();
				iod >> nDestID;
				m_pListener->OnNWPLGameToolThrowShit(nSrcID, nDestID);
			}
			break;
		}
		break;

	case CPORDER_ERROR:
		{
			switch (iod.GetFParam())
			{
			case CPORDER_ERROR_TYPE_GAMESTART:
				m_pListener->OnNWPLError(NWP_ERROR_TYPE_GAMESTART);
				break;
			case CPORDER_ERROR_TYPE_HASINROOM:
				m_pListener->OnNWPLError(NWP_ERROR_TYPE_HASINROOM);
				break;
			case CPORDER_ERROR_TYPE_ROOMMAXUSER:
				m_pListener->OnNWPLError(NWP_ERROR_TYPE_ROOMMAXUSER);
				break;
			}
		}
		break;

	case CPORDER_LOGIN:
		{
			m_state = login;
			BLUEString strName;
			iod >> strName;
			m_pListener->OnNWPLLoginServer(strName);
		}
		break;

	case CPORDER_RLBEGIN:
		m_pListener->OnNWPLRefreshRoomListBegin();
		break;

	case CPORDER_ROOMINFO:
		{
			BLUEString strName;
			int nMaxClientNum;
			iod >> nMaxClientNum >> strName;
			m_pListener->OnNWPLRoomInfo(
				iod.GetFParam(),
				strName,
				iod.GetSParam(),
				nMaxClientNum
				);
		}
		break;

	case CPORDER_RLEND:
		m_pListener->OnNWPLRefreshRoomListEnd();
		break;

	case CPORDER_ENTERROOM:
		m_state = room;
		m_nRoomID = iod.GetFParam();
		m_nInRoomID = iod.GetSParam();
		m_pListener->OnNWPLEnterRoom(m_nRoomID, m_nInRoomID);
		break;

	case CPORDER_QUITROOM:
		m_state = login;
		m_pListener->OnNWPLQuitRoom();
		m_nInRoomID = -1;
		m_nRoomID = -1;
		break;

	case CPORDER_ROOMUSER:
		{
			BLUEString strName;
			iod >> strName;
			m_pListener->OnNWPLRoomUser(strName, iod.GetFParam());
		}
		break;

	case CPORDER_USERENTERRROOM:
		{
			BLUEString strName;
			iod >> strName;
			m_pListener->OnNWPLUserEnterRoom(strName, iod.GetFParam());
		}
		break;

	case CPORDER_USERLEAVEROOM:
		m_pListener->OnNWPLUserLeaveRoom(iod.GetFParam());
		break;

	case CPORDER_PREPAREGAME:
		m_pListener->OnNWPLPrepareGame(iod.GetFParam());
		break;

	case CPORDER_STARTGAME:
		{
			BLUEDWORD dwInitRows;
			m_state = game;
			iod >> dwInitRows;
			m_pListener->OnNWPLStartGame(iod.GetFParam(), dwInitRows, iod.GetSParam());
		}
		break;

	case CPORDER_STOPGAME:
		m_pListener->OnNWPLStopGame(iod.GetFParam() == 0);
		m_state = room;
		break;
	}

	//继续接收新数据
	CBLUENetWorkIODataPackage* pIOD = m_nwIODPM.CreateLoadNWIOData();
	m_pCNWM->RecvData(pIOD);
	pIOD->Release();
}

void CCBCNetWorkProcessor::OnCNWError(int nErrorCode)
{

}

void CCBCNetWorkProcessor::LoginServer(const BLUEString& strName, const BLUEString& strPassword)
{
	if (m_pCNWM == BLUENULL) return;

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetOrder(CPORDER_LOGIN);
	(*pIOP) << strName << strPassword;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::RoomList(void)
{
	if (m_pCNWM == BLUENULL) return;

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetOrder(CPORDER_ROOMLIST);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::EnterRoom(const int nRoomID)
{
	if (m_pCNWM == BLUENULL) return;

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_ENTERROOM, nRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::QuitRoom(void)
{
	if (m_pCNWM == BLUENULL) return;

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetOrder(CPORDER_QUITROOM);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::PrepareGame(void)
{
	if (m_pCNWM == BLUENULL || m_state != room) return;

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_PREPAREGAME, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameOver(void)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_OVER, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameShootBubble(const int nAngle)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_SHOOTBUBBLE, m_nInRoomID);
	(*pIOP) << nAngle;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameShootBadBubble(BLUEDWORD dwNumber)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_SHOOTBADBUBBLE, m_nInRoomID);
	(*pIOP) << dwNumber;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameEraseBubble( const BLUEDWORD dwNumber )
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_ERASEBUBBLE, m_nInRoomID);
	(*pIOP) << dwNumber;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameAddLineBadBubble( void )
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_ADDLINEBADBUBBLE, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameUnknowBubbleColor(void)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_UNKNOWBUBBLECOLOR, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameThrowShit(void)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_THROWSHIT, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameAdjustToolOrder(void)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_ADJUSTTOOLORDER, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameToolThrowBadBubble(const int nID, const BLUEDWORD dwNumber)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_THROWBADBUBBLE, m_nInRoomID);
	(*pIOP) << nID << (int)dwNumber;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();

}

void CCBCNetWorkProcessor::GameToolEraseBubble(const int nID, const BLUEDWORD dwNumber)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_ERASEBUBBLE, m_nInRoomID);
	(*pIOP) << nID << (int)dwNumber;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();

}

void CCBCNetWorkProcessor::GameToolThrowLineBadBubble(const int nID)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_THROWLINEBADBUBBLE, m_nInRoomID);
	(*pIOP) << nID;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameToolChangeToolMode(void)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_CHANGETOOLMODE, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameToolRemoveTool( void )
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_REMOVETOOL, m_nInRoomID);
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameToolUnknowBubbleColor(int nID)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_UNKNOWBUBBLECOLOR, m_nInRoomID);
	(*pIOP) << nID;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}

void CCBCNetWorkProcessor::GameToolThrowShit(int nID)
{
	if (m_pCNWM == BLUENULL) return;
	BLUEASSERT(m_state == game);

	CBLUENetWorkIODataPackage* pIOP = m_nwIODPM.CreateStoreNWIOData();
	pIOP->SetBaseData(CPORDER_GAME, CPORDER_GAME_TOOL_THROWSHIT, m_nInRoomID);
	(*pIOP) << nID;
	m_pCNWM->SendData(pIOP);
	pIOP->Release();
}