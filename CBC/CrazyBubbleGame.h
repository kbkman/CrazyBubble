#pragma once
#include "crazybubblegamecore.h"
#include "CBCAnimationProcessor.h"
#include "CrazyBubbleGameToolManager.h"
#include "ResManager.h"
#include "CBCAudioProcessor.h"
#include "ClientUser.h"
#include "BLUEDirectX/engine2d.h"
#include "../Common/ToolKit.h"
#include <queue>

//��Ϸ����
#define CBG_ROWS                                         11       //��������
#define CBG_COLS                                           8        //��������
#define CBG_BUBBLE_RADIUS                         16      //���ݰ뾶
#define CBG_BUBBLEMOVE_SPEED                  25      //�����ƶ��ٶ�
#define CBG_BADBUBBLEMOVESPEED              45      //�������ƶ��ٶ�
#define CBG_GAMEAREATOTOP                        85      //ʵ����Ϸ���뻭���϶˾���
#define CBG_GAMEAREATOLEFT                       28      //ʵ����Ϸ���뻭����˾���
#define CBG_TOOLAREATOTOP                        10     //ʵ�ʵ������뻭���϶˾���
#define CBG_TOOLAREATOLEFT                       309     //ʵ�ʵ������뻭����˾���

/**
�ӿ�������Ϸ�����߽ӿ�
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class ICrazyBubbleGameMachineListener
{
public:
	//����������Ϸ������ʧ�ܣ��¼�
	virtual void OnCBGMEventGameOver(void* pCallBackParam) = 0;

	//�����������������¼�
	virtual void OnCBGMEventShootBubble(void* pCallBackParam, const int nAngle) = 0;

	//�����������仵�����¼�
	virtual void OnCBGMEventShootBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber) = 0;

	//�����������������¼�
	virtual void OnCBGMEventEraseBubble(void* pCallBackParam, const BLUEDWORD dwNumber) = 0;

	//�������������ƻ�������
	virtual void OnCBGMEventAddLineBadBubble(void* pCallBackParam) = 0;

	//������������������ɫ
	virtual void OnCBGMEventUnknowBubbleColor(void* pCallBackParam) = 0;

	//�����������ӱ�����
	virtual void OnCBGMEventThrowShit(void* pCallBackParam) = 0;

	//���������л�����
	virtual void OnCBGMEventAdjustToolOrder(void* pCallBackParam) = 0;

	//����������������ģʽ
	virtual void OnCBGMEventChangeToolMode(void* pCallBackParam) = 0;

	//���������Ƴ���һ�����ߣ���ʹ����һ�����ߣ�
	virtual void OnCBGMEventRemoveTool(void* pCallBackParam) = 0;

	//�����������������ʱ
	virtual void OnCBGMEventShootTimerOver(void* pCallBackParam) = 0;

	//��������ʹ���˵��ߣ��ӻ����ݣ�
	virtual void OnCBGMEventUsingToolForAddBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber) = 0;

	//��������ʹ���˵��ߣ��Ӷ��л����ݣ�
	virtual void OnCBGMEventUsingToolForLineBadBubble(void* pCallBackParam) = 0;

	//��������ʹ���˵��ߣ���һ�л����ݣ�
	virtual void OnCBGMEventUsingToolForAddOneLineBadBubble(void * pCallBackParam) = 0;

	//��������ʹ���˵��ߣ���һ�����ݣ�
	virtual void OnCBGMEventUsingToolForSubOneLineBadBubble(void* pCallBackParam) = 0;

	//��������ʹ���˵��ߣ�����������ɫ��
	virtual void OnCBGMEventUsingToolForUnknowBubbleColor(void* pCallBackParam) = 0;

	//��������ʹ���˵��ߣ��ӱ�㣩
	virtual void OnCBGMEventUsingToolForThrowShit(void* pCallBackParam) = 0;
};

/**
�����������ʱ��
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCBCShootTimer
{
public:
	CCBCShootTimer(void) {Reset();}

	//���÷����ʱ��
	void Reset(void)
	{
		m_nShootTimerLevel = 5;
		m_nShootTimer = 300;
	}

	/*
	�����������м�ʱ��
	���ܣ���
	˵������
	��������
	����ֵ������false��ʾ�Ѿ�
	*/
	bool RunOnce(void)
	{
		if (--m_nShootTimer <= 0)
		{
			if (--m_nShootTimerLevel <= 0)
			{
				Reset();
				return false;
			}
		}
		return true;
	}

