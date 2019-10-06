#include "StdAfx.h"
#include "CBCGUIProcessor.h"

CCBCGUIProcessor::CCBCGUIProcessor(ICBCGUIProcessorListener* pListener, const CDirectDraw7& dd7, const CDirectDrawSurface7& ddsCanvas) :
m_pListener(pListener),
m_dd7(dd7),
m_ddsCanvas(ddsCanvas),
m_pCurrentGUIManager(BLUENULL),
m_pLogoGUIManager(BLUENULL),
m_pLoginGUIManager(BLUENULL),
m_pRoomListGUIManager(BLUENULL),
m_pGameGUIManager(BLUENULL),
m_pGUIModelDialog(BLUENULL),
m_pGUIBtnOK(BLUENULL),
m_pGUIModelDlgImg(BLUENULL)
{
	BLUEASSERT(m_pListener);
	::DXG2DSetGlobalParam(dd7, ddsCanvas);
}

CCBCGUIProcessor::~CCBCGUIProcessor(void)
{
	Destroy();
}

bool CCBCGUIProcessor::Create(void)
{
	//创建模式对话框资源
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\okBtn.bmp"), 0, &m_surfaceOKBtn);
	m_surfaceOKBtn.SetSrcColorKey(0x00FF00FF);
	m_spriteOKBtn.Create(m_surfaceOKBtn, 127, 42);

	//创建背景资源
	if (FAILED(m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\background.bmp"), 0, &m_surfaceBackground)))
		return false;
	//创建两个背景精灵
	if (!m_spriteBg_1.Create(m_surfaceBackground))
		return false;
	if (!m_spriteBg_2.Create(m_surfaceBackground))
		return false;
	//第二个背景精灵初始位置设置，让两个精灵位置相连
	m_spriteBg_2.SetPosX(m_spriteBg_1.GetCellWidth());

	//切换到Logo界面
	return ChangeUIType(logo);
}

void CCBCGUIProcessor::Destroy(void)
{
	DestroyGameUI();
	DestroyRoomListUI();
	DestroyLoginUI();
	DestroyLogoUI();
	m_pCurrentGUIManager = BLUENULL;
	m_spriteBg_2.Destroy();
	m_spriteBg_1.Destroy();
	if (m_pGUIBtnOK)
	{
		delete m_pGUIBtnOK;
		m_pGUIBtnOK = BLUENULL;
	}
	if (m_pGUIModelDlgImg)
	{
		delete m_pGUIModelDlgImg;
		m_pGUIModelDlgImg = BLUENULL;
	}
	if (m_pGUIModelDialog)
	{
		delete m_pGUIModelDialog;
		m_pGUIModelDialog = BLUENULL;
	}
}

void CCBCGUIProcessor::AddModelDialog(
									  const CBLUERect& rcDlg,
									  const CBLUEPoint& ptOKBtnPos,
									  const CBLUERect& rcImg,
									  const CBLUERect& rcText,
									  const BLUEString& strText,
									  const CDirectDrawSurface7* pSurfaceImg
									  )
{
	BLUEASSERT(m_pCurrentGUIManager);
	if (m_pGUIModelDialog) {m_pGUIModelDialog->RemoveAllChildWinow();delete m_pGUIModelDialog;}
	m_pGUIModelDialog = new CDXGWindow2D;
	m_pCurrentGUIManager->AddTopWindow(m_pGUIModelDialog, rcDlg, true);
	m_pGUIModelDialog->SetBKSrcColorKey(0x00FF00FF);
	m_pGUIModelDialog->FillBackColor(0x00FF00FF);

	if (pSurfaceImg)
	{
		if (m_pGUIModelDlgImg) delete m_pGUIModelDlgImg;
		m_pGUIModelDlgImg = new CDXGImage2D(*pSurfaceImg);
		m_pGUIModelDialog->AddChildWindow(m_pGUIModelDlgImg, rcImg);
	}

	if (!strText.empty())
	{
		m_pGUIModelDialog->AddChildWindow(&m_guiModelDlgStatic, rcText);
		m_guiModelDlgStatic.SetText(strText);
	}

	if (m_pGUIBtnOK) delete m_pGUIBtnOK;
	m_pGUIBtnOK = new CDXGButton2D(m_spriteOKBtn);
	m_pGUIBtnOK->SetListener(this);
	m_pGUIModelDialog->AddChildWindow(m_pGUIBtnOK, CBLUERect(ptOKBtnPos, CBLUESize(127, 42)));

}

bool CCBCGUIProcessor::ChangeUIType(UITYPE uiType)
{
	switch (uiType)
	{
	case logo:
		if (!CreateLogoUI()) return false;
		m_pCurrentGUIManager = m_pLogoGUIManager;
		break;
	case login:
		if (!CreateLoginUI()) return false;
		m_pCurrentGUIManager = m_pLoginGUIManager;
		break;
	case roomlist:
		if (!CreateRoomListUI()) return false;
		m_pCurrentGUIManager = m_pRoomListGUIManager;
		break;
	case game:
		if (!CreateGameUI()) return false;
		m_pCurrentGUIManager = m_pGameGUIManager;
		break;
	default:
		return false;
	}

	m_uiType = uiType;
	BLUEASSERT(m_pListener);
	m_pListener->OnGUIPChangeUIType(m_uiType);
	return true;
}

void CCBCGUIProcessor::OnDXGCommand(CDXGWindow* pWindow, unsigned int nEvent, int nDetail)
{
	BLUEASSERT(m_pListener);
	BLUEASSERT(pWindow);

	if (pWindow == m_pGUIBtnLogin)
	{
		BLUEASSERT(m_pGUIEditIP);
		BLUEASSERT(m_pGUIEditUserName);
		BLUEASSERT(m_pGUIEditPassWord);
		m_pListener->OnGUIPLConnectServer(
			m_pGUIEditIP->GetText(), 8080,
			m_pGUIEditUserName->GetText(),
			m_pGUIEditPassWord->GetText());
	}
	else if (pWindow == m_pGUIBtnPerpareGame)
	{
		m_pListener->OnGUIPLPrepareGame();
	}
	else if (pWindow == m_pGUIBtnQuitRoom)
	{
		m_pListener->OnGUIPLQuitRoom();
	}
	else if (pWindow == m_pGUIListRoomList)
	{
		m_pListener->OnGUIPLEnterRoom(nDetail);
	}
	else if (pWindow == m_pGUIBtnPrevRLPageBtn)
	{
		m_pListener->OnGUIPLPrevRLPage();
	}
	else if (pWindow == m_pGUIBtnNextRLPageBtn)
	{
		m_pListener->OnGUIPLNextRLPage();
	}
	else if (pWindow == m_pGUIBtnOK)
	{
		m_pGUIModelDialog->RemoveAllChildWinow();
		m_pCurrentGUIManager->RemoveTopWindow(m_pGUIModelDialog);
	}
	else if (pWindow == m_pGUIBtnHelp)
	{
		m_pListener->OnGUIPLHelp();
	}
	else if (pWindow == m_pGUIBtnAbout)
	{
		m_pListener->OnGUIPLAbout();
	}
}

bool CCBCGUIProcessor::CreateLogoUI(void)
{
	if (m_pLogoGUIManager)
		return true;

	m_pLogoGUIManager = new CDXGManager();
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\logo.bmp"), 0, &m_surfaceLogo);
	return true;
}

bool CCBCGUIProcessor::CreateLoginUI(void)
{
	if (m_pLoginGUIManager)
		return true;

	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\loginBack.bmp"), 0, &m_surfaceLoginBack);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\loginBtn.bmp"), 0, &m_surfaceLoginBtn);
	m_surfaceLoginBack.SetSrcColorKey(0x0000FFFF);
	m_surfaceLoginBtn.SetSrcColorKey(0x00FF00FF);
	m_spriteLoginBtn.Create(m_surfaceLoginBtn, 88, 88);

	m_pLoginGUIManager = new CDXGManager;
	m_pGUIImgLoginBack = new CDXGImage2D(m_surfaceLoginBack);
	m_pGUIEditIP = new CDXGEdit2D;
	m_pGUIEditUserName = new CDXGEdit2D;
	m_pGUIEditPassWord = new CDXGEdit2D;
	m_pGUIBtnLogin = new CDXGButton2D(m_spriteLoginBtn);

	m_pGUIBtnLogin->SetListener(this);

	m_pLoginGUIManager->AddTopWindow(m_pGUIImgLoginBack, CBLUERect(CBLUEPoint(50, 100), CBLUESize(640, 337)));
	m_pLoginGUIManager->AddTopWindow(m_pGUIEditUserName, CBLUERect(CBLUEPoint(512, 211), CBLUESize(160, 20)));
	m_pLoginGUIManager->AddTopWindow(m_pGUIEditPassWord, CBLUERect(CBLUEPoint(512, 259), CBLUESize(160, 20)));
	m_pLoginGUIManager->AddTopWindow(m_pGUIEditIP, CBLUERect(CBLUEPoint(512, 307), CBLUESize(160, 20)));
	m_pLoginGUIManager->AddTopWindow(m_pGUIBtnLogin, CBLUERect(CBLUEPoint(400, 360), CBLUESize(88, 88)));

	m_pGUIEditUserName->SetBackColor(0xFFFF00);
	m_pGUIEditPassWord->SetBackColor(0xFFFF00);
	m_pGUIEditIP->SetBackColor(0xFFFF00);
	m_pGUIEditPassWord->SetStyle(true);

	m_pGUIEditIP->SetText(_BLUET("127.0.0.1"));
	m_pGUIEditIP->SetCursorPosEnd();
	return true;
}

