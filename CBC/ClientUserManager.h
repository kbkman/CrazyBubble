#pragma once

#include "ClientUser.h"
#include <map>

class CClientUserManager
{
	//�û����ϣ����û����û���Ϊ��
	typedef std::map<BLUEString, CClientUser*> USERCOLL;

public:
	CClientUserManager(void);
	~CClientUserManager(void);

public:
	/*
	������������û�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void Clear(void);

	/*
	�������������û�����ȡ�û�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CClientUser* GetUser(const BLUEString& strUserName);

	/*
	�����������һ���û�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CClientUser* AddUser(const BLUEString& strUserName);

private:
	USERCOLL m_userColl;
};
