#include "StdAfx.h"
#include "CBCGameRun.h"
#include "BLUESkin.h"

#define GAMEMACHINENUM                6           //游戏机数量
#define GAMEMACHINE_LEFTOFFSET  10         //游戏机绘制左偏移量
#define GAMEMACHINE_TOPOFFSET   10         //游戏机绘制上偏移量

CCBCGameRun::CCBCGameRun(void) :
m_state(logo),
m_nSelID(-1),
m_aniProcessor(m_resManager),
m_pGUIProcessor(BLUENULL),
m_pNWProcessor(BLUENULL),
m_pAudioProcessor(BLUENULL)
{
	//皮肤库初始化
	::BLUESkin_Initialize();


	//m_pAudioProcessor = new CCBCAudioProcessor;
	m_pAudioProcessor = NULL;
}

CCBCGameRun::~CCBCGameRun(void)
{
	/*
	if (m_pAudioProcessor)
		delete m_pAudioProcessor;
	*/

	if (m_pNWProcessor)
		delete m_pNWProcessor;

	if (m_pGUIProcessor)
		delete m_pGUIProcessor;

	for (GAMECOLL::size_type i = 0; i < m_gameColl.size(); i++)
			delete m_gameColl[i];

	//皮肤库析构
	::BLUESkin_UnBindNCSkin(GetMainWnd());
	::BLUESkin_Uninitialize();
}