bool CCBCGUIProcessor::CreateRoomListUI(void)
{
	if (m_pRoomListGUIManager)
		return true;

	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\rlbg.bmp"), 0, &m_surfaceRLBg);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\prevRLPageBtn.bmp"), 0, &m_surfacePrevRLPageBtn);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\nextRLPageBtn.bmp"), 0, &m_surfaceNextRLPageBtn);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\roomBtn.bmp"), 0, &m_surfaceEnterRoomBtn);

	m_surfaceRLBg.SetSrcColorKey(0x00FF00FF);
	m_surfacePrevRLPageBtn.SetSrcColorKey(0X00FF00FF);
	m_surfaceNextRLPageBtn.SetSrcColorKey(0X00FF00FF);
	m_surfaceEnterRoomBtn.SetSrcColorKey(0X00FF00FF);

	m_spritePrevRLPageBtn.Create(m_surfacePrevRLPageBtn, 88, 88);
	m_spriteNextRLPageBtn.Create(m_surfaceNextRLPageBtn, 88, 88);

	m_pRoomListGUIManager = new CDXGManager;
	m_pGUIImgRLBg = new CDXGImage2D(m_surfaceRLBg);
	m_pGUIBtnPrevRLPageBtn = new CDXGButton2D(m_spritePrevRLPageBtn);
	m_pGUIBtnNextRLPageBtn = new CDXGButton2D(m_spriteNextRLPageBtn);
	m_pGUIListRoomList = new CDXGList2D(m_surfaceEnterRoomBtn, 305, 60, 15, 30);

	m_pGUIBtnPrevRLPageBtn->SetListener(this);
	m_pGUIBtnNextRLPageBtn->SetListener(this);
	m_pGUIListRoomList->SetListener(this);

	m_pGUIListRoomList->SetElementVSpace(30);

	m_pRoomListGUIManager->AddTopWindow(m_pGUIImgRLBg, CBLUERect(CBLUEPoint(50, 20), CBLUESize(746, 493)));
	m_pRoomListGUIManager->AddTopWindow(m_pGUIBtnPrevRLPageBtn, CBLUERect(CBLUEPoint(370, 470), CBLUESize(88, 88)));
	m_pRoomListGUIManager->AddTopWindow(m_pGUIBtnNextRLPageBtn, CBLUERect(CBLUEPoint(560, 470), CBLUESize(88, 88)));
	m_pRoomListGUIManager->AddTopWindow(m_pGUIListRoomList, CBLUERect(CBLUEPoint(420, 100), CBLUESize(400, 400)));

	return true;
}

