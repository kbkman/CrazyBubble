#pragma once

#include "BLUEServerNetWorkModel.h"
#include <set>
#include "ServerUser.h"

/**
���������������û�������
���ܣ����ڶԵ�¼�Ϸ��������û����й���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CServerUserManager
{
	typedef std::set<CServerUser*> SERVERUSERCOLL;

public:
	CServerUserManager(void);
	~CServerUserManager(void);

public:
	CServerUser* CreateServerUser(IBLUEServerNetWorkSession* pSNWSession);
	void DestroyServerUser(CServerUser* pServerUser);

	/*
	���������������û�������Ϣ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod);

	/*
	���������������û�������Ϣ������pExcludeServerUser���û���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const CServerUser* pExcludeServerUser);
	
	/*
	���������������û�������Ϣ������pExcludeName�û������û���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendNetWorkDate(CBLUENetWorkIODataPackage& iod, const BLUEString& pExcludeName);

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
		SERVERUSERCOLL::iterator it = m_serverUserColl.begin(), itEnd = m_serverUserColl.end();
		while (it != itEnd)
		{
			fn(*it);
			it++;
		}
	}

	/*
	�������������û�����ȡ�û�����
	���ܣ���
	˵������
	��������
	����ֵ���û�����ָ�룬����û��������򷵻�BLUENULL
	*/
	CServerUser* GetUser(const BLUEString& strName);

	/*
	���������û���¼
	���ܣ���
	˵������
	������serverUser ��¼����
	������strName ��¼�û���
	������strPassword ��¼����
	����ֵ����
	*/
	bool ServerUserLogin(CServerUser& serverUser, const BLUEString& strName, const BLUEString& strPassword);


private:
	void ClearServerUser(void);

private:
	SERVERUSERCOLL m_serverUserColl;

};
