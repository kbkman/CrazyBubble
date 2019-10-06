#include "StdAfx.h"
#include "CrazyBubbleService.h"
#include "BLUEAdv.h"
#include "BLUEServerNetWorkModel.h"
#include "CrazyBubbleServiceCore.h"
#include "ConsoleProcessor.h"

//投入多线程安全消息队列的所有消息类型
#define MSMQMSG_TYPE_SNWM             1          //来自服务器网络模型的消息
#define MSMQMSG_TYPE_CONSOL          2          //来自控制台的消息

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

	//创建服务器核心对象
	pServiceCore = new CCrazyBubbleServiceCore();

	//创建一个多线程安全消息队列（必须先创建最的一个销毁，因为其他模型和工具均依赖于消息队列）
	if (!(pMSMQ = ::BLUEAdvCreateMSMQ()))
		goto ERROR_PROCESS;

	//创建服务器网络模型
	if (!(pSNWM = ::BLUECreateSNWM()))
		goto ERROR_PROCESS;
	int nResult =  pSNWM->Create(pMSMQ, MSMQMSG_TYPE_SNWM, BLUENULL, 8080);
	if (nResult != SNWMRESULT_OK)
	{
		switch (nResult)
		{
		case SNWMRESULT_E_LISTEN:
			_tprintf(_BLUET("error绑定地址或端口失败\n"));
			break;
		}
		goto ERROR_PROCESS;
	}
	pSNWM->Run();//必须调用运行函数，否则不会运行

	//创建控制台处理对象，用于处理用户控制台输入的命令
	pCP  = new CConsoleProcessor(pMSMQ, MSMQMSG_TYPE_CONSOL);
	if (!pCP->StartProcess())
		goto ERROR_PROCESS;

	_tprintf(_BLUET("服务器启动，按Q键退出服务器程序\n"));
	BLUETRACE(_BLUET("服务程序正常启动，开始消息循环\n"));
	//开始循环消息并处理
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
	_tprintf(_BLUET("服务器程序停止\n"));
	BLUETRACE(_BLUET("消息循环结束，服务程序销毁\n"));

ERROR_PROCESS:

	//销毁控制台处理器
	if (pCP)
		delete pCP;

	//销毁服务器网络模型
	if (pSNWM)
		::BLUEDestroySNWM(pSNWM);

	//销毁多线程安全消息队列
	if (pMSMQ)
		::BLUEAdvDestroyMSMQ(pMSMQ);

	//销毁服务器核心对象
	if (pServiceCore)
		delete pServiceCore;
}