bool CCBCGUIProcessor::CreateGameUI(void)
{
	if (m_pGameGUIManager)
		return true;

	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\startBtn.bmp"), 0, &m_surfacePerpareGameBtn);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\quitRoomBtn.bmp"), 0, &m_surfaceQuitRoomBtn);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\helpBtn.bmp"), 0, &m_surfaceHelpBtn);
	m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\aboutBtn.bmp"), 0, &m_surfaceAboutBtn);
	m_surfacePerpareGameBtn.SetSrcColorKey(0x00FF00FF);
	m_surfaceQuitRoomBtn.SetSrcColorKey(0x00FF00FF);
	m_surfaceHelpBtn.SetSrcColorKey(0x00FF00FF);
	m_surfaceAboutBtn.SetSrcColorKey(0x00FF00FF);
	m_spritePerpareGameBtn.Create(m_surfacePerpareGameBtn, 127, 42);
	m_spriteQuitRoomBtn.Create(m_surfaceQuitRoomBtn, 127, 42);
	m_spriteHelpBtn.Create(m_surfaceHelpBtn, 127, 42);
	m_spriteAboutBtn.Create(m_surfaceAboutBtn, 127, 42);

	m_pGameGUIManager = new CDXGManager;
	m_pGUIBtnPerpareGame = new CDXGButton2D(m_spritePerpareGameBtn);
	m_pGUIBtnQuitRoom = new CDXGButton2D(m_spriteQuitRoomBtn);
	m_pGUIBtnHelp = new CDXGButton2D(m_spriteHelpBtn);
	m_pGUIBtnAbout = new CDXGButton2D(m_spriteAboutBtn);

	m_pGUIBtnPerpareGame->SetListener(this);
	m_pGUIBtnQuitRoom->SetListener(this);
	m_pGUIBtnHelp->SetListener(this);
	m_pGUIBtnAbout->SetListener(this);

	m_pGameGUIManager->AddTopWindow(m_pGUIBtnPerpareGame, CBLUERect(CBLUEPoint(400, 280), CBLUESize(127, 42)));
	m_pGameGUIManager->AddTopWindow(m_pGUIBtnQuitRoom, CBLUERect(CBLUEPoint(400, 340), CBLUESize(127, 42)));
	m_pGameGUIManager->AddTopWindow(m_pGUIBtnHelp, CBLUERect(CBLUEPoint(400, 400), CBLUESize(127, 42)));
	m_pGameGUIManager->AddTopWindow(m_pGUIBtnAbout, CBLUERect(CBLUEPoint(400, 460), CBLUESize(127, 42)));
	return true;
}

