#include "StdAfx.h"
#include "ServerUser.h"

CServerUser::CServerUser(IBLUEServerNetWorkSession* pSNWSession) :
m_pSNWSession(pSNWSession),
m_state(connect),
m_pRoom(BLUENULL)
{
	BLUEASSERT(pSNWSession);
}

CServerUser::~CServerUser(void)
{
}
