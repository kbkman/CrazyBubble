#pragma once

#include "User.h"
#include "ServerGameDesk.h"
#include "BLUENetWorkModelHelper.h"

class CServerGameDesk;

/*
���������������û���
˵�����ڷ�������ڱ���Ͳ�����ʶ��Ϸ�е��û�
*/
class CServerUser : public CUser
{
	friend class CServerUserManager;
	friend class CServerGameDesk;

public:
	/*
	ö�������û�״̬ö��
	���ܣ���
	˵������
	*/
	enum SERVERUSERSTATE
	{
		connect,          //�û��Ѿ�����
		login,               //�û��Ѿ���½�����������Ӻ���ܵ�½��
		room,               //�û��Ѿ��ڷ����ڣ������ȵ�½��
		game,              //�û��Ѿ�����Ϸ�У��������ڷ����ڣ�
		death              //�û��Ѿ�����
	};


private:
	//ֻ����ԪCServerUserManager�����ſ��Դ��������ٱ������
	CServerUser(IBLUEServerNetWorkSession* pSNWSession);
	virtual ~CServerUser(void);

public:
	SERVERUSERSTATE GetState(void) const {return m_state;}
	void SetState(SERVERUSERSTATE state)
	{
		BLUEASSERT(
			state == connect ||
			state == login ||
			state == room ||
			state == game ||
			state == death
			);
		m_state = state;
	}
	bool IsPrepareGame(void) const {return m_bPerpareGame;}
	CServerGameDesk* GetRoom(void) const {return m_pRoom;}
	void SetGameOver(bool bOver) {m_bIsGameOver = bOver;}
	bool IsGameOver(void) const {return m_bIsGameOver;}

	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod) const
	{
		BLUEASSERT(m_pSNWSession);
		m_pSNWSession->SendData(&iod);
	}

private:
	/*
	���������û�״̬
	���ܣ���
	˵������
	*/
	SERVERUSERSTATE m_state;

	/*
	���������Ựָ��
	���ܣ����ڱ�����û���Ӧ������Ự
	˵������
	*/
	IBLUEServerNetWorkSession* const m_pSNWSession;

	/*
	���������û����ڵķ���ָ��
	���ܣ���
	˵����ֻ����״̬Ϊroom��gameʱ�Ų�ΪBLUENULL
	*/
	CServerGameDesk* m_pRoom;

	/*
	���������Ƿ��Ѿ���Ϸʧ�ܣ���ʧ�ܲ���ʾ������ˣ�
	���ܣ���
	˵������
	*/
	bool m_bIsGameOver;
};
