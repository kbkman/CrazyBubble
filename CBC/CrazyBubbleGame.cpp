#include "stdafx.h"
#include "CrazyBubbleGame.h"
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#include "resource.h"


CCrazyBubbleGame::CCrazyBubbleGame(
								   ICrazyBubbleGameMachineListener* pListener,
								   CDirectDraw7& dd7,
								   const CResManager& resManager,
								   CCBCAudioProcessor* pAudioProcessor) :
m_pListener(pListener),
m_state(nosituser),
m_pUser(BLUENULL),
m_nIDInRoom(-1),
m_bPlaySound(false),
m_pCallBackParam(BLUENULL),
m_nExternUseID(0),
m_toolManager(dd7, resManager.GetSfSrcToolBg(), resManager.GetSfSrcToolBigBubble(), resManager.GetSfSrcToolSmallBubble(), resManager.GetSfSrcTool()),
m_resManager(resManager),
m_pAudioProcessor(pAudioProcessor),
m_aniManager(resManager),
m_nBringToolRate(0),
m_nUnknowBubbleColorNum(0)
{
	BLUEASSERT(m_pListener);
	//BLUEASSERT(m_pAudioProcessor);

	//���������������Ϸ����
	m_pRandom = new CBLUERandom(1);
	m_pGameCore = new CCrazyBubbleGameCore(
		1,
		CBG_BUBBLE_RADIUS,
		CBG_ROWS,
		CBG_COLS,
		CBG_BUBBLEMOVE_SPEED,
		CBG_BADBUBBLEMOVESPEED,
		this,
		0
		);

	m_spriteBubble.Create(m_resManager.GetSfSrcBubble(), 32, 32);
	m_spriteTool.Create(m_resManager.GetSfSrcTool(), 32, 32);
	m_spriteLong.Create(m_resManager.GetSfSrcLong(), 46, 36);
	m_spriteUserNum.Create(m_resManager.GetSfSrcUserNum(), 64, 64);

	//������Ϸ���ĸ����Ļ����ߴ紴����Ϸ����
	BLUEDWORD dwBgWidth, dwBgHeight;
	m_resManager.GetSfSrcBg().GetBaseDesc(dwBgWidth, dwBgHeight);
	dwBgWidth++;
	dwBgHeight++;
	//����������1����Ȼvista��win7�������⣨�в�֪����ʲô���⣩
	dd7.CreateGeneralSurface(dwBgWidth, dwBgHeight, 0, &m_surfaceGameCoreCanvas);
	dd7.CreateGeneralSurface(dwBgWidth, dwBgHeight, 0, &m_surfaceCanvas);
	m_surfaceGameCoreCanvas.SetSrcColorKey(0x00FF00FF);
	m_surfaceCanvas.SetSrcColorKey(0x00FF00FF);
	m_surfaceGameCoreCanvas.ClipOwn(dd7);
	m_surfaceCanvas.ClipOwn(dd7);

	m_drawRc.left = 0;
	m_drawRc.top = 0;
	m_drawRc.right = dwBgWidth;
	m_drawRc.bottom = dwBgHeight;

	//���������õı���
	dd7.CreateGeneralSurface(300, 300, 0, &m_surfaceShoot);
	m_surfaceShoot.SetSrcColorKey(0);

	//������������õı���
	dd7.CreateGeneralSurface(200, 30, 0, &m_surfaceUserName);
	m_surfaceUserName.SetSrcColorKey(0x00FF00FF);
	m_surfaceUserName.FillColor(0x00FF00FF);

	//��ʼ�·���ǹ��ת��90��
	m_pGameCore->WheelEmitter(90);
}

CCrazyBubbleGame::~CCrazyBubbleGame(void)
{
	delete m_pGameCore;
	delete m_pRandom;
	if (m_pUser)
		delete m_pUser;
}

bool CCrazyBubbleGame::SitUser(const BLUEString& strName, const int nID)
{
	BLUEASSERT(nID >= 0);

	if (m_pUser == BLUENULL)
	{
		m_pUser = new CClientUser();
		m_nIDInRoom = nID;

		//����û���
		HDC hDC;
		HFONT hFont = ::CreateFont(25, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("����"));
		m_surfaceUserName->GetDC(&hDC);
		HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
		::SetTextColor(hDC, 0x00FF0000);
		::SetBkColor(hDC, 0x00FF00FF);
		DWORD dwWidth, dwHeight;
		m_surfaceUserName.GetBaseDesc(dwWidth, dwHeight);
		RECT rc = {0, 0, dwWidth, dwHeight};
		::DrawText(hDC, strName.c_str(), (int)strName.length(), &rc, DT_CENTER);
		::SelectObject(hDC, hFont);
		m_surfaceUserName->ReleaseDC(hDC);
		::DeleteObject(hFont);


		m_pGameCore->CoerceRefresh();
		return true;
	}

	return false;
}

