#include "stdafx.h"
#include "CBC.h"
#include "../Common/ToolKit.h"
#include "CBCGameRun.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//�ڴ�й©���
	BLUECHECKMEMLEAK();

	//��ʼ������
	::InitToolKit();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//������Ϸ
	DXGAMERUNDATABASE dgrd = {};
	dgrd.hInstance = hInstance;
	dgrd.hPrevInstance = hPrevInstance;
	dgrd.lpCmdLine = lpCmdLine;
	dgrd.nCmdShow = nCmdShow;

	CCBCGameRun* pGameRun = new CCBCGameRun();
	pGameRun->Run(&dgrd);
	delete pGameRun;

	//�ͷŹ�����
	::ReleaseToolKit();

	return 0;
}