HRESULT CCBCGameRun::GameInit(void)
{
	//去掉最大化按钮和边框可调节大小功能
	::SetWindowLong(
		GetMainWnd(),
		GWL_STYLE,
		::GetWindowLong(GetMainWnd(), GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX)
		);

	//加载皮肤
	::BLUESkin_BindNCSkin(GetMainWnd(), _BLUET("skin\\black"));
	::SetWindowPos(GetMainWnd(), NULL, 0, 0, 800, 600, SWP_NOMOVE);//不加上这句皮肤会无法立即显示
	
	//资源管理器初始
	if (!m_resManager.Load(m_dd7))
	{
		::MessageBox(GetMainWnd(), _BLUET("资源管理器初始化失败"), _BLUET("出错"), MB_OK);
		return E_FAIL;
	}

	//网络初始化
	m_pNWProcessor = new CCBCNetWorkProcessor(this);


	//初始六个游戏机
	for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
	{
		CCrazyBubbleGame* pGameMachine;
		pGameMachine = new CCrazyBubbleGame(
			this,
			m_dd7,
			m_resManager,
			m_pAudioProcessor
			);
		pGameMachine->SetCallBackParam((void*)i);
		m_gameColl.push_back(pGameMachine);
	}

	//初始六个游戏机的绘制坐标
	m_gameColl[0]->m_drawRc.left = GAMEMACHINE_LEFTOFFSET;
	m_gameColl[0]->m_drawRc.right += GAMEMACHINE_LEFTOFFSET;
	m_gameColl[0]->m_drawRc.top = GAMEMACHINE_TOPOFFSET;
	m_gameColl[0]->m_drawRc.bottom += GAMEMACHINE_TOPOFFSET;
	m_gameColl[0]->m_nExternUseID = 1;

	m_gameColl[1]->m_drawRc.left = m_gameColl[0]->m_drawRc.right + GAMEMACHINE_LEFTOFFSET;
	m_gameColl[1]->m_drawRc.right = (m_gameColl[1]->m_drawRc.right >> 1);
	m_gameColl[1]->m_drawRc.right += m_gameColl[1]->m_drawRc.left;
	m_gameColl[1]->m_drawRc.top = GAMEMACHINE_TOPOFFSET;
	m_gameColl[1]->m_drawRc.bottom = (m_gameColl[1]->m_drawRc.bottom >> 1);
	m_gameColl[1]->m_drawRc.bottom += m_gameColl[1]->m_drawRc.top;
	m_gameColl[1]->m_nExternUseID = 2;

	m_gameColl[2]->m_drawRc.left += m_gameColl[1]->m_drawRc.right + GAMEMACHINE_LEFTOFFSET;
	m_gameColl[2]->m_drawRc.right = (m_gameColl[2]->m_drawRc.right >> 1);
	m_gameColl[2]->m_drawRc.right += m_gameColl[2]->m_drawRc.left;
	m_gameColl[2]->m_drawRc.top += GAMEMACHINE_TOPOFFSET;
	m_gameColl[2]->m_drawRc.bottom = (m_gameColl[2]->m_drawRc.bottom >> 1);
	m_gameColl[2]->m_drawRc.bottom += m_gameColl[2]->m_drawRc.top;
	m_gameColl[2]->m_nExternUseID = 3;

	m_gameColl[3]->m_drawRc.left += m_gameColl[2]->m_drawRc.right + GAMEMACHINE_LEFTOFFSET;
	m_gameColl[3]->m_drawRc.right = (m_gameColl[3]->m_drawRc.right >> 1);
	m_gameColl[3]->m_drawRc.right += m_gameColl[3]->m_drawRc.left;
	m_gameColl[3]->m_drawRc.top += GAMEMACHINE_TOPOFFSET;
	m_gameColl[3]->m_drawRc.bottom = (m_gameColl[3]->m_drawRc.bottom >> 1);
	m_gameColl[3]->m_drawRc.bottom += m_gameColl[3]->m_drawRc.top;
	m_gameColl[3]->m_nExternUseID = 4;

	m_gameColl[4]->m_drawRc.left = m_gameColl[2]->m_drawRc.left;
	m_gameColl[4]->m_drawRc.right = m_gameColl[2]->m_drawRc.right;
	m_gameColl[4]->m_drawRc.top += m_gameColl[2]->m_drawRc.bottom + GAMEMACHINE_TOPOFFSET;
	m_gameColl[4]->m_drawRc.bottom = (m_gameColl[4]->m_drawRc.bottom >> 1);
	m_gameColl[4]->m_drawRc.bottom += m_gameColl[4]->m_drawRc.top;
	m_gameColl[4]->m_nExternUseID = 5;

	m_gameColl[5]->m_drawRc.left = m_gameColl[3]->m_drawRc.left;
	m_gameColl[5]->m_drawRc.right = m_gameColl[3]->m_drawRc.right;
	m_gameColl[5]->m_drawRc.top += m_gameColl[3]->m_drawRc.bottom + GAMEMACHINE_TOPOFFSET;
	m_gameColl[5]->m_drawRc.bottom = (m_gameColl[5]->m_drawRc.bottom >> 1);
	m_gameColl[5]->m_drawRc.bottom += m_gameColl[5]->m_drawRc.top;
	m_gameColl[5]->m_nExternUseID = 6;

	//用户UI初始化
	BLUEASSERT(m_pGUIProcessor == BLUENULL);
	m_pGUIProcessor = new CCBCGUIProcessor(this, m_dd7, m_ddsback);
	if (!m_pGUIProcessor->Create())
	{
		::MessageBox(GetMainWnd(), _BLUET("UI创建失败"), _BLUET("出错"), MB_OK);
		return E_FAIL;
	}
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::logo);


	//更新窗口标题文字
	UpdateWindowCaptionText();

	return S_OK;
}

HRESULT CCBCGameRun::GameMain(void)
{
	//网络处理
	NetWorkProcess();

	//按键等输入处理
	InputProcess();

	//渲染
	Render();

	return S_OK;
}

HRESULT CCBCGameRun::GameShutDown(void)
{
	//销毁音频处理器
	/*
	if (m_pAudioProcessor)
	{
		delete m_pAudioProcessor;
		m_pAudioProcessor = BLUENULL;
	}
	*/

	//销毁网络处理器
	if (m_pNWProcessor)
	{
		delete m_pNWProcessor;
		m_pNWProcessor = BLUENULL;
	}

	//销毁UI处理器
	if (m_pGUIProcessor)
	{
		m_pGUIProcessor->Destroy();
		delete m_pGUIProcessor;
		m_pGUIProcessor = BLUENULL;
	}

	return S_OK;
}