private:
	/*
	�����������ݷ�������ñ���
	���ܣ���
	˵������
	*/
	int m_nShootTimerLevel;
	int m_nShootTimer;
};

class CCrazyBubbleGame : private ICrazyBubbleGameCoreListener
{
	/*
	ö������
	���ܣ���
	˵������
	*/
	enum GAMEACTIONTYPE
	{
		shootbubble,                       //�������
		shootbadbubble,                 //���仵����
		erasebubble,                       //��������
		addlinebadbubble,              //������л�����
		removetool,                        //ɾ������
		changetoolmode,               //�л�����ģʽ������ģʽ/����ģʽ��
		unknowbubblecolor,          //����������ɫ
		throwshit,                          //��ӱ���ϰ�
		adjusttoolorder                 //�л�����
	};

	/**
	�ṹ����
	���ܣ���
	˵������
	�汾��1.0.0.1
	���ߣ�BLUE
	*/
	struct GAMEACTION
	{
		GAMEACTIONTYPE actionType;        //��������(����GAMEACTIONTYPE)
		int pf;                                              //����1
		int ps;                                             //����2
	};

	//��Ϸ�������϶���
	typedef std::queue<GAMEACTION> GAMEACTIONCOLL;

public:
	/*
	ö��������Ϸ��״̬ö��
	���ܣ���
	˵������
	*/
	enum GAMEMACHINESTATE
	{
		nosituser,                      //���û�����
		hassituser,                    //���û�����
		preparegame,               //׼���ÿ�ʼ��Ϸ
		ingame,                         //��Ϸ������
		gameover,                     //��Ϸ������ʧ�ܣ�
		gamesuccess                 //��Ϸ������ʤ����
	};

public:
	/*
	������������һ����������Ϸ
	���ܣ���
	˵������
	������dd7 ddraw���ӿ�
	������sfSrcBg ��������
	������sfSrcBubble ������ݵı���
	������sfSrcShoot ��ŷ���ǹ�ı���
	����ֵ����
	*/
	CCrazyBubbleGame(
		ICrazyBubbleGameMachineListener* pListener,
		CDirectDraw7& dd7,
		const CResManager& resManager,
		CCBCAudioProcessor* pAudioProcessor
		);
	~CCrazyBubbleGame(void);

public:
	/*
	����������Ϸ�������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool SitUser(const BLUEString& strName, const int nID);

	/*
	���������������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool LeaveUser(void);

	/*
	����������ȡ��ǰ������Ϸ���ϵ���Ҷ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CClientUser* GetSitUser(void) const {return m_pUser;}

	/*
	������������һ��
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CDirectDrawSurface7& RunOne(void);

	/*
	����������ʼ��Ϸ
	���ܣ���
	˵������
	������nSeed ���������
	������dwInitRows ��ʼ����
	������nBringToolRate Я�����ߵĸ���,0��100
	����ֵ����
	*/
	bool StartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate);

	/*
	��������������Ϸ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool StopGame(void);

	/*
	����������ȡ��Ϸ��״̬
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	GAMEMACHINESTATE GetState(void) const {return m_state;}

	/*
	�����������ûص�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SetCallBackParam(void* pCallBackParam) {m_pCallBackParam = pCallBackParam;}

	/*
	����������ȡ�ص�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void* GetCallBackParam(void) const {return m_pCallBackParam;}

	/*
	���������Ƿ񲥷�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool IsPlaySound(void) const {return m_bPlaySound;}

	/*
	�������������Ƿ񲥷�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SetPlaySound(const bool bPlaySound) {m_bPlaySound = bPlaySound;}

	/*
	����������ȡ��ǰ�������ݵĽǶ�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	int GetCurrentShootAngle(void) const
	{
		BLUEASSERT(m_pGameCore);
		return m_pGameCore->GetCurrentShootAngle();
	}

	/*
	�����������ĵ���ģʽ����������ģʽ�����ӵ���ģʽ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void ChangeToolMode(void)
	{
		BLUEASSERT(m_pGameCore);
		BLUEASSERT(m_pListener);
		m_toolManager.UsingTool(!m_toolManager.IsUsingTool());
		m_pGameCore->CoerceRefresh();
		/*
		if (IsPlaySound())
			m_pAudioProcessor->PlayEffect(MUSICLINE_CHANGETOOL);
		*/
		m_pListener->OnCBGMEventChangeToolMode(m_pCallBackParam);
	}

	/*
	��������ʹ�õ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void UseTool(void);

	/*
	����������������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool ShootBubble(void)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->ShootBubble())
		{
			m_shootTimer.Reset();
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventShootBubble(m_pCallBackParam, GetCurrentShootAngle());
			//ÿ����һ�����ݣ��������ݸ�����һ
			if (m_nUnknowBubbleColorNum > 0)
				m_nUnknowBubbleColorNum--;
			return true;
		}
		return false;
	}

	/*
	�����������仵����
	���ܣ���
	˵������
	������dwNumber �����ݸ���
	����ֵ����
	*/
	bool ShootBadBubble(const BLUEDWORD dwNumber)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->ShootBadBubble(dwNumber))
		{
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventShootBadBubble(m_pCallBackParam, dwNumber);
			return true;
		}
		return false;
	}

	/*
	����������������
	���ܣ���
	˵������
	������dwNumber�����ĸ���
	����ֵ����
	*/
	bool EraseBubble(const BLUEDWORD dwNumber)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->EraseBubble(dwNumber))
		{
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventEraseBubble(m_pCallBackParam, dwNumber);
			return true;
		}
		return false;
	}

	/*
	�������������ƻ�������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool AddLineBadBubble(void)
	{
		BLUEASSERT(m_pGameCore);
		if (m_pGameCore->AddLineBadBubble())
		{
			BLUEASSERT(m_pListener);
			m_pListener->OnCBGMEventAddLineBadBubble(m_pCallBackParam);
			return true;
		}
		return false;
	}

	void UnknowBubbleColor(void)
	{
		BLUEASSERT(m_pGameCore);
		BLUEASSERT(m_pListener);
		m_nUnknowBubbleColorNum += 4;
		m_pGameCore->CoerceRefresh();
		m_pListener->OnCBGMEventUnknowBubbleColor(m_pCallBackParam);
	}

	void ThrowShit(void)
	{
		//���������������ζ�Ҫ���ţ�����IsPlaySound���ƣ�
		/*
		BLUEASSERT(m_pAudioProcessor);
		m_pAudioProcessor->PlayEffect(MUSICLINE_SHIT);
		*/

		//��ӱ�㶯��
		m_aniManager.AddShit();

		//�����¼�
		BLUEASSERT(m_pListener);
		m_pListener->OnCBGMEventThrowShit(m_pCallBackParam);
	}

	void AdjustToolOrder(void)
	{
		if (m_toolManager.AdjustToolOrder())
		{
			BLUEASSERT(m_pGameCore);
			BLUEASSERT(m_pListener);
			m_pGameCore->CoerceRefresh();
			m_pListener->OnCBGMEventAdjustToolOrder(m_pCallBackParam);
		}
	}

	//ת�����
	void WheelEmitter(const int nAngle) {BLUEASSERT(m_pGameCore);m_pGameCore->WheelEmitter(nAngle);}
	void WheelEmitterDeasil(void) {BLUEASSERT(m_pGameCore);m_spriteLong.NextFrame();m_pGameCore->WheelEmitterDeasil();}
	void WheelEmitterWiddershins(void) {BLUEASSERT(m_pGameCore);m_spriteLong.PrevFrame();m_pGameCore->WheelEmitterWiddershins();}

	//�������
	void ActionShootBubble(const int nAngle);
	void ActionShootBadBubble(const BLUEDWORD dwNumber);
	void ActionEraseBubble(const BLUEDWORD dwNumber);
	void ActionAddLineBadBubble(void);
	void ActionChangeToolMode(void);
	void ActionRemoveTool(void);
	void ActionUnknowBubbleColor(void);//������ɫ�޷�����
	void ActionThrowShit(void);//���ӵ����
	void ActionAdjustToolOrder(void);//�л�����˳��

	//��Ϸ�����¼�
	virtual void CBGCGameOver(void);
	virtual void CBGCBubbleCreate(CBubble* pBubble);
	virtual void CBGCStartDraw(void);
	virtual void CBGCEndDraw(void);
	virtual void CBGCDrawBubble(const CBubble* pBubble);
	virtual void CBGCDrawCurrentShootBubble(const CBubble* pBubble);
	virtual void CBGCDrawNextShootBubble(const CBubble* pBubble);
	virtual void CBGCWheelEmitter(const int nAngle);
	virtual void CBGCBubbleShoot(const CBubble* pBubble);
	virtual void CBGCBubbleFix(const CBubble* pBubble);
	virtual void CBGCBubbleKickWall(const CBubble* pBubble);
	virtual void CBGCBubbleDropStart(void);
	virtual void CBGCBubbleDrop(const CBubble* pBubble);
	virtual void CBGCBubbleDropEnd(const BLUEDWORD dwDropNumber);
	virtual void CBGCBubbleEraseStart(void);
	virtual void CBGCBubbleErase(const CBubble* pBubble);
	virtual void CBGCBubbleEraseEnd(const BLUEDWORD dwEraseNumber);

