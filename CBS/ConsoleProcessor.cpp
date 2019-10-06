#include "ConsoleProcessor.h"
#include <conio.h>

CConsoleProcessor::CConsoleProcessor(IBLUEMTSafeMessageQueue* pMSMQ, const BLUEDWORD dwMessageType) :
m_pMSMQ(pMSMQ),
m_dwMessageType(dwMessageType),
m_pProcessThread(BLUENULL)
{
	BLUEASSERT(m_pMSMQ);
}

CConsoleProcessor::~CConsoleProcessor(void)
{
	if (m_pProcessThread)
	{
		m_pProcessThread->Close(3000);
		::BLUEAdvDestroyThread(m_pProcessThread);
	}
}

bool CConsoleProcessor::StartProcess(void)
{
	BLUEASSERT(m_pProcessThread == BLUENULL);
	m_pProcessThread = ::BLUEAdvCreateThread();
	if (m_pProcessThread)
		return m_pProcessThread->Create(stProcessThread, this, false);

	return false;
}

BLUEDWORD CConsoleProcessor::ProcessThread(void)
{
	BLUEASSERT(m_pMSMQ);

	int ch;
	while ((ch = ::_getch()) != 'q')
	{
	}

	m_pMSMQ->PutOneMessage(m_dwMessageType, 0, 0, 0);
	return 0;
}

BLUEDWORD CConsoleProcessor::stProcessThread(void* p)
{
	BLUEASSERT(p);
	return ((CConsoleProcessor*)p)->ProcessThread();
}