void CCBCGameRun::InputProcess(void)
{
	if (m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game)
	{
		BLUEASSERT(m_state == inroom);
		if (SUCCEEDED(m_diKeyboard.GSSafe()))
		{
			if (m_diKeyboard.IsKeyDown(DIK_LEFT))
				m_gameColl[GetID()]->WheelEmitterWiddershins();

			if (m_diKeyboard.IsKeyDown(DIK_RIGHT))
				m_gameColl[GetID()]->WheelEmitterDeasil();
		}
	}
}

void CCBCGameRun::Render(void)
{
	if (m_pNWProcessor->GetState() == CCBCNetWorkProcessor::disconnect)
		m_ddsback.FillColor(0);

	//UI界面先渲染游戏背景
	BLUEASSERT(m_pGUIProcessor);
	m_pGUIProcessor->FillBack();

	//如果在房间内则绘制游戏画面
	if (m_pNWProcessor->GetState() >= CCBCNetWorkProcessor::room)
	{
		for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
		{
			m_ddsback->Blt(
				&m_gameColl[i]->m_drawRc,
				m_gameColl[i]->RunOne(),
				NULL,
				DDBLT_WAIT | DDBLT_KEYSRC,
				NULL
				);
		}
	}

	//UI界面渲染
	m_pGUIProcessor->Render();

	//动画渲染
	m_aniProcessor.ActorAllAni(m_ddsback);

	//翻转画面
	Flip();
}

void CCBCGameRun::UpdateWindowCaptionText(void)
{
	BLUEASSERT(GetMainWnd());

	BLUEString strTitle(_BLUET("疯狂泡泡龙Online---["));
	switch (m_pNWProcessor->GetState())
	{
	case CCBCNetWorkProcessor::disconnect:
		strTitle += _BLUET("登录");
		break;
	case CCBCNetWorkProcessor::connect:
		strTitle += _BLUET("已登录");
		break;
	case CCBCNetWorkProcessor::login:
		strTitle += _BLUET("房间列表");
		break;
	case CCBCNetWorkProcessor::room:
		strTitle += _BLUET("房间内");
		break;
	case CCBCNetWorkProcessor::game:
		strTitle += _BLUET("游戏中");
		break;
	}
	strTitle += _BLUET("]");
	::SetWindowText(GetMainWnd(), strTitle.c_str());
}

int CCBCGameRun::GetIDFromExternID(const int nExternID)
{
	for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
		if (m_gameColl[i] && m_gameColl[i]->m_nExternUseID == nExternID)
			return (int)i;

	return -1;
}

void CCBCGameRun::PrevRLPage(void)
{
	//回退一页，再执行下一页
	BLUEASSERT(m_pGUIProcessor);
	int nBackRoll = m_pGUIProcessor->GetRoomListNum() + 4;
	int nID;
	for (int i = 0; i < nBackRoll; i++)
		m_rManager.GetPrevRoom(nID);

	NextRLPage();
}

void CCBCGameRun::NextRLPage(void)
{
	BLUEASSERT(m_pGUIProcessor);

	int nID;
	for (int i = 0; i < 4; i++)
	{
		if (CClientRoom* pRoom = m_rManager.GetNextRoom(nID))
		{
			if (i == 0)
				m_pGUIProcessor->ClearRoomListInfo();

			m_pGUIProcessor->AddRoomInfo(
				nID,
				pRoom->GetName(),
				pRoom->GetCurClientNum(),
				pRoom->GetMaxClientNum()
				);
		}
	}
}

