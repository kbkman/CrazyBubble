// CBS.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CrazyBubbleService.h"
#include "ToolKit.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//�ڴ�й©���
	BLUECHECKMEMLEAK();

	/*
	�������ʼ��
	��������Ϸ����ǰ��ʼ��
	��Ϊ��Ϸ��Ҫ�õ��������и��ֹ���
	*/
	::InitToolKit();

	//���ɲ����з�����
	CCrazyBubbleService* pCBService = new CCrazyBubbleService();
	pCBService->Run();
	delete pCBService;

	/*
	��󹤾������������ͷţ�
	�������������Դй©
	*/
	::ReleaseToolKit();//�ͷŹ�����

	//����main
	return 0;
}

