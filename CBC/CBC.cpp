#include "stdafx.h"
#include "CBC.h"
#include "../Common/ToolKit.h"
#include "CBCGameRun.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//内存泄漏检测
	BLUECHECKMEMLEAK();

	//初始工具箱
	::InitToolKit();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//启动游戏
	DXGAMERUNDATABASE dgrd = {};
	dgrd.hInstance = hInstance;
	dgrd.hPrevInstance = hPrevInstance;
	dgrd.lpCmdLine = lpCmdLine;
	dgrd.nCmdShow = nCmdShow;

	CCBCGameRun* pGameRun = new CCBCGameRun();
	pGameRun->Run(&dgrd);
	delete pGameRun;

	//释放工具箱
	::ReleaseToolKit();

	return 0;
}