void CCBCGameRun::AddStarAni(int nSrcID, int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	CBLUEPoint ptStar;
	CBLUEPoint ptEnd;
	ptStar.m_nX = m_gameColl[nSrcID]->m_drawRc.left + (m_gameColl[nSrcID]->m_drawRc.right - m_gameColl[nSrcID]->m_drawRc.left) / 2;
	ptStar.m_nY = m_gameColl[nSrcID]->m_drawRc.top + (m_gameColl[nSrcID]->m_drawRc.bottom - m_gameColl[nSrcID]->m_drawRc.top) / 2;
	ptEnd.m_nX = m_gameColl[nDestID]->m_drawRc.left + (m_gameColl[nDestID]->m_drawRc.right - m_gameColl[nDestID]->m_drawRc.left) / 2;
	ptEnd.m_nY = m_gameColl[nDestID]->m_drawRc.top + (m_gameColl[nDestID]->m_drawRc.bottom - m_gameColl[nDestID]->m_drawRc.top) / 2;;
	m_aniProcessor.AddStar(ptStar, ptEnd);
}

bool CCBCGameRun::OnPreMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//UI管理器对消息的处理
	if (m_pGUIProcessor)
		m_pGUIProcessor->ProcessMessage(uMsg, wParam, lParam);

	//游戏本身对消息的处理
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game)
		{
			if (wParam == VK_SPACE)//空格为发射泡泡
			{
				m_gameColl[GetID()]->ShootBubble();
			}
			else if (wParam >= _T('1') && wParam < _T('7'))//1~6为使用道具
			{
				m_nSelID = GetIDFromExternID((int)wParam - (int)_T('0'));
				m_gameColl[GetID()]->UseTool();
			}
			else if (wParam == 'A' || wParam == 'a')//a为切换道具与泡泡
			{
				m_gameColl[GetID()]->ChangeToolMode();
			}
			else if (wParam == 'S' || wParam == 's')//s为切换道具顺序
			{
				m_gameColl[GetID()]->AdjustToolOrder();
			}
		}
		break;
	}

	return true;
}

void CCBCGameRun::OnGUIPChangeUIType(int uiType)
{
	if (uiType == CCBCGUIProcessor::login)
	{
		CDirectDrawSurface7 surfaceWait;
		if (SUCCEEDED(m_dd7.CreateImageSurfaceFromFile(_BLUET("res\\wait.bmp"), 0, &surfaceWait)))
		{
			m_ddsback.FillColor(0);
			surfaceWait.Blt(m_ddsback, 30, 200);
			Flip();
		}
		//音频初始化
		/*
		m_pAudioProcessor->Create(GetMainWnd());
		m_pAudioProcessor->PlayBackground(MUSICLINE_BK_NORMAL);
		*/

		::SetWindowPos(GetMainWnd(), NULL, 0, 0, 800, 600, SWP_NOMOVE);
	}
	else if (uiType == CCBCGUIProcessor::roomlist)
	{
		::SetWindowPos(GetMainWnd(), NULL, 0, 0, 870, 630, SWP_NOMOVE);
	}
	else if (uiType == CCBCGUIProcessor::game)
	{
		::SetWindowPos(GetMainWnd(), NULL, 0, 0, 950, 580, SWP_NOMOVE);
	}
}

void CCBCGameRun::OnGUIPLConnectServer(const BLUEString& strIP, BLUEWORD nPort, const BLUEString& strUserName, const BLUEString& strPassWord)
{
	//如果是UNICODE字符串则先转成ANSI
	BLUEStringA strServerIP;
#ifdef UNICODE
	::BLUEW2A(strIP, strServerIP);
#else
	strServerIP = strIP;
#endif

	//用户名不能为空
	if (strUserName.empty())
	{
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("用户名不能为空"),
			&m_resManager.GetSfSrcModelDlgBk());
		return;
	}

	//IP地址不能为空
	if (strServerIP.empty())
	{
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("IP地址不能为空"),
			&m_resManager.GetSfSrcModelDlgBk());
		return;
		return;
	}

	//网络初始化或连接服务器
	if (m_pNWProcessor->Connect(strServerIP, nPort) != CCBCNetWorkProcessor::noerror)
	{
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("连接失败"),
			&m_resManager.GetSfSrcModelDlgBk());
		return;
	}

	//连接成功后则登录
	m_pNWProcessor->LoginServer(strUserName, strPassWord);
}

void CCBCGameRun::OnGUIPLPrevRLPage(void)
{
	PrevRLPage();
}

