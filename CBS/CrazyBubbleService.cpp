#include "StdAfx.h"
#include "CrazyBubbleService.h"
#include "BLUEAdv.h"
#include "BLUEServerNetWorkModel.h"
#include "CrazyBubbleServiceCore.h"
#include "ConsoleProcessor.h"

//Ͷ����̰߳�ȫ��Ϣ���е�������Ϣ����
#define MSMQMSG_TYPE_SNWM             1          //���Է���������ģ�͵���Ϣ
#define MSMQMSG_TYPE_CONSOL          2          //���Կ���̨����Ϣ

CCrazyBubbleService::CCrazyBubbleService(void)
{
}

CCrazyBubbleService::~CCrazyBubbleService(void)
{
}

void CCrazyBubbleService::Run(void)
{
	CCrazyBubbleServiceCore* pServiceCore = BLUENULL;
	IBLUEMTSafeMessageQueue* pMSMQ = BLUENULL;
	IBLUEServerNetWorkModel* pSNWM = BLUENULL;
	CConsoleProcessor* pCP = BLUENULL;
	BLUEDWORD dwMsgType, fp;
	void *sp, *tp;

	//�������������Ķ���
	pServiceCore = new CCrazyBubbleServiceCore();

	//����һ�����̰߳�ȫ��Ϣ���У������ȴ������һ�����٣���Ϊ����ģ�ͺ͹��߾���������Ϣ���У�
	if (!(pMSMQ = ::BLUEAdvCreateMSMQ()))
		goto ERROR_PROCESS;

	//��������������ģ��
	if (!(pSNWM = ::BLUECreateSNWM()))
		goto ERROR_PROCESS;
	int nResult =  pSNWM->Create(pMSMQ, MSMQMSG_TYPE_SNWM, BLUENULL, 8080);
	if (nResult != SNWMRESULT_OK)
	{
		switch (nResult)
		{
		case SNWMRESULT_E_LISTEN:
			_tprintf(_BLUET("error�󶨵�ַ��˿�ʧ��\n"));
			break;
		}
		goto ERROR_PROCESS;
	}
	pSNWM->Run();//����������к��������򲻻�����

	//��������̨����������ڴ����û�����̨���������
	pCP  = new CConsoleProcessor(pMSMQ, MSMQMSG_TYPE_CONSOL);
	if (!pCP->StartProcess())
		goto ERROR_PROCESS;

	_tprintf(_BLUET("��������������Q���˳�����������\n"));
	BLUETRACE(_BLUET("�������������������ʼ��Ϣѭ��\n"));
	//��ʼѭ����Ϣ������
	while (pMSMQ->GetOneMessage(dwMsgType, fp, sp, tp))
	{
		switch (dwMsgType)
		{
		case MSMQMSG_TYPE_SNWM:
			::BLUEProcessSNWMMsg(fp, sp, tp, pServiceCore);
			break;

		case MSMQMSG_TYPE_CONSOL:
			pMSMQ->PutOneQuitMessage();
			break;

		default:
			break;
		}

	}
	_tprintf(_BLUET("����������ֹͣ\n"));
	BLUETRACE(_BLUET("��Ϣѭ�������������������\n"));

ERROR_PROCESS:

	//���ٿ���̨������
	if (pCP)
		delete pCP;

	//���ٷ���������ģ��
	if (pSNWM)
		::BLUEDestroySNWM(pSNWM);

	//���ٶ��̰߳�ȫ��Ϣ����
	if (pMSMQ)
		::BLUEAdvDestroyMSMQ(pMSMQ);

	//���ٷ��������Ķ���
	if (pServiceCore)
		delete pServiceCore;
}