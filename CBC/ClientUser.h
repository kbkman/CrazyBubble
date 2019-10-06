#pragma once

#include "..\Common\User.h"

/**
类名：客户端用户类
说明：在客户端用于保存和操作标识游戏中的其他的用户
*/
class CClientUser : public CUser
{
	friend class CClientUserManager;

public:
	CClientUser(void);
	virtual ~CClientUser(void);
};