void CCBCGameRun::OnGUIPLNextRLPage(void)
{
	NextRLPage();
}

void CCBCGameRun::OnGUIPLEnterRoom(int nRoomID)
{
	//界面事件，用户进入了ID为nRoomID的房间
	BLUEASSERT(m_pNWProcessor);
	m_pNWProcessor->EnterRoom(nRoomID);
}

void CCBCGameRun::OnGUIPLPrepareGame(void)
{
	//界面事件，用户点击准备开始游戏
	BLUEASSERT(m_pNWProcessor);
	m_pNWProcessor->PrepareGame();
	//准备音效
	//m_pAudioProcessor->PlayEffect(MUSICLINE_READY);
}

void CCBCGameRun::OnGUIPLQuitRoom( void )
{
	//界面事件，用户离开房间
	BLUEASSERT(m_pNWProcessor);
	m_pNWProcessor->QuitRoom();
}

void CCBCGameRun::OnGUIPLHelp(void)
{
	BLUEASSERT(m_pGUIProcessor);

	m_pGUIProcessor->AddModelDialog(
		CBLUERect(CBLUEPoint(300, 10), CBLUESize(353, 504)),
		CBLUEPoint(110, 460),
		CBLUERect(0, 0, 353, 504),
		CBLUERect(130, 50, 360, 200),
		_BLUET(""),
		&m_resManager.GetSfSrcHelp());
}

void CCBCGameRun::OnGUIPLAbout(void)
{
	BLUEASSERT(m_pGUIProcessor);

	m_pGUIProcessor->AddModelDialog(
		CBLUERect(CBLUEPoint(300, 150), CBLUESize(350, 200)),
		CBLUEPoint(110, 140),
		CBLUERect(0, 0, 350, 200),
		CBLUERect(130, 50, 360, 200),
		_BLUET(""),
		&m_resManager.GetSfSrcAbout());
}

void CCBCGameRun::OnNWPLError(const int nErrorCode)
{
	switch (nErrorCode)
	{
	case NWP_ERROR_TYPE_GAMESTART:
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("游戏已经开始"),
			&m_resManager.GetSfSrcModelDlgBk());
		break;
	case NWP_ERROR_TYPE_HASINROOM:
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("已经在房间中"),
			&m_resManager.GetSfSrcModelDlgBk());
		break;
	case NWP_ERROR_TYPE_ROOMMAXUSER:
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("房间已经客满"),
			&m_resManager.GetSfSrcModelDlgBk());
		break;
	}
}

void CCBCGameRun::OnNWPLLoginServer(const BLUEString& strName)
{
	m_strSelfUserName =  strName;
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::roomlist);
	UpdateWindowCaptionText();
}
void CCBCGameRun::OnNWPLRefreshRoomListBegin(void)
{
	BLUETRACE(_BLUET("OnNWPLRefreshRoomListBegin\n"));
	m_rManager.Clear();
}

void CCBCGameRun::OnNWPLRoomInfo(int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum)
{
	BLUETRACE(_BLUET("OnNWPLRoomInfo %d\n"), nID);
	m_rManager.AddRoom(nID, strName, nCurClientNum, nMaxClientNum);
}

void CCBCGameRun::OnNWPLRefreshRoomListEnd(void)
{
	BLUETRACE(_BLUET("OnNWPLRefreshRoomListEnd\n"));
	m_rManager.ResetInnerIt();
	NextRLPage();
}

void CCBCGameRun::OnNWPLEnterRoom(const int nRoomID, const int nID)
{
	//进房间后的状态为inroom
	m_state = inroom;

	//自己的控制的游戏机启动声音
	for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
		m_gameColl[i]->SetPlaySound(nID == i);

	//交换使用窗口与第一个窗口
	std::swap(m_gameColl[0]->m_drawRc, m_gameColl[GetID()]->m_drawRc);
	std::swap(m_gameColl[0]->m_nExternUseID, m_gameColl[GetID()]->m_nExternUseID);

	//让UI界面更换为房间内界面
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::game);
	m_pGUIProcessor->SetPrepareBtnEnable(true);

	//更新窗口标题文字
	UpdateWindowCaptionText();
}

