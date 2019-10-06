#pragma once

#include "BLUEAdv.h"
#include "BLUEClientNetWorkModel.h"
#include "BLUENetWorkModelHelper.h"

/**
�ӿ���������������ͻ������紦���������߽ӿ�
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class ICBCNetWorkProcessorListener
{
public:
	virtual void OnNWPLError(const int nErrorCode) = 0;//������Ϣ
	virtual void OnNWPLLoginServer(const BLUEString& strName) = 0;//��½��������
	virtual void OnNWPLRefreshRoomListBegin(void) = 0;//��ʼˢ�·����б�
	virtual void OnNWPLRoomInfo(int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum) = 0;//������Ϣ
	virtual void OnNWPLRefreshRoomListEnd(void) = 0;//����ˢ�·����б�
	virtual void OnNWPLEnterRoom(const int nRoomID, const int nID) = 0;//������IDΪnRoomID�ķ��䣬�Լ���ID��ΪnID
	virtual void OnNWPLQuitRoom(void) = 0;//�뿪�˷���
	virtual void OnNWPLRoomUser(const BLUEString& strName, const int nID) = 0;//�������Ѵ��ڵ��û�
	virtual void OnNWPLUserEnterRoom(const BLUEString& strName, const int nID) = 0;//���û������˷��䣬�����û���ΪstrName���ڷ������ID��ΪnID
	virtual void OnNWPLUserLeaveRoom(const int nID) = 0;//���û��뿪�˷��䣬�ڷ������ID��ΪnID
	virtual void OnNWPLPrepareGame(const int nID) = 0;//���û�׼���ÿ�ʼ��Ϸ�ˣ�nIDΪ���û���ID��
	virtual void OnNWPLStartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate) = 0;//��Ϸ��ʼ��nRandomΪ��Ϸ��ʼ�����������
	virtual void OnNWPLStopGame(bool bWin) = 0;//��Ϸ����,bWin��ʾ�Ƿ�ʤ��
	virtual void OnNWPLGameShootBubble(const int nID, const int nAngle) = 0;//ID��ΪnID���û�������һ�����ݣ������ݽǶ�ΪnAngle
	virtual void OnNWPLGameShootBadBubble(const int nID, const BLUEDWORD dwNumber) = 0;//ID��ΪnID���û������˻�����
	virtual void OnNWPLGameEraseBubble(const int nID, const BLUEDWORD dwNumber) = 0;//ID��ΪnID���û�����������
	virtual void OnNWPLGameAddLineBadBubble(const int nID) = 0;//ID��ΪnID���û�������л�����
	virtual void OnNWPLGameUnknowBubbleColor(int nID) = 0;//ID��ΪnID���û�����������ɫ
	virtual void OnNWPLGameThrowShit(int nID) = 0;//ID��ΪnID���û����ӱ��
	virtual void OnNWPLGameAdjustToolOrder(int nID) = 0;//ID��ΪnID���û��л�����˳��
	virtual void OnNWPLGameToolThrowBadBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber) = 0;//ID��nSrcID���û���ID��ΪnDestID���û�����dwNumber������
	virtual void OnNWPLGameToolEraseBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber) = 0;//ID��nSrcID���û���ID��ΪnDestID���û�������dwNumber������
	virtual void OnNWPLGameToolThrowLineBadBubble(const int nSrcID, const int nDestID) = 0;//ID��nSrcID���û���ID��ΪnDestID���û�������������
	virtual void OnNWPLGameToolChangeToolMode(const int nID) = 0;//ID��ΪnID���û������˵���ģʽ
	virtual void OnNWPLGameToolRemoveTool(const int nID) = 0;//ID��ΪnID���û��Ƴ��˵��ߣ���ʹ���˵��ߣ�
	virtual void OnNWPLGameToolUnknowBubbleColor(int nSrcID, int nDestID) = 0;//ID��nSrcID���û���ID��ΪnDestID���û���������
	virtual void OnNWPLGameToolThrowShit(int nSrcID, int nDestID) = 0;//ID��nSrcID���û���ID��ΪnDestID���û����˱��
};

//��������б�
#define NWP_ERROR_TYPE_GAMESTART          1      //��Ϸ�Ѿ���ʼ
#define NWP_ERROR_TYPE_HASINROOM         2      //�Ѿ������˸÷���
#define NWP_ERROR_TYPE_ROOMMAXUSER    3      //�������

/**
����������������ͻ������紦����
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCBCNetWorkProcessor : private IBLUECNWMMsgProcessorCallBack
{
public:
	/*
	ö����������ö��
	���ܣ���
	˵������
	*/
	enum CBCNWPRESULT
	{
		noerror,                      //�޳���
		nwiniterror,                //�����ʼ��ʧ��
		connecterror,             //����ʧ��
		connectederror,         //�Ѿ�����
		othererror                  //��������
	};

	/*
	ö����������״̬
	���ܣ���
	˵������
	*/
	enum NETSTATE
	{
		disconnect,             //δ����
		connect,                 //������
		login,                      //�ѵ�½
		room,                     //�ڷ�����
		game                     //����Ϸ��
	};

