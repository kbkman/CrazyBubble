#pragma once

#include "BLUEAdv.h"

class CConsoleProcessor
{
public:
	CConsoleProcessor(IBLUEMTSafeMessageQueue* pMSMQ, const BLUEDWORD dwMessageType);
	~CConsoleProcessor(void);

	bool StartProcess(void);

private:
	BLUEDWORD ProcessThread(void);
	static BLUEDWORD stProcessThread(void* p);

private:
	IBLUEMTSafeMessageQueue* m_pMSMQ;
	BLUEDWORD m_dwMessageType;
	IBLUEThread* m_pProcessThread;
};