void CCBCGUIProcessor::DestroyLogoUI(void)
{
	if (m_pLogoGUIManager)
	{
		m_pLogoGUIManager->RemoveAllTopWindow();
		m_spriteLoginBtn.Destroy();
		delete m_pLogoGUIManager;
		m_pLogoGUIManager = BLUENULL;
	}
}

void CCBCGUIProcessor::DestroyLoginUI(void)
{
	if (m_pLoginGUIManager)
	{
		m_pLoginGUIManager->RemoveAllTopWindow();
		delete m_pGUIEditIP;
		delete m_pGUIEditUserName;
		delete m_pGUIEditPassWord;
		delete m_pGUIBtnLogin;
		delete m_pGUIImgLoginBack;
		delete m_pLoginGUIManager;
		m_pLoginGUIManager = BLUENULL;
	}
}

void CCBCGUIProcessor::DestroyRoomListUI(void)
{
	if (m_pRoomListGUIManager)
	{
		delete m_pGUIListRoomList;
		delete m_pGUIBtnNextRLPageBtn;
		delete m_pGUIBtnPrevRLPageBtn;
		delete m_pGUIImgRLBg;
		delete m_pRoomListGUIManager;
		m_pRoomListGUIManager = BLUENULL;
	}
}

void CCBCGUIProcessor::DestroyGameUI(void)
{
	if (m_pGameGUIManager)
	{
		delete m_pGUIBtnAbout;
		delete m_pGUIBtnHelp;
		delete m_pGUIBtnQuitRoom;
		delete m_pGUIBtnPerpareGame;
		delete m_pGameGUIManager;
		m_pGameGUIManager = BLUENULL;
	}
}

void CCBCGUIProcessor::FillBack(void)
{
	if (m_uiType == logo)
	{
		static bool bAdd = true;
		static int alphaValue = 0;

		if (bAdd)
			alphaValue += 8;
		else
			alphaValue -= 8;

		if (alphaValue > 100)
			bAdd = false;
		m_surfaceLogo.AlphaBlendBlt(m_ddsCanvas, 150, 100, NULL, alphaValue);

		if (!bAdd && alphaValue <= 0)
			ChangeUIType(login);
	}
	else
	{
		//绘制移动背景
		m_spriteBg_1.Paint(m_ddsCanvas);
		m_spriteBg_2.Paint(m_ddsCanvas);
		m_spriteBg_1.MoveX(-5);
		m_spriteBg_2.MoveX(-5);
		if (m_spriteBg_2.GetPosX() < 0)
		{
			m_spriteBg_1.SetPosX(0);
			m_spriteBg_2.SetPosX(m_spriteBg_1.GetCellWidth());
		}
	}
}

void CCBCGUIProcessor::ClearRoomListInfo(void)
{
	if (m_pRoomListGUIManager == BLUENULL)
		CreateRoomListUI();

	m_pGUIListRoomList->Clear();
}

void CCBCGUIProcessor::AddRoomInfo(
								   int nID,
								   const BLUEString& strRoomName,
								   int nCurClientNum,
								   int nMaxClientNum
								   )
{
	if (m_pRoomListGUIManager == BLUENULL)
		CreateRoomListUI();

	BLUEString strCaption(strRoomName);
	BLUETCHAR strNum[10];
	strCaption += _BLUET("　人数:(");
	::wsprintf(strNum, _BLUET("%d"), nCurClientNum);
	strCaption += strNum;
	strCaption += _BLUET("/");
	::wsprintf(strNum, _BLUET("%d"), nMaxClientNum);
	strCaption += strNum;
	strCaption += _BLUET(")");
	m_pGUIListRoomList->AddElement(
		strCaption,
		nID);
}