void CCBCGameRun::OnNWPLQuitRoom(void)
{
	//让UI界面更换为房间外界面
	BLUEASSERT(m_pGUIProcessor);
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::roomlist);

	//交换回使用窗口与第一个窗口
	std::swap(m_gameColl[0]->m_drawRc, m_gameColl[GetID()]->m_drawRc);
	std::swap(m_gameColl[0]->m_nExternUseID, m_gameColl[GetID()]->m_nExternUseID);

	//出房间后的状态为inroom
	m_state = outroom;	

	for (int i = 0; i < GAMEMACHINENUM; i++)
		m_gameColl[i]->LeaveUser();

	//更新窗口标题文字
	UpdateWindowCaptionText();

	//请求房间列表
	BLUEASSERT(m_pNWProcessor);
	m_pNWProcessor->RoomList();
}

void CCBCGameRun::OnNWPLRoomUser(const BLUEString& strName, const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	m_gameColl[nID]->SitUser(strName, nID);
}

void CCBCGameRun::OnNWPLUserEnterRoom(const BLUEString& strName, const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	m_gameColl[nID]->SitUser(strName, nID);
}

void CCBCGameRun::OnNWPLUserLeaveRoom(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_gameColl[nID]->GetSitUser());

	m_gameColl[nID]->LeaveUser();
}

void CCBCGameRun::OnNWPLPrepareGame(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//房间内ID号为nID的用户已经作好开始游戏的准备
	BLUEASSERT(m_pNWProcessor);
	if (nID == m_pNWProcessor->GetInRoomID())
		m_pGUIProcessor->SetPrepareBtnEnable(false);
}

void CCBCGameRun::OnNWPLStartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate)
{
	BLUEASSERT(m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game);
	//所有游戏机开始游戏
	for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
		m_gameColl[i]->StartGame(nSeed, dwInitRows, nBringToolRate);

	/* //开启游戏背景音乐
	m_pAudioProcessor->PlayBackground(MUSICLINE_BK_GAME);
	//游戏开始音效
	m_pAudioProcessor->PlayEffect(MUSICLINE_GAMESTART);
	*/
}

void CCBCGameRun::OnNWPLStopGame(bool bWin)
{
	BLUEASSERT(m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game);
	//开启正常背景音乐
	/*
	m_pAudioProcessor->PlayBackground(MUSICLINE_BK_NORMAL);
	//播放胜利或失败音效
	m_pAudioProcessor->PlayEffect(bWin ? MUSICLINE_GAMEWIN : MUSICLINE_GAMELOST);
	*/

	//恢复准备按钮可用状态
	BLUEASSERT(m_pGUIProcessor);
	m_pGUIProcessor->SetPrepareBtnEnable(true);
}


void CCBCGameRun::OnNWPLGameShootBubble(const int nID, const int nAngle)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionShootBubble(nAngle);
}

