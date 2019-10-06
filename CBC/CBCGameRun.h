#pragma once

#include "BLUEDirectX/game2d.h"
#include "ResManager.h"
#include "CBCGUIProcessor.h"
#include "CBCNetWorkProcessor.h"
#include "CBCAudioProcessor.h"
#include "ClientRoomManager.h"
#include "ClientUserManager.h"
#include <vector>
#include "CrazyBubbleGame.h"

/**
��������Ϸ������
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCBCGameRun :
	public CDXGameRun2D,
	private ICBCGUIProcessorListener,
	private ICBCNetWorkProcessorListener,
	private ICrazyBubbleGameMachineListener
{
	typedef std::vector<CCrazyBubbleGame*> GAMECOLL;

	//��Ϸ����״̬
	enum GAMERUNSTATE
	{
		logo,                             //logo����
		login,                            //��½����
		outroom,                      //���������
		inroom                          //�����ڽ���
	};

public:
	CCBCGameRun(void);
	~CCBCGameRun(void);

private:
	virtual HRESULT GameInit(void);
	virtual HRESULT GameMain(void);
	virtual HRESULT GameShutDown(void);

	/*
	�����������紦��
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void NetWorkProcess(void) {BLUEASSERT(m_pNWProcessor);m_pNWProcessor->RunOne();}

	/*
	�����������루������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void InputProcess(void);

	/*
	����������Ⱦ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void Render(void);

	/*
	�����������´��ڱ����ļ�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void UpdateWindowCaptionText(void);

	/*
	�����������ⲿID��ȡ�ڲ�ID
	���ܣ���
	˵��������CCrazyBubbleGame::m_nExternID
	��������
	����ֵ������-1��ʾ�����ڸ��û�
	*/
	int GetIDFromExternID(const int nExternID);

	/*
	����������ȡ�Լ��ڷ����е�ID��
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	int GetID(void) const {BLUEASSERT(m_pNWProcessor);return m_pNWProcessor->GetInRoomID();}
	
	/*
	���������ϣ��£�һҳ�����б�
	���ܣ���
	˵����һҳֻ��ʾ4��������Ϣ
	��������
	����ֵ����
	*/
	void PrevRLPage(void);
	void NextRLPage(void);

	/*
	��������������Ƕ���
	���ܣ���
	˵������
	������nSrcID ���ĸ�ID�����Ϸ���
	������nDestID �͵��ĸ�ID������
	����ֵ����
	*/
	void AddStarAni(int nSrcID, int nDestID);

	/*
	��������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	virtual bool OnPreMainWndProc(HWND, UINT, WPARAM, LPARAM);

	//UI��ػص�
	virtual void OnGUIPChangeUIType(int uiType);
	virtual void OnGUIPLConnectServer(const BLUEString& strIP, BLUEWORD nPort, const BLUEString& strUserName, const BLUEString& strPassWord);
	virtual void OnGUIPLPrevRLPage(void);
	virtual void OnGUIPLNextRLPage(void);
	virtual void OnGUIPLEnterRoom(int nRoomID);
	virtual void OnGUIPLPrepareGame(void);
	virtual void OnGUIPLQuitRoom(void);
	virtual void OnGUIPLHelp(void);
	virtual void OnGUIPLAbout(void);

	//������ػص�
	virtual void OnNWPLError(const int nErrorCode);
	virtual void OnNWPLLoginServer(const BLUEString& strName);
	virtual void OnNWPLRefreshRoomListBegin(void);
	virtual void OnNWPLRoomInfo(int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum);
	virtual void OnNWPLRefreshRoomListEnd(void);
	virtual void OnNWPLEnterRoom(const int nRoomID, const int nID);
	virtual void OnNWPLQuitRoom(void);
	virtual void OnNWPLRoomUser(const BLUEString& strName, const int nID);
	virtual void OnNWPLUserEnterRoom(const BLUEString& strName, const int nID);
	virtual void OnNWPLUserLeaveRoom(const int nID);
	virtual void OnNWPLPrepareGame(const int nID);
	virtual void OnNWPLStartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate);
	virtual void OnNWPLStopGame(bool bWin);
	virtual void OnNWPLGameShootBubble(const int nID, const int nAngle);
	virtual void OnNWPLGameShootBadBubble(const int nID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameEraseBubble(const int nID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameAddLineBadBubble(const int nID);
	virtual void OnNWPLGameUnknowBubbleColor(int nID);
	virtual void OnNWPLGameThrowShit(int nID);
	virtual void OnNWPLGameAdjustToolOrder(int nID);
	virtual void OnNWPLGameToolThrowBadBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameToolEraseBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber);
	virtual void OnNWPLGameToolThrowLineBadBubble(const int nSrcID, const int nDestID);
	virtual void OnNWPLGameToolChangeToolMode(const int nID);
	virtual void OnNWPLGameToolRemoveTool(const int nID);
	virtual void OnNWPLGameToolUnknowBubbleColor(int nSrcID, int nDestID);
	virtual void OnNWPLGameToolThrowShit(int nSrcID, int nDestID);

	//��Ϸ����ػص�
	virtual void OnCBGMEventGameOver(void* pCallBackParam);
	virtual void OnCBGMEventShootBubble(void* pCallBackParam, const int nAngle);
	virtual void OnCBGMEventShootBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber);
	virtual void OnCBGMEventEraseBubble(void* pCallBackParam, const BLUEDWORD dwNumber);
	virtual void OnCBGMEventAddLineBadBubble(void* pCallBackParam);
	virtual void OnCBGMEventUnknowBubbleColor(void* pCallBackParam);
	virtual void OnCBGMEventThrowShit(void* pCallBackParam);
	virtual void OnCBGMEventChangeToolMode(void* pCallBackParam);
	virtual void OnCBGMEventRemoveTool(void* pCallBackParam);
	virtual void OnCBGMEventAdjustToolOrder(void* pCallBackParam);
	virtual void OnCBGMEventShootTimerOver(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForAddBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber);
	virtual void OnCBGMEventUsingToolForLineBadBubble(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForAddOneLineBadBubble(void * pCallBackParam);
	virtual void OnCBGMEventUsingToolForSubOneLineBadBubble(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForUnknowBubbleColor(void* pCallBackParam);
	virtual void OnCBGMEventUsingToolForThrowShit(void* pCallBackParam);

private:
	/*
	����������Ϸ����״̬
	���ܣ���
	˵������
	*/
	GAMERUNSTATE m_state;

	/*
	����������ǰѡ����û���ʹ�õ��ߵĶ���ID
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	int m_nSelID;

	/*
	���������Լ����û���
	���ܣ���
	˵������
	*/
	BLUEString m_strSelfUserName;

	/*
	�����������й�����
	���ܣ���
	˵������
	*/
	CResManager m_resManager;//��Դ������
	CClientRoomManager m_rManager;//���������
	//CClientUserManager m_uManager;//�û�������

	/*
	�����������д�����
	���ܣ���
	˵������
	*/
	CCBCGUIProcessor* m_pGUIProcessor;//�û�UI������
	CCBCAudioProcessor* m_pAudioProcessor;//��Ƶ������
	CCBCNetWorkProcessor*m_pNWProcessor;//���紦����
	CCBCAnimationProcessor m_aniProcessor;//����������


	/*
	����������Ϸ������
	���ܣ���
	˵������
	*/
	GAMECOLL m_gameColl;
};