bool CCrazyBubbleGame::LeaveUser(void)
{
	if (m_pUser)
	{
		StopGame();
		m_nIDInRoom = -1;
		delete m_pUser;
		m_pUser = BLUENULL;
		//�����û���
		m_surfaceUserName.FillColor(0x00FF00FF);
		m_pGameCore->CoerceRefresh();
		return true;
	}

	return false;
}

CDirectDrawSurface7& CCrazyBubbleGame::RunOne(void)
{
	BLUEASSERT(m_pGameCore);
	BLUEASSERT(m_pListener);

	//�������ж���
	ProcessActionQueue();

	if (!m_shootTimer.RunOnce())
		m_pListener->OnCBGMEventShootTimerOver(m_pCallBackParam);

	//��Ϸ��������һ��
	m_pGameCore->RunOnce();

	//����
	if (m_aniManager.HasAni())
	{
		//����ж�����ʹ�����㻺�����
		m_surfaceCanvas->Blt(NULL, m_surfaceGameCoreCanvas, NULL, DDBLT_WAIT, NULL);
		m_aniManager.ActorAllAni(m_surfaceCanvas);
		return m_surfaceCanvas;
	}

	//û�ж�����ֱ��������ӿ��ٶ�
	return m_surfaceGameCoreCanvas;
}

bool CCrazyBubbleGame::StartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate)
{
	//���û����ڸ���Ϸ���ϲ���Ч
	if (m_pUser)
	{
		//action�����ÿ�
		while (!m_actionQueue.empty())
			m_actionQueue.pop();

		//��������һ���������
		delete m_pRandom;
		m_pRandom = new CBLUERandom(nSeed);//�����������ӣ���Ȼ������Ӳ�һ������ɵ��߲�ͬ

		//���������
		m_toolManager.Clear();

		//�Ƴ���������
		m_aniManager.Clear();

		//���߳��ָ���
		m_nBringToolRate = nBringToolRate;
		
		//������������ɫ
		m_nUnknowBubbleColorNum = 0;
		
		//������Ϸ����
		BLUEASSERT(m_pGameCore);
		m_pGameCore->Reset(nSeed, dwInitRows);

		//�����ʱ������
		m_shootTimer.Reset();

		//������ʾ��ID��
		m_spriteUserNum.SetFrame(m_nExternUseID);

		//����������90��
		m_pGameCore->WheelEmitter(90);

		//ǿ��ˢ��һ��
		m_pGameCore->CoerceRefresh();

		return true;
	}
	return false;
}

bool CCrazyBubbleGame::StopGame(void)
{


	return true;
}

void CCrazyBubbleGame::UseTool(void)
{
	BLUEASSERT(m_pGameCore);
	BLUEASSERT(m_pListener);
	if (!m_toolManager.IsUsingTool())
	{
		//ʹ������
		BLUEDWORD dwBadBubbleNumber = m_toolManager.RemoveThrowBubble();
		if (dwBadBubbleNumber > 0)
		{
			m_pListener->OnCBGMEventRemoveTool(m_pCallBackParam);
			m_pListener->OnCBGMEventUsingToolForAddBadBubble(m_pCallBackParam, dwBadBubbleNumber);
			m_pGameCore->CoerceRefresh();
		}
	}
	else
	{
		//ʹ�õ���
		if (m_toolManager.IsHasTool())//�����е��߿�ʹ��
		{
			CCrazyBubbleGameToolManager::TOOL tool = m_toolManager.RemoveTool();
			m_pListener->OnCBGMEventRemoveTool(m_pCallBackParam);
			switch (tool)
			{
			case CCrazyBubbleGameToolManager::linebadbubble:
				m_pListener->OnCBGMEventUsingToolForLineBadBubble(m_pCallBackParam);
				break;
			case CCrazyBubbleGameToolManager::addonelinebadbubble:
				m_pListener->OnCBGMEventUsingToolForAddOneLineBadBubble(m_pCallBackParam);
				break;
			case CCrazyBubbleGameToolManager::subonelinebaddbubble:
				m_pListener->OnCBGMEventUsingToolForSubOneLineBadBubble(m_pCallBackParam);
				break;
			case CCrazyBubbleGameToolManager::unknowbubblecolor:
				m_pListener->OnCBGMEventUsingToolForUnknowBubbleColor(m_pCallBackParam);
				break;
			case CCrazyBubbleGameToolManager::throwshit:
				m_pListener->OnCBGMEventUsingToolForThrowShit(m_pCallBackParam);
				break;
			default:
				BLUEASSERT(0);
			}
			m_pGameCore->CoerceRefresh();
		}
	}
}