public:
	/*
	��������������Ƶ���Ļ�ϵ��������ⲿ����
	���ܣ���
	˵������
	*/
	RECT m_drawRc;

	/*
	���������ⲿʹ�õ�ID�ţ����ʹ�õ���ʱ���ո�ID��ʹ�õ���
	���ܣ���
	˵������ID��������ڷ����ڵ�ID�벻ͬ
	*/
	int m_nExternUseID;

private:
	/*
	������������������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void ProcessActionQueue(void);

private:
	/*
	����������Ϸ��״̬
	���ܣ���
	˵������
	*/
	GAMEMACHINESTATE m_state;

	/*
	��������������
	���ܣ���
	˵������
	*/
	ICrazyBubbleGameMachineListener* m_pListener;

	/*
	���������ڷ����е�ǰ��ID��
	���ܣ���
	˵����Ĭ��Ϊ-1����ʾû����λ
	*/
	int m_nIDInRoom;

	/*
	���������������
	���ܣ���
	˵������
	*/
	CBLUERandom* m_pRandom;

	/*
	����������Ϸ���Ķ���
	���ܣ���
	˵������
	*/
	CCrazyBubbleGameCore* m_pGameCore;

	/*
	������������Ϸ���û�����
	���ܣ���
	˵������
	*/
	CClientUser* m_pUser;

	/*
	����������Ϸ���ջ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CDirectDrawSurface7 m_surfaceCanvas;

	/*
	����������Ϸ�����õĻ�������
	���ܣ����ڻ���������Ϸ�е�����
	˵������
	*/
	CDirectDrawSurface7 m_surfaceGameCoreCanvas;

	/*
	�����������������Ƶ�Դ����
	���ܣ���
	˵������
	*/
	CDirectDrawSurface7 m_surfaceUserName;

	/*
	����������Դ������
	���ܣ���
	˵������
	*/
	const CResManager& m_resManager;

	//����
	CDDrawSprite m_spriteBubble;//���ݾ���
	CDDrawSprite m_spriteLong;//������
	CDDrawSprite m_spriteTool;//���߾���
	CDDrawSprite m_spriteUserNum;//�û���ž���

	/*
	�����������ڻ������ǹ�ı���
	���ܣ���
	˵������
	*/
	CDirectDrawSurface7 m_surfaceShoot;

	/*
	���������Ƿ񲥷�����
	���ܣ���
	˵������
	*/
	bool m_bPlaySound;

	/*
	����������Ƶ������
	���ܣ���
	˵������
	*/
	CCBCAudioProcessor* m_pAudioProcessor;

	/*
	���������ص�����
	���ܣ���
	˵������
	*/
	void* m_pCallBackParam;

	/*
	����������������
	���ܣ���
	˵������
	*/
	GAMEACTIONCOLL m_actionQueue;

	/*
	�����������߹�����
	���ܣ���
	˵������
	*/
	CCrazyBubbleGameToolManager m_toolManager;

	/*
	������������������
	���ܣ���
	˵������
	*/
	CCBCAnimationProcessor m_aniManager;

	/*
	��������Я�����ߵĸ���
	���ܣ���
	˵������
	*/
	int m_nBringToolRate;

	/*
	���������������ݸ���
	���ܣ���
	˵������
	*/
	int m_nUnknowBubbleColorNum;

	/*
	�������������ʱ��
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CCBCShootTimer m_shootTimer;
};
