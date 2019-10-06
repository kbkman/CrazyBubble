#pragma once

#include "BLUENetWorkModelHelper.h"
#include "Room.h"
#include "ServerUser.h"
#include <vector>

#define DESK_MAX_USERNUMBER                6    //ÿ����Ϸ���������

class CServerUser;
/**
��������Ϸ��
���ܣ���
˵����ÿ����Ϸ��������һ����Ϸ���Ͻ���
�汾��1.0.0.1
���ߣ�BLUE
*/

//�����б���������ֵ������>=0������
#define SGD_ERROR_INGAME             (-1)           //�����Ѿ�����Ϸ��
#define SGD_ERROR_HASUSER           (-2)           //�Ѿ����ڸ��û�
#define SGD_ERROR_MAXUSER           (-3)           //����
#define SGD_ERROR_NOUSER             (-4)           //�޸��û�
#define SGD_ERROR_HASINGAME        (-5)          //�Ѿ�����Ϸ��
#define SGD_ERROR_LESSUSER          (-6)           //�û�̫��
#define SGD_ERROR_LESSPREPARE    (-7)           //���������û�׼����

class CServerGameDesk : public CRoom
{
	typedef std::vector<CServerUser*> ROOMSERVERUSERCOLL;
	friend class CServerGameDeskManager;

public:
	/*
	ö��������Ϸ����״̬
	���ܣ���
	˵������
	*/
	enum STATE {
		normal,           //��ͨ
		ingame           //��Ϸ��
	};

private:
	CServerGameDesk(
		CBLUENetWorkIODataPackageManager& nwIODPM,
		const BLUEString& strName,
		BLUEDWORD dwInitRows = 3,
		int nBringToolRate = 10);
	~CServerGameDesk(void);

public:
	/*
	���������û������ID��
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	int GetMaxServerUserID(void) const {return DESK_MAX_USERNUMBER;}

	/*
	������������ID�Ż�ȡ�û�����
	���ܣ���
	˵���������ID����Ч��
	��������
	����ֵ����
	*/
	CServerUser* GetServerUser(int nID) const {return m_userColl[nID];}

	/*
	������������ID�Ż�ȡ�û�����
	���ܣ���
	˵�������ID����Ч��
	��������
	����ֵ����
	*/
	CServerUser* GetServerUserSafe(int nID) const
	{
		if (nID >= 0 && nID < DESK_MAX_USERNUMBER)
			return m_userColl[nID];
		return BLUENULL;
	}

	/*
	��������ÿ���û�ִ���ض�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	template<class FN>
	void ForEach(FN fn)
	{
		for (ROOMSERVERUSERCOLL::size_type i = 0; i < m_userColl.size(); i++)
			if (m_userColl[i])
				fn(m_userColl[i], (int)i);
	}

	/*
	����������ȡ��Ϸ����ǰ��״̬
	���ܣ���
	˵������Ϸ����ǰ״̬����ͨ״̬������Ϸ��״̬����
	��������
	����ֵ����Ϸ����ǰ��״̬
	*/
	STATE GetState(void) const {return m_state;}

	/*
	����������Ϸ�����Ƿ��и��û�
	���ܣ���
	˵������
	�������û�����ָ��
	����ֵ����
	*/
	bool HasUser(const CServerUser* pUser);

	/*
	����������ȡ��ǰ�����ڵ�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	int GetUserNumber(void) const;

	/*
	�����������һ�����
	���ܣ���
	˵������
	��������
	����ֵ��������ڵ���0�����ʾ�����ҳɹ�������ֵΪ�û��ڷ����ڵ�ID�ţ�
	��������С��0��ʾ�����������SGD_ERROR_   ��
	*/
	int AddUser(CServerUser* pUser);

	/*
	���������Ƴ�һ�����
	���ܣ���
	˵��������뿪��Ϸ������������ɾ��һ����Ҷ���
	��������
	����ֵ��������ڵ���0�����ʾɾ����ҳɹ�������ֵΪ�û��ڷ����ڵ�ID�ţ�
	��������С��0��ʾ�����������SGD_ERROR_   ��
	*/
	int DelUser(CServerUser* pUser);

	/*
	����������Ϸ�Ƿ��Ѿ�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool TestOverGame(void);


	/*
	����������Ϸ����ʼ��Ϸ
	���ܣ���
	˵������
	��������
	����ֵ��>=0��ʾ��ʼ��Ϸ�ɹ�
	*/
	int StartGame(void);

	/*
	������������Ϸ���������û�������Ϣ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod);

	/*
	������������Ϸ���������û�������Ϣ������IDΪnExcludeID���û���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const int nExcludeID);

	/*
	������������Ϸ���������û�������Ϣ������pExcludeServerUser���û���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const CServerUser* pExcludeServerUser);

private:
	/*
	��������IO��������
	���ܣ���
	˵������
	*/
	CBLUENetWorkIODataPackageManager& m_nwIODPM;

	/*
	���������û�����
	���ܣ�����ÿ����Ϸ���ϵ������û�
	˵������
	*/
	ROOMSERVERUSERCOLL m_userColl;


	/*
	����������Ϸ����ǰ״̬
	���ܣ���
	˵������
	*/
	STATE m_state;

	/*
	����������Ϸ�������
	���ܣ���
	˵������
	*/
	BLUEDWORD m_dwInitRows;//��ʼ��Ϸʱ�ĳ�ʼ����
	int m_nBringToolRate;//��ϷЯ�����߸���
};