void CCrazyBubbleGame::ActionShootBubble(const int nAngle)
{
	//��ӷ������ݶ���
	GAMEACTION action;
	action.actionType = shootbubble;
	action.pf = nAngle;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionShootBadBubble(const BLUEDWORD dwNumber)
{
	//��ӷ��仵���ݶ���
	GAMEACTION action;
	action.actionType = shootbadbubble;
	action.pf = (int)dwNumber;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionEraseBubble(const BLUEDWORD dwNumber)
{
	//����������ݶ���
	GAMEACTION action;
	action.actionType = erasebubble;
	action.pf = (int)dwNumber;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionAddLineBadBubble(void)
{
	//����������ݶ���
	GAMEACTION action;
	action.actionType = addlinebadbubble;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionChangeToolMode( void )
{
	//��Ӹ�������ģʽ����
	GAMEACTION action;
	action.actionType = changetoolmode;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionRemoveTool(void)
{
	//����Ƴ����߶���
	GAMEACTION action;
	action.actionType = removetool;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionUnknowBubbleColor(void)
{
	//�������������ɫ����
	GAMEACTION action;
	action.actionType = unknowbubblecolor;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionThrowShit(void)
{
	//����ӱ�㶯��
	GAMEACTION action;
	action.actionType = throwshit;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionAdjustToolOrder(void)
{
	//����л����߶���
	GAMEACTION action;
	action.actionType = adjusttoolorder;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::CBGCGameOver(void)
{
	//������Ϸ��������ʧ�ܣ��¼�
	BLUEASSERT(m_pListener);
	m_surfaceGameCoreCanvas.ChangeToGrayMode();
	m_pListener->OnCBGMEventGameOver(m_pCallBackParam);
}

void CCrazyBubbleGame::CBGCBubbleCreate(CBubble* pBubble)
{
	BLUEASSERT(pBubble);
	BLUEASSERT(m_pRandom);

	int nCustomData = m_pRandom->NextInt(100);
	if (nCustomData >= m_nBringToolRate)
		nCustomData = -1;
	else
		nCustomData = m_pRandom->NextInt(5);
	pBubble->SetCustomData(nCustomData);
}

void CCrazyBubbleGame::CBGCStartDraw(void)
{
	m_surfaceGameCoreCanvas.FillColor(0x00FF00FF);
	m_resManager.GetSfSrcBg().Blt(m_surfaceGameCoreCanvas);
}

void CCrazyBubbleGame::CBGCEndDraw(void)
{
	//���Ʒ���ǹ
	m_surfaceShoot.Blt(
		m_surfaceGameCoreCanvas,
		m_pGameCore->GetWidth() / 2 - 143 + CBG_GAMEAREATOLEFT,
		m_pGameCore->GetHeight() + CBG_GAMEAREATOTOP - 150 + 60
		);

	//���Ƶ�ǰ��������
	if (m_nUnknowBubbleColorNum == 0)
	{
		m_spriteBubble.SetFrame(m_pGameCore->GetCurrentShootBubbleColor());
		m_spriteBubble.Paint(m_surfaceGameCoreCanvas, 142, m_pGameCore->GetHeight() + 45 + CBG_GAMEAREATOTOP);
		if (m_pGameCore->GetCurrentShootBubbleCustomData() >= 0)
		{
			m_spriteTool.SetFrame(m_pGameCore->GetCurrentShootBubbleCustomData());
			m_spriteTool.Paint(m_surfaceGameCoreCanvas, 142, m_pGameCore->GetHeight() + 45 + CBG_GAMEAREATOTOP);
		}
	}
	else
	{
		m_spriteBubble.SetFrame(8);
		m_spriteBubble.Paint(m_surfaceGameCoreCanvas, 142, m_pGameCore->GetHeight() + 45 + CBG_GAMEAREATOTOP);
	}

	//������һ����������
	if (m_nUnknowBubbleColorNum == 0)
	{
		m_spriteBubble.SetFrame(m_pGameCore->GetNextShootBubbleColor());
		m_spriteBubble.Paint(m_surfaceGameCoreCanvas, 90, m_pGameCore->GetHeight() + 45 + CBG_GAMEAREATOTOP);
		if (m_pGameCore->GetNextShootBubbleCustomData() >= 0)
		{
			m_spriteTool.SetFrame(m_pGameCore->GetNextShootBubbleCustomData());
			m_spriteTool.Paint(m_surfaceGameCoreCanvas, 90, m_pGameCore->GetHeight() + 45 + CBG_GAMEAREATOTOP);
		}
	}
	else
	{
		m_spriteBubble.SetFrame(8);
		m_spriteBubble.Paint(m_surfaceGameCoreCanvas, 90, m_pGameCore->GetHeight() + 45 + CBG_GAMEAREATOTOP);
	}

	//���ƹ�����
	m_toolManager.GetCanvas().Blt(
		m_surfaceGameCoreCanvas,
		CBG_TOOLAREATOLEFT,
		CBG_TOOLAREATOTOP
		);

	//������
	m_spriteLong.Paint(m_surfaceGameCoreCanvas, 190, 430);

	//�����������
	m_surfaceUserName.Blt(m_surfaceGameCoreCanvas, 30, 25);

	//��ʾ�û����
	m_spriteUserNum.Paint(m_surfaceGameCoreCanvas, 30, 8);

	//���û���û�������Ϸ���ϣ�����ʾһ�Ż�ͼ
	if (m_pUser == BLUENULL)
		m_resManager.GetSfSrcBgDisable().Blt(m_surfaceGameCoreCanvas);
}

void CCrazyBubbleGame::CBGCDrawBubble(const CBubble* pBubble)
{
	BLUEASSERT(pBubble);
	BLUEASSERT(m_pGameCore);

	//�������ݵ�����
	m_spriteBubble.SetFrame(pBubble->GetColor());
	m_spriteBubble.Paint(
		m_surfaceGameCoreCanvas,
		pBubble->GetPosX() + CBG_GAMEAREATOLEFT,
		pBubble->GetPosY() + CBG_GAMEAREATOTOP
		);

	if (pBubble->GetCurstomData() >= 0)
	{
		m_spriteTool.SetFrame(pBubble->GetCurstomData());
		m_spriteTool.Paint(
			m_surfaceGameCoreCanvas,
			pBubble->GetPosX() + CBG_GAMEAREATOLEFT,
			pBubble->GetPosY() + CBG_GAMEAREATOTOP
			);
	}
}

void CCrazyBubbleGame::CBGCDrawCurrentShootBubble(const CBubble* pBubble)
{
	BLUEASSERT(pBubble);
}

void CCrazyBubbleGame::CBGCDrawNextShootBubble(const CBubble* pBubble)
{
	BLUEASSERT(pBubble);
}

void CCrazyBubbleGame::CBGCWheelEmitter(const int nAngle)
{
	DDSURFACEDESC2 ddsdDest, ddsdSrc;

	E2D_INIT_STRUCT(ddsdDest);
	E2D_INIT_STRUCT(ddsdSrc);
	DWORD dwSrcWidth, dwSrcHeight, dwSrcBitCount, dwDestBitCount;
	DWORD dwSrcSurfaceBytesOfOnePixel, dwDestSurfaceBytesOfOnePixel;
	m_surfaceShoot.GetBitCount(dwDestBitCount);
	m_resManager.GetSfSrcShoot().GetBaseDesc(dwSrcWidth, dwSrcHeight, dwSrcBitCount);
	dwSrcSurfaceBytesOfOnePixel = ::E2DGetPixelNumberOfBytesFromBit(dwSrcBitCount);
	dwDestSurfaceBytesOfOnePixel = ::E2DGetPixelNumberOfBytesFromBit(dwDestBitCount);

	m_surfaceShoot.FillColor(0);
	m_surfaceShoot->Lock(NULL, &ddsdDest, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	m_resManager.GetSfSrcShoot()->Lock(NULL, &ddsdSrc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	for (DWORD x = 0; x < dwSrcWidth; x++)
	{
		for (DWORD y = 0; y < dwSrcHeight; y++)
		{
			int rx = (int)(150.0f + ((float)x - 33.0f) * ::GetCosValue(-nAngle) - ((float)y - 37.0f) * ::GetSinValue(-nAngle));//GetRotX(150, 33, 37, x, y, -nAngle);
			int ry = (int)(150.0f + ((float)y - 37.0f) * ::GetCosValue(-nAngle) + ((float)x - 33.0f) * ::GetSinValue(-nAngle));//GetRotY(150, 33, 37, x, y, -nAngle);

			::E2DSetSurfacePixel(
				rx, ry, ddsdDest.lPitch, (char*)ddsdDest.lpSurface, dwDestSurfaceBytesOfOnePixel,
				::E2DGetSurfacePixel(x, y, ddsdSrc.lPitch, (char*)ddsdSrc.lpSurface, dwSrcSurfaceBytesOfOnePixel)
				);
		}
	}
	m_resManager.GetSfSrcShoot()->Unlock(NULL);
	m_surfaceShoot->Unlock(NULL);
}

void CCrazyBubbleGame::CBGCBubbleShoot(const CBubble* pBubble)
{
	if (m_bPlaySound)
	{
		//BLUEASSERT(m_pAudioProcessor);
		//m_pAudioProcessor->PlayEffect(MUSICLINE_BUBBLESHOOT);
	}
}

void CCrazyBubbleGame::CBGCBubbleFix(const CBubble* pBubble)
{
	if (m_bPlaySound)
	{
		//BLUEASSERT(m_pAudioProcessor);
		//m_pAudioProcessor->PlayEffect(MUSICLINE_BUBBLEFIX);
	}
}

void CCrazyBubbleGame::CBGCBubbleKickWall(const CBubble* pBubble)
{
	if (m_bPlaySound)
	{
		//BLUEASSERT(m_pAudioProcessor);
		//m_pAudioProcessor->PlayEffect(MUSICLINE_BUBBLEKICKWALL);
	}
}

void CCrazyBubbleGame::CBGCBubbleDropStart(void)
{
	if (m_bPlaySound)
	{
		//BLUEASSERT(m_pAudioProcessor);
		//m_pAudioProcessor->PlayEffect(MUSICLINE_BUBBLEDROP);
	}
}

void CCrazyBubbleGame::CBGCBubbleDrop(const CBubble* pBubble)
{
	if (pBubble->GetCurstomData() != -1)
		m_toolManager.AddTool((CCrazyBubbleGameToolManager::TOOL)pBubble->GetCurstomData());
}

void CCrazyBubbleGame::CBGCBubbleDropEnd(const BLUEDWORD dwDropNumber)
{
	m_toolManager.AddThrowBubble(dwDropNumber);
}

void CCrazyBubbleGame::CBGCBubbleEraseStart(void)
{
	if (m_bPlaySound)
	{
		//BLUEASSERT(m_pAudioProcessor);
		//m_pAudioProcessor->PlayEffect(MUSICLINE_BUBBLEERASE);
	}
}

void CCrazyBubbleGame::CBGCBubbleErase(const CBubble* pBubble)
{
	m_aniManager.AddBomb(
		pBubble->GetPosX() + CBG_GAMEAREATOLEFT,
		pBubble->GetPosY() + CBG_GAMEAREATOTOP
		);
	if (pBubble->GetCurstomData() != -1)
		m_toolManager.AddTool((CCrazyBubbleGameToolManager::TOOL)pBubble->GetCurstomData());

}

void CCrazyBubbleGame::CBGCBubbleEraseEnd(const BLUEDWORD dwEraseNumber)
{
	m_toolManager.AddThrowBubble(dwEraseNumber - 3);

}

void CCrazyBubbleGame::ProcessActionQueue(void)
{
	//������������
	if (!m_actionQueue.empty())
	{
		//ȡ���׶���
		GAMEACTION& action = m_actionQueue.front();
		bool bDoActionResult = false;
		switch (action.actionType)
		{
		case shootbubble:
			{
				int nAngle = action.pf;
				if (GetCurrentShootAngle() != nAngle)
					WheelEmitter(nAngle);
				bDoActionResult = ShootBubble();
			}
			break;

		case shootbadbubble:
			bDoActionResult = ShootBadBubble((BLUEDWORD)action.pf);
			break;

		case erasebubble:
			bDoActionResult = EraseBubble((BLUEDWORD)action.pf);
			break;

		case addlinebadbubble:
			bDoActionResult = AddLineBadBubble();
			break;

		case removetool:
			bDoActionResult = true;
			UseTool();
			break;

		case changetoolmode:
			bDoActionResult = true;
			ChangeToolMode();
			break;

		case unknowbubblecolor:
			bDoActionResult = true;
			UnknowBubbleColor();
			break;

		case throwshit:
			bDoActionResult = true;
			ThrowShit();
			break;

		case adjusttoolorder:
			bDoActionResult = true;
			AdjustToolOrder();
			break;

		}

		//���ִ�гɹ����򵯳��˴ζ�����������һ�ض���
		if (bDoActionResult)
			m_actionQueue.pop();
	}

}