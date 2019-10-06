// CBS.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CrazyBubbleService.h"
#include "ToolKit.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//内存泄漏检测
	BLUECHECKMEMLEAK();

	/*
	工具箱初始化
	必须在游戏运行前初始化
	因为游戏中要用到工具箱中各种工具
	*/
	::InitToolKit();

	//生成并运行服务框架
	CCrazyBubbleService* pCBService = new CCrazyBubbleService();
	pCBService->Run();
	delete pCBService;

	/*
	最后工具箱用完后必须释放，
	否则可能引起资源泄漏
	*/
	::ReleaseToolKit();//释放工具箱

	//返回main
	return 0;
}

