#include "StdAfx.h"
#include "CommunicateProtocol.h"
#include "ToolKit.h"
#include "ServerGameDesk.h"

CServerGameDesk::CServerGameDesk(
								 CBLUENetWorkIODataPackageManager& nwIODPM,
								 const BLUEString& strName,
								 BLUEDWORD dwInitRows,
								 int nBringToolRate) :
m_nwIODPM(nwIODPM),
m_state(normal),
m_dwInitRows(dwInitRows),
m_nBringToolRate(nBringToolRate)
{
	SetName(strName);
	m_nMaxClientNum = 6;

	//每桌人数固定，所有先定好缓冲大小
	m_userColl.reserve(DESK_MAX_USERNUMBER);
	m_userColl.assign(DESK_MAX_USERNUMBER, (CServerUser*)BLUENULL);
}

CServerGameDesk::~CServerGameDesk(void)
{
}

bool CServerGameDesk::HasUser(const CServerUser* pUser)
{
	BLUEASSERT(pUser);
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i] == pUser)
			return true;

	return false;
}

int CServerGameDesk::GetUserNumber(void) const
{
	int nNumber = 0;
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i])
			nNumber++;

	return nNumber;
}

int CServerGameDesk::AddUser(CServerUser* pUser)
{
	BLUEASSERT(pUser);

	//如果已经在游戏中，则不能进入
	if (m_state == ingame)
		return SGD_ERROR_INGAME;

	//如果已经在房间里，则不能重复进入
	if (HasUser(pUser))
		return SGD_ERROR_HASUSER;

	//查看是否有空位给用户坐入
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
	{
		if (m_userColl[i] == BLUENULL)
		{
			//设置用户在房间内数据
			m_userColl[i] = pUser;
			pUser->SetState(CServerUser::room);
			pUser->m_pRoom = this;
			pUser->m_bIsGameOver = false;//初始时游戏为非失败状态

			//通知用户房间内所有用户
			CBLUENetWorkIODataPackage* pIOD;
			for (ROOMSERVERUSERCOLL::size_type j = 0; j < m_userColl.size(); j++)
			{
				if (m_userColl[j])
				{
					pIOD = m_nwIODPM.CreateStoreNWIOData();
					pIOD->SetBaseData(CPORDER_ROOMUSER, (int)j);
					(*pIOD) << m_userColl[j]->GetName();
					pUser->SendNetWorkDate(*pIOD);
					pIOD->Release();
				}
			}

			//通知房间所有其他用户有新用户加入
			pIOD = m_nwIODPM.CreateStoreNWIOData();
			pIOD->SetBaseData(CPORDER_USERENTERRROOM, (int)i);
			(*pIOD) << pUser->GetName();
			SendNetWorkDate(*pIOD, pUser);
			pIOD->Release();

			//当前人数加一
			m_nCurClientNum++;

			//返回ID号
			return (int)i;
		}
	}

	//客满
	return SGD_ERROR_MAXUSER;
}

int CServerGameDesk::DelUser(CServerUser* pUser)
{
	BLUEASSERT(pUser);

	//是否存在该用户，存在则删除该用户
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
	{
		if (m_userColl[i] == pUser)
		{
			m_userColl[i] = BLUENULL;
			pUser->SetState(CServerUser::login);//用户退回已登录状态
			pUser->m_pRoom = BLUENULL;//用户不属于任何游戏桌
			pUser->SetPerpareGame(false);//用户为非游戏准备状态

			//通知所有用户该用户离开了房间
			CBLUENetWorkIODataPackage* pIOD = m_nwIODPM.CreateStoreNWIOData();
			pIOD->SetBaseData(CPORDER_USERLEAVEROOM, (int)i);
			SendNetWorkDate(*pIOD);
			pIOD->Release();

			//测试是否游戏结束了
			TestOverGame();

			//当前人数减一
			m_nCurClientNum--;

			//返回删除用户的ID号
			return (int)i;
		}
	}

	return SGD_ERROR_NOUSER;
}

bool CServerGameDesk::TestOverGame(void)
{
	if (m_state == ingame)
	{
		//只有一个用户在游戏，说明该用户已经赢得游戏
 		if (GetUserNumber() == 1)
			goto DOSTOPGAME;

		CServerUser* pNoGameOverUser = BLUENULL;
		for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		{
			if (m_userColl[i] && !m_userColl[i]->m_bIsGameOver)
			{
				if (pNoGameOverUser)
					return false;
				pNoGameOverUser = m_userColl[i];
			}
		}

DOSTOPGAME:
		//游戏结束
		m_state = normal;
		for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		{
			if (m_userColl[i])
			{
				m_userColl[i]->SetState(CServerUser::room);//用户退回房间内状态，不再是游戏状态
				m_userColl[i]->SetPerpareGame(false);

				//通知用户游戏结束
				CBLUENetWorkIODataPackage* pIOD = m_nwIODPM.CreateStoreNWIOData();
				pIOD->SetBaseData(CPORDER_STOPGAME, m_userColl[i]->IsGameOver());
				m_userColl[i]->SendNetWorkDate(*pIOD);
				pIOD->Release();
			}
		}
		return true;
	}

	return false;
}

int CServerGameDesk::StartGame(void)
{
	//如果房间已经在游戏中则不能再开始游戏
	if (m_state == ingame)
		return SGD_ERROR_HASINGAME;

	//如果房间里一个人都不到，不能开始游戏
	if (GetUserNumber() < 2)
		return SGD_ERROR_LESSUSER;

	//如果有用户没有准备好开始游戏则不能开始游戏
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i] && !m_userColl[i]->IsPerpareGame())
			return SGD_ERROR_LESSPREPARE;

	//设置所有用户的状态为游戏状态
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i])
		{
			m_userColl[i]->SetState(CServerUser::game);
			m_userColl[i]->m_bIsGameOver = false;//初始时游戏为非失败状态
		}

	//房间状态设置为ingame
	m_state = ingame;

	//游戏开始成功，则发送游戏开始消息
	CBLUENetWorkIODataPackage* pIOD = m_nwIODPM.CreateStoreNWIOData();
	pIOD->SetBaseData(CPORDER_STARTGAME, ::GetRandInt(), m_nBringToolRate);
	(*pIOD) << m_dwInitRows;
	SendNetWorkDate(*pIOD);
	pIOD->Release();

	return 0;
}

void CServerGameDesk::SendNetWorkDate(CBLUENetWorkIODataPackage& iod)
{
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i])
			m_userColl[i]->SendNetWorkDate(iod);
}

void CServerGameDesk::SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const int nExcludeID)
{
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (nExcludeID != i && m_userColl[i])
			m_userColl[i]->SendNetWorkDate(iod);
}

void CServerGameDesk::SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const CServerUser* pExcludeServerUser)
{
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i] != pExcludeServerUser && m_userColl[i])
			m_userColl[i]->SendNetWorkDate(iod);
}