#pragma once
#include "..\Common\room.h"

class CClientRoom :
	public CRoom
{
	friend class CClientRoomManager;

private:
	CClientRoom(void);
	~CClientRoom(void);
};
