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

	//构造随机数器与游戏核心
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

	//根据游戏核心给出的画布尺寸创建游戏画布
	BLUEDWORD dwBgWidth, dwBgHeight;
	m_resManager.GetSfSrcBg().GetBaseDesc(dwBgWidth, dwBgHeight);
	dwBgWidth++;
	dwBgHeight++;
	//画布长宽都加1，不然vista和win7上有问题（尚不知道是什么问题）
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

	//发射器所用的表面
	dd7.CreateGeneralSurface(300, 300, 0, &m_surfaceShoot);
	m_surfaceShoot.SetSrcColorKey(0);

	//创建玩家名称用的表面
	dd7.CreateGeneralSurface(200, 30, 0, &m_surfaceUserName);
	m_surfaceUserName.SetSrcColorKey(0x00FF00FF);
	m_surfaceUserName.FillColor(0x00FF00FF);

	//初始下发射枪旋转至90度
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

		//填充用户名
		HDC hDC;
		HFONT hFont = ::CreateFont(25, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("宋体"));
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
		//擦除用户名
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

	//处理所有动作
	ProcessActionQueue();

	if (!m_shootTimer.RunOnce())
		m_pListener->OnCBGMEventShootTimerOver(m_pCallBackParam);

	//游戏核心运行一次
	m_pGameCore->RunOnce();

	//动画
	if (m_aniManager.HasAni())
	{
		//如果有动画才使用两层缓冲输出
		m_surfaceCanvas->Blt(NULL, m_surfaceGameCoreCanvas, NULL, DDBLT_WAIT, NULL);
		m_aniManager.ActorAllAni(m_surfaceCanvas);
		return m_surfaceCanvas;
	}

	//没有动画则直接输出，加快速度
	return m_surfaceGameCoreCanvas;
}

bool CCrazyBubbleGame::StartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate)
{
	//有用户座在该游戏机上才有效
	if (m_pUser)
	{
		//action队列置空
		while (!m_actionQueue.empty())
			m_actionQueue.pop();

		//重新申请一个随机数器
		delete m_pRandom;
		m_pRandom = new CBLUERandom(nSeed);//必须填入种子，不然大家种子不一样会造成道具不同

		//工具箱清空
		m_toolManager.Clear();

		//移除所有脏物
		m_aniManager.Clear();

		//道具出现概率
		m_nBringToolRate = nBringToolRate;
		
		//不隐藏泡泡颜色
		m_nUnknowBubbleColorNum = 0;
		
		//重置游戏核心
		BLUEASSERT(m_pGameCore);
		m_pGameCore->Reset(nSeed, dwInitRows);

		//发射计时器重置
		m_shootTimer.Reset();

		//设置显示的ID号
		m_spriteUserNum.SetFrame(m_nExternUseID);

		//发射重新置90度
		m_pGameCore->WheelEmitter(90);

		//强制刷新一次
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
		//使用泡泡
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
		//使用道具
		if (m_toolManager.IsHasTool())//必须有道具可使用
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
	//添加发射泡泡动作
	GAMEACTION action;
	action.actionType = shootbubble;
	action.pf = nAngle;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionShootBadBubble(const BLUEDWORD dwNumber)
{
	//添加发射坏泡泡动作
	GAMEACTION action;
	action.actionType = shootbadbubble;
	action.pf = (int)dwNumber;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionEraseBubble(const BLUEDWORD dwNumber)
{
	//添加消除泡泡动作
	GAMEACTION action;
	action.actionType = erasebubble;
	action.pf = (int)dwNumber;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionAddLineBadBubble(void)
{
	//添加消除泡泡动作
	GAMEACTION action;
	action.actionType = addlinebadbubble;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionChangeToolMode( void )
{
	//添加更换道具模式动作
	GAMEACTION action;
	action.actionType = changetoolmode;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionRemoveTool(void)
{
	//添加移除道具动作
	GAMEACTION action;
	action.actionType = removetool;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionUnknowBubbleColor(void)
{
	//添加隐藏泡泡颜色动作
	GAMEACTION action;
	action.actionType = unknowbubblecolor;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionThrowShit(void)
{
	//添加扔便便动作
	GAMEACTION action;
	action.actionType = throwshit;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::ActionAdjustToolOrder(void)
{
	//添加切换道具动作
	GAMEACTION action;
	action.actionType = adjusttoolorder;
	m_actionQueue.push(action);
}

void CCrazyBubbleGame::CBGCGameOver(void)
{
	//触发游戏机结束（失败）事件
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
	//绘制发射枪
	m_surfaceShoot.Blt(
		m_surfaceGameCoreCanvas,
		m_pGameCore->GetWidth() / 2 - 143 + CBG_GAMEAREATOLEFT,
		m_pGameCore->GetHeight() + CBG_GAMEAREATOTOP - 150 + 60
		);

	//绘制当前发射泡泡
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

	//绘制下一个发射泡泡
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

	//绘制工具条
	m_toolManager.GetCanvas().Blt(
		m_surfaceGameCoreCanvas,
		CBG_TOOLAREATOLEFT,
		CBG_TOOLAREATOTOP
		);

	//绘制龙
	m_spriteLong.Paint(m_surfaceGameCoreCanvas, 190, 430);

	//绘制玩家名称
	m_surfaceUserName.Blt(m_surfaceGameCoreCanvas, 30, 25);

	//显示用户编号
	m_spriteUserNum.Paint(m_surfaceGameCoreCanvas, 30, 8);

	//如果没有用户坐在游戏机上，则显示一张灰图
	if (m_pUser == BLUENULL)
		m_resManager.GetSfSrcBgDisable().Blt(m_surfaceGameCoreCanvas);
}

void CCrazyBubbleGame::CBGCDrawBubble(const CBubble* pBubble)
{
	BLUEASSERT(pBubble);
	BLUEASSERT(m_pGameCore);

	//绘制泡泡到画布
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
	//遍历动作队列
	if (!m_actionQueue.empty())
	{
		//取队首动作
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

		//如果执行成功，则弹出此次动作，继续下一回动作
		if (bDoActionResult)
			m_actionQueue.pop();
	}

}