#pragma once

#include "..\Common\User.h"

/**
�������ͻ����û���
˵�����ڿͻ������ڱ���Ͳ�����ʶ��Ϸ�е��������û�
*/
class CClientUser : public CUser
{
	friend class CClientUserManager;

public:
	CClientUser(void);
	virtual ~CClientUser(void);
};
