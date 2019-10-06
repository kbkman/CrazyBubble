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

	//ÿ�������̶��������ȶ��û����С
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

	//����Ѿ�����Ϸ�У����ܽ���
	if (m_state == ingame)
		return SGD_ERROR_INGAME;

	//����Ѿ��ڷ���������ظ�����
	if (HasUser(pUser))
		return SGD_ERROR_HASUSER;

	//�鿴�Ƿ��п�λ���û�����
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
	{
		if (m_userColl[i] == BLUENULL)
		{
			//�����û��ڷ���������
			m_userColl[i] = pUser;
			pUser->SetState(CServerUser::room);
			pUser->m_pRoom = this;
			pUser->m_bIsGameOver = false;//��ʼʱ��ϷΪ��ʧ��״̬

			//֪ͨ�û������������û�
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

			//֪ͨ�������������û������û�����
			pIOD = m_nwIODPM.CreateStoreNWIOData();
			pIOD->SetBaseData(CPORDER_USERENTERRROOM, (int)i);
			(*pIOD) << pUser->GetName();
			SendNetWorkDate(*pIOD, pUser);
			pIOD->Release();

			//��ǰ������һ
			m_nCurClientNum++;

			//����ID��
			return (int)i;
		}
	}

	//����
	return SGD_ERROR_MAXUSER;
}

int CServerGameDesk::DelUser(CServerUser* pUser)
{
	BLUEASSERT(pUser);

	//�Ƿ���ڸ��û���������ɾ�����û�
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
	{
		if (m_userColl[i] == pUser)
		{
			m_userColl[i] = BLUENULL;
			pUser->SetState(CServerUser::login);//�û��˻��ѵ�¼״̬
			pUser->m_pRoom = BLUENULL;//�û��������κ���Ϸ��
			pUser->SetPerpareGame(false);//�û�Ϊ����Ϸ׼��״̬

			//֪ͨ�����û����û��뿪�˷���
			CBLUENetWorkIODataPackage* pIOD = m_nwIODPM.CreateStoreNWIOData();
			pIOD->SetBaseData(CPORDER_USERLEAVEROOM, (int)i);
			SendNetWorkDate(*pIOD);
			pIOD->Release();

			//�����Ƿ���Ϸ������
			TestOverGame();

			//��ǰ������һ
			m_nCurClientNum--;

			//����ɾ���û���ID��
			return (int)i;
		}
	}

	return SGD_ERROR_NOUSER;
}

bool CServerGameDesk::TestOverGame(void)
{
	if (m_state == ingame)
	{
		//ֻ��һ���û�����Ϸ��˵�����û��Ѿ�Ӯ����Ϸ
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
		//��Ϸ����
		m_state = normal;
		for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		{
			if (m_userColl[i])
			{
				m_userColl[i]->SetState(CServerUser::room);//�û��˻ط�����״̬����������Ϸ״̬
				m_userColl[i]->SetPerpareGame(false);

				//֪ͨ�û���Ϸ����
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
	//��������Ѿ�����Ϸ�������ٿ�ʼ��Ϸ
	if (m_state == ingame)
		return SGD_ERROR_HASINGAME;

	//���������һ���˶����������ܿ�ʼ��Ϸ
	if (GetUserNumber() < 2)
		return SGD_ERROR_LESSUSER;

	//������û�û��׼���ÿ�ʼ��Ϸ���ܿ�ʼ��Ϸ
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i] && !m_userColl[i]->IsPerpareGame())
			return SGD_ERROR_LESSPREPARE;

	//���������û���״̬Ϊ��Ϸ״̬
	for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
		if (m_userColl[i])
		{
			m_userColl[i]->SetState(CServerUser::game);
			m_userColl[i]->m_bIsGameOver = false;//��ʼʱ��ϷΪ��ʧ��״̬
		}

	//����״̬����Ϊingame
	m_state = ingame;

	//��Ϸ��ʼ�ɹ���������Ϸ��ʼ��Ϣ
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