void CCBCGameRun::OnNWPLGameShootBadBubble(const int nID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionShootBadBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameEraseBubble(const int nID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionEraseBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameAddLineBadBubble(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionAddLineBadBubble();
}

void CCBCGameRun::OnNWPLGameUnknowBubbleColor(int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionUnknowBubbleColor();
}

void CCBCGameRun::OnNWPLGameThrowShit(int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionThrowShit();
}

void CCBCGameRun::OnNWPLGameAdjustToolOrder(int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionAdjustToolOrder();
}

void CCBCGameRun::OnNWPLGameToolThrowBadBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);
	
	//添加星星动画
	AddStarAni(nSrcID, nDestID);


	//如果是自己被使用了道具，则执行道具功能
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionShootBadBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameToolEraseBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//添加星星动画
	AddStarAni(nSrcID, nDestID);

	//如果是自己被使用了道具，则执行道具功能
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionEraseBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameToolThrowLineBadBubble(const int nSrcID, const int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//添加星星动画
	AddStarAni(nSrcID, nDestID);

	//添加星星动画
	AddStarAni(nSrcID, nDestID);

	//如果是自己被使用了道具，则执行道具功能
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionAddLineBadBubble();
}

void CCBCGameRun::OnNWPLGameToolChangeToolMode(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionChangeToolMode();
}

void CCBCGameRun::OnNWPLGameToolRemoveTool(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//自己不可能收到自己的该消息

	m_gameColl[nID]->ActionRemoveTool();
}

void CCBCGameRun::OnNWPLGameToolUnknowBubbleColor(int nSrcID, int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//添加星星动画
	AddStarAni(nSrcID, nDestID);

	//如果是自己被使用了道具，则执行道具功能
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionUnknowBubbleColor();
}

void CCBCGameRun::OnNWPLGameToolThrowShit(int nSrcID, int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//便便不用播放道具声音
	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//添加星星动画
	AddStarAni(nSrcID, nDestID);

	//如果是自己被使用了道具，则执行道具功能
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionThrowShit();
}


#pragma warning(push)
#pragma warning(disable : 4311)//去掉void*转成int的错误提示
void CCBCGameRun::OnCBGMEventGameOver(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameOver();
}

void CCBCGameRun::OnCBGMEventShootBubble(void* pCallBackParam, const int nAngle)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameShootBubble(nAngle);
}

void CCBCGameRun::OnCBGMEventShootBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameShootBadBubble(dwNumber);
}

void CCBCGameRun::OnCBGMEventEraseBubble(void* pCallBackParam, const BLUEDWORD dwNumber)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameEraseBubble(dwNumber);

}

void CCBCGameRun::OnCBGMEventAddLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameAddLineBadBubble();
}

void CCBCGameRun::OnCBGMEventUnknowBubbleColor(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameUnknowBubbleColor();
}

void CCBCGameRun::OnCBGMEventThrowShit(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameThrowShit();
}


void CCBCGameRun::OnCBGMEventChangeToolMode(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameToolChangeToolMode();
}

void CCBCGameRun::OnCBGMEventRemoveTool(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
	{
		//通知其他用户自己使用了道具，道具栏应该减少道具了
		m_pNWProcessor->GameToolRemoveTool();
	}
}

void CCBCGameRun::OnCBGMEventAdjustToolOrder( void* pCallBackParam )
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
	{
		//通知其他用户自己切换了道具顺序
		m_pNWProcessor->GameAdjustToolOrder();
	}
}

void CCBCGameRun::OnCBGMEventShootTimerOver( void* pCallBackParam )
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID() && m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game)
	{
		//通知其他用户自己切换了道具顺序
		m_gameColl[GetID()]->ActionShootBubble(m_gameColl[GetID()]->GetCurrentShootAngle());
	}
}

void CCBCGameRun::OnCBGMEventUsingToolForAddBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
	{
		//对自己是消泡泡，对敌人才是扔泡泡
		if (m_nSelID == GetID())
			m_pNWProcessor->GameToolEraseBubble(m_nSelID, dwNumber);
		else
			m_pNWProcessor->GameToolThrowBadBubble(m_nSelID, dwNumber);
	}
}

void CCBCGameRun::OnCBGMEventUsingToolForLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameToolThrowLineBadBubble(m_nSelID);
}

void CCBCGameRun::OnCBGMEventUsingToolForAddOneLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameToolThrowBadBubble(m_nSelID, 8);
}

void CCBCGameRun::OnCBGMEventUsingToolForSubOneLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameToolEraseBubble(m_nSelID, 8);
}

void CCBCGameRun::OnCBGMEventUsingToolForUnknowBubbleColor(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);
	
	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameToolUnknowBubbleColor(m_nSelID);
}

void CCBCGameRun::OnCBGMEventUsingToolForThrowShit(void* pCallBackParam)
{
	//pCallBackParam参数代表用户的ID号
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//如果是主玩家的发来的事件
	if (nID == GetID())
		m_pNWProcessor->GameToolThrowShit(m_nSelID);

}
#pragma warning(pop)