public:
	CCBCNetWorkProcessor(ICBCNetWorkProcessorListener* pListener);
	~CCBCNetWorkProcessor(void);

public:
	/*
	����������ȡ��ǰ����״̬
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	NETSTATE GetState(void) const {return m_state;}

	/*
	����������ȡ�Լ��ڷ����ڵ�ID��
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	int GetInRoomID(void) const {return m_nInRoomID;}

	/*
	������������������һ��
	���ܣ���
	˵�����ú���������Ϸ����ѯ
	*/
	void RunOne(void);

	/*
	�����������ӷ�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CBCNWPRESULT Connect(const BLUEStringA& strIP, const BLUEWORD nConnectPort);

	/*
	���������ر�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void Close(void);

public:
	void LoginServer(const BLUEString& strName, const BLUEString& strPassword);//��½
	void RoomList(void);//���󷿼��б���Ϣ
	void EnterRoom(const int nRoomID);//����
	void QuitRoom(void);//�뿪��ǰ����
	void PrepareGame(void);//׼���ÿ�ʼ��Ϸ
	void GameOver(void);//��Ϸ��Ϣ����Ϸʧ��
	void GameShootBubble(const int nAngle);//��Ϸ��Ϣ����������
	void GameShootBadBubble(const BLUEDWORD dwNumber);//��Ϸ��Ϣ�����仵����
	void GameEraseBubble(const BLUEDWORD dwNumber);//��Ϸ��Ϣ����������
	void GameAddLineBadBubble(void);//��Ϸ��Ϣ�����������ݻ���
	void GameUnknowBubbleColor(void);//��Ϸ��Ϣ������������ɫ
	void GameThrowShit(void);//��Ϸ��Ϣ���ܵ��ӱ��
	void GameAdjustToolOrder(void);//��Ϸ��Ϣ���л�����˳��
	void GameToolThrowBadBubble(const int nID, const BLUEDWORD dwNumber);//��Ϸ������Ϣ����ĳ�û��ӻ����ݣ�����ΪdwNumber
	void GameToolEraseBubble(const int nID, const BLUEDWORD dwNumber);//��Ϸ������Ϣ����ĳ�û��������ݣ�����ΪdwNumber
	void GameToolThrowLineBadBubble(const int nID);//��Ϸ������Ϣ����ĳ�û������л�����
	void GameToolChangeToolMode(void);//��Ϸ������Ϣ���л�����ģʽ������ģʽ\����ģʽ��
	void GameToolRemoveTool(void);//��Ϸ������Ϣ���Ƴ����ߣ��û�ʹ���˵��ߣ�
	void GameToolUnknowBubbleColor(int nID);//��Ϸ������Ϣ������������ɫ
	void GameToolThrowShit(int nID);//��Ϸ������Ϣ����ĳ�û��ӱ��

private:
	virtual void OnCNWRecvData(CBLUENetWorkIODataPackage& iod);
	virtual void OnCNWError(int nErrorCode);

private:
	/*
	������������״̬
	���ܣ���
	˵������
	*/
	NETSTATE m_state;

	/*
	�����������ڷ����ID��
	���ܣ���
	˵������
	*/
	int m_nRoomID;

	/*
	���������ڷ������ID��
	���ܣ���
	˵������
	*/
	int m_nInRoomID;

	/*
	�������������߶���
	���ܣ���
	˵������
	*/
	ICBCNetWorkProcessorListener* m_pListener;

	/*
	���������ͻ�������ģ��
	���ܣ���
	˵������
	*/
	IBLUEClientNetWorkModel* m_pCNWM;

	/*
	����������ȫ��Ϣ����
	���ܣ���
	˵������
	*/
	IBLUEMTSafeMessageQueue* m_pMSMQ;

	/*
	������������IO��������
	���ܣ���
	˵������
	*/
	CBLUENetWorkIODataPackageManager m_nwIODPM;

};
