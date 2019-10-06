#include "StdAfx.h"
#include "CBCGameRun.h"
#include "BLUESkin.h"

#define GAMEMACHINENUM                6           //��Ϸ������
#define GAMEMACHINE_LEFTOFFSET  10         //��Ϸ��������ƫ����
#define GAMEMACHINE_TOPOFFSET   10         //��Ϸ��������ƫ����

CCBCGameRun::CCBCGameRun(void) :
m_state(logo),
m_nSelID(-1),
m_aniProcessor(m_resManager),
m_pGUIProcessor(BLUENULL),
m_pNWProcessor(BLUENULL),
m_pAudioProcessor(BLUENULL)
{
	//Ƥ�����ʼ��
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

	//Ƥ��������
	::BLUESkin_UnBindNCSkin(GetMainWnd());
	::BLUESkin_Uninitialize();
}

HRESULT CCBCGameRun::GameInit(void)
{
	//ȥ����󻯰�ť�ͱ߿�ɵ��ڴ�С����
	::SetWindowLong(
		GetMainWnd(),
		GWL_STYLE,
		::GetWindowLong(GetMainWnd(), GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX)
		);

	//����Ƥ��
	::BLUESkin_BindNCSkin(GetMainWnd(), _BLUET("skin\\black"));
	::SetWindowPos(GetMainWnd(), NULL, 0, 0, 800, 600, SWP_NOMOVE);//���������Ƥ�����޷�������ʾ
	
	//��Դ��������ʼ
	if (!m_resManager.Load(m_dd7))
	{
		::MessageBox(GetMainWnd(), _BLUET("��Դ��������ʼ��ʧ��"), _BLUET("����"), MB_OK);
		return E_FAIL;
	}

	//�����ʼ��
	m_pNWProcessor = new CCBCNetWorkProcessor(this);


	//��ʼ������Ϸ��
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

	//��ʼ������Ϸ���Ļ�������
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

	//�û�UI��ʼ��
	BLUEASSERT(m_pGUIProcessor == BLUENULL);
	m_pGUIProcessor = new CCBCGUIProcessor(this, m_dd7, m_ddsback);
	if (!m_pGUIProcessor->Create())
	{
		::MessageBox(GetMainWnd(), _BLUET("UI����ʧ��"), _BLUET("����"), MB_OK);
		return E_FAIL;
	}
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::logo);


	//���´��ڱ�������
	UpdateWindowCaptionText();

	return S_OK;
}

HRESULT CCBCGameRun::GameMain(void)
{
	//���紦��
	NetWorkProcess();

	//���������봦��
	InputProcess();

	//��Ⱦ
	Render();

	return S_OK;
}

HRESULT CCBCGameRun::GameShutDown(void)
{
	//������Ƶ������
	/*
	if (m_pAudioProcessor)
	{
		delete m_pAudioProcessor;
		m_pAudioProcessor = BLUENULL;
	}
	*/

	//�������紦����
	if (m_pNWProcessor)
	{
		delete m_pNWProcessor;
		m_pNWProcessor = BLUENULL;
	}

	//����UI������
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

	//UI��������Ⱦ��Ϸ����
	BLUEASSERT(m_pGUIProcessor);
	m_pGUIProcessor->FillBack();

	//����ڷ������������Ϸ����
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

	//UI������Ⱦ
	m_pGUIProcessor->Render();

	//������Ⱦ
	m_aniProcessor.ActorAllAni(m_ddsback);

	//��ת����
	Flip();
}

void CCBCGameRun::UpdateWindowCaptionText(void)
{
	BLUEASSERT(GetMainWnd());

	BLUEString strTitle(_BLUET("���������Online---["));
	switch (m_pNWProcessor->GetState())
	{
	case CCBCNetWorkProcessor::disconnect:
		strTitle += _BLUET("��¼");
		break;
	case CCBCNetWorkProcessor::connect:
		strTitle += _BLUET("�ѵ�¼");
		break;
	case CCBCNetWorkProcessor::login:
		strTitle += _BLUET("�����б�");
		break;
	case CCBCNetWorkProcessor::room:
		strTitle += _BLUET("������");
		break;
	case CCBCNetWorkProcessor::game:
		strTitle += _BLUET("��Ϸ��");
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
	//����һҳ����ִ����һҳ
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
	//UI����������Ϣ�Ĵ���
	if (m_pGUIProcessor)
		m_pGUIProcessor->ProcessMessage(uMsg, wParam, lParam);

	//��Ϸ�������Ϣ�Ĵ���
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game)
		{
			if (wParam == VK_SPACE)//�ո�Ϊ��������
			{
				m_gameColl[GetID()]->ShootBubble();
			}
			else if (wParam >= _T('1') && wParam < _T('7'))//1~6Ϊʹ�õ���
			{
				m_nSelID = GetIDFromExternID((int)wParam - (int)_T('0'));
				m_gameColl[GetID()]->UseTool();
			}
			else if (wParam == 'A' || wParam == 'a')//aΪ�л�����������
			{
				m_gameColl[GetID()]->ChangeToolMode();
			}
			else if (wParam == 'S' || wParam == 's')//sΪ�л�����˳��
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
		//��Ƶ��ʼ��
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
	//�����UNICODE�ַ�������ת��ANSI
	BLUEStringA strServerIP;
#ifdef UNICODE
	::BLUEW2A(strIP, strServerIP);
#else
	strServerIP = strIP;
#endif

	//�û�������Ϊ��
	if (strUserName.empty())
	{
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("�û�������Ϊ��"),
			&m_resManager.GetSfSrcModelDlgBk());
		return;
	}

	//IP��ַ����Ϊ��
	if (strServerIP.empty())
	{
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("IP��ַ����Ϊ��"),
			&m_resManager.GetSfSrcModelDlgBk());
		return;
		return;
	}

	//�����ʼ�������ӷ�����
	if (m_pNWProcessor->Connect(strServerIP, nPort) != CCBCNetWorkProcessor::noerror)
	{
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("����ʧ��"),
			&m_resManager.GetSfSrcModelDlgBk());
		return;
	}

	//���ӳɹ������¼
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
	//�����¼����û�������IDΪnRoomID�ķ���
	BLUEASSERT(m_pNWProcessor);
	m_pNWProcessor->EnterRoom(nRoomID);
}

void CCBCGameRun::OnGUIPLPrepareGame(void)
{
	//�����¼����û����׼����ʼ��Ϸ
	BLUEASSERT(m_pNWProcessor);
	m_pNWProcessor->PrepareGame();
	//׼����Ч
	//m_pAudioProcessor->PlayEffect(MUSICLINE_READY);
}

void CCBCGameRun::OnGUIPLQuitRoom( void )
{
	//�����¼����û��뿪����
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
			_BLUET("��Ϸ�Ѿ���ʼ"),
			&m_resManager.GetSfSrcModelDlgBk());
		break;
	case NWP_ERROR_TYPE_HASINROOM:
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("�Ѿ��ڷ�����"),
			&m_resManager.GetSfSrcModelDlgBk());
		break;
	case NWP_ERROR_TYPE_ROOMMAXUSER:
		m_pGUIProcessor->AddModelDialog(
			CBLUERect(200, 150, 400, 400),
			CBLUEPoint(110, 100),
			CBLUERect(0, 0, 350, 170),
			CBLUERect(130, 50, 360, 250),
			_BLUET("�����Ѿ�����"),
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
	//��������״̬Ϊinroom
	m_state = inroom;

	//�Լ��Ŀ��Ƶ���Ϸ����������
	for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
		m_gameColl[i]->SetPlaySound(nID == i);

	//����ʹ�ô������һ������
	std::swap(m_gameColl[0]->m_drawRc, m_gameColl[GetID()]->m_drawRc);
	std::swap(m_gameColl[0]->m_nExternUseID, m_gameColl[GetID()]->m_nExternUseID);

	//��UI�������Ϊ�����ڽ���
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::game);
	m_pGUIProcessor->SetPrepareBtnEnable(true);

	//���´��ڱ�������
	UpdateWindowCaptionText();
}

void CCBCGameRun::OnNWPLQuitRoom(void)
{
	//��UI�������Ϊ���������
	BLUEASSERT(m_pGUIProcessor);
	m_pGUIProcessor->ChangeUIType(CCBCGUIProcessor::roomlist);

	//������ʹ�ô������һ������
	std::swap(m_gameColl[0]->m_drawRc, m_gameColl[GetID()]->m_drawRc);
	std::swap(m_gameColl[0]->m_nExternUseID, m_gameColl[GetID()]->m_nExternUseID);

	//��������״̬Ϊinroom
	m_state = outroom;	

	for (int i = 0; i < GAMEMACHINENUM; i++)
		m_gameColl[i]->LeaveUser();

	//���´��ڱ�������
	UpdateWindowCaptionText();

	//���󷿼��б�
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

	//������ID��ΪnID���û��Ѿ����ÿ�ʼ��Ϸ��׼��
	BLUEASSERT(m_pNWProcessor);
	if (nID == m_pNWProcessor->GetInRoomID())
		m_pGUIProcessor->SetPrepareBtnEnable(false);
}

void CCBCGameRun::OnNWPLStartGame(int nSeed, BLUEDWORD dwInitRows, int nBringToolRate)
{
	BLUEASSERT(m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game);
	//������Ϸ����ʼ��Ϸ
	for (GAMECOLL::size_type i = 0; i < GAMEMACHINENUM; i++)
		m_gameColl[i]->StartGame(nSeed, dwInitRows, nBringToolRate);

	/* //������Ϸ��������
	m_pAudioProcessor->PlayBackground(MUSICLINE_BK_GAME);
	//��Ϸ��ʼ��Ч
	m_pAudioProcessor->PlayEffect(MUSICLINE_GAMESTART);
	*/
}

void CCBCGameRun::OnNWPLStopGame(bool bWin)
{
	BLUEASSERT(m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game);
	//����������������
	/*
	m_pAudioProcessor->PlayBackground(MUSICLINE_BK_NORMAL);
	//����ʤ����ʧ����Ч
	m_pAudioProcessor->PlayEffect(bWin ? MUSICLINE_GAMEWIN : MUSICLINE_GAMELOST);
	*/

	//�ָ�׼����ť����״̬
	BLUEASSERT(m_pGUIProcessor);
	m_pGUIProcessor->SetPrepareBtnEnable(true);
}


void CCBCGameRun::OnNWPLGameShootBubble(const int nID, const int nAngle)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionShootBubble(nAngle);
}

void CCBCGameRun::OnNWPLGameShootBadBubble(const int nID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionShootBadBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameEraseBubble(const int nID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionEraseBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameAddLineBadBubble(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionAddLineBadBubble();
}

void CCBCGameRun::OnNWPLGameUnknowBubbleColor(int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionUnknowBubbleColor();
}

void CCBCGameRun::OnNWPLGameThrowShit(int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionThrowShit();
}

void CCBCGameRun::OnNWPLGameAdjustToolOrder(int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionAdjustToolOrder();
}

void CCBCGameRun::OnNWPLGameToolThrowBadBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);
	
	//������Ƕ���
	AddStarAni(nSrcID, nDestID);


	//������Լ���ʹ���˵��ߣ���ִ�е��߹���
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionShootBadBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameToolEraseBubble(const int nSrcID, const int nDestID, const BLUEDWORD dwNumber)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//������Ƕ���
	AddStarAni(nSrcID, nDestID);

	//������Լ���ʹ���˵��ߣ���ִ�е��߹���
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionEraseBubble(dwNumber);
}

void CCBCGameRun::OnNWPLGameToolThrowLineBadBubble(const int nSrcID, const int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//������Ƕ���
	AddStarAni(nSrcID, nDestID);

	//������Ƕ���
	AddStarAni(nSrcID, nDestID);

	//������Լ���ʹ���˵��ߣ���ִ�е��߹���
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionAddLineBadBubble();
}

void CCBCGameRun::OnNWPLGameToolChangeToolMode(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionChangeToolMode();
}

void CCBCGameRun::OnNWPLGameToolRemoveTool(const int nID)
{
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(nID != GetID());//�Լ��������յ��Լ��ĸ���Ϣ

	m_gameColl[nID]->ActionRemoveTool();
}

void CCBCGameRun::OnNWPLGameToolUnknowBubbleColor(int nSrcID, int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//������Ƕ���
	AddStarAni(nSrcID, nDestID);

	//������Լ���ʹ���˵��ߣ���ִ�е��߹���
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionUnknowBubbleColor();
}

void CCBCGameRun::OnNWPLGameToolThrowShit(int nSrcID, int nDestID)
{
	BLUEASSERT(nSrcID >= 0 && nSrcID < GAMEMACHINENUM);
	BLUEASSERT(nDestID >= 0 && nDestID < GAMEMACHINENUM);

	//��㲻�ò��ŵ�������
	//BLUEASSERT(m_pAudioProcessor);
	//m_pAudioProcessor->PlayEffect(MUSICLINE_USETOOL);

	//������Ƕ���
	AddStarAni(nSrcID, nDestID);

	//������Լ���ʹ���˵��ߣ���ִ�е��߹���
	if (nDestID == GetID())
		m_gameColl[GetID()]->ActionThrowShit();
}


#pragma warning(push)
#pragma warning(disable : 4311)//ȥ��void*ת��int�Ĵ�����ʾ
void CCBCGameRun::OnCBGMEventGameOver(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameOver();
}

void CCBCGameRun::OnCBGMEventShootBubble(void* pCallBackParam, const int nAngle)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameShootBubble(nAngle);
}

void CCBCGameRun::OnCBGMEventShootBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameShootBadBubble(dwNumber);
}

void CCBCGameRun::OnCBGMEventEraseBubble(void* pCallBackParam, const BLUEDWORD dwNumber)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameEraseBubble(dwNumber);

}

void CCBCGameRun::OnCBGMEventAddLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameAddLineBadBubble();
}

void CCBCGameRun::OnCBGMEventUnknowBubbleColor(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameUnknowBubbleColor();
}

void CCBCGameRun::OnCBGMEventThrowShit(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameThrowShit();
}


void CCBCGameRun::OnCBGMEventChangeToolMode(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameToolChangeToolMode();
}

void CCBCGameRun::OnCBGMEventRemoveTool(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
	{
		//֪ͨ�����û��Լ�ʹ���˵��ߣ�������Ӧ�ü��ٵ�����
		m_pNWProcessor->GameToolRemoveTool();
	}
}

void CCBCGameRun::OnCBGMEventAdjustToolOrder( void* pCallBackParam )
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
	{
		//֪ͨ�����û��Լ��л��˵���˳��
		m_pNWProcessor->GameAdjustToolOrder();
	}
}

void CCBCGameRun::OnCBGMEventShootTimerOver( void* pCallBackParam )
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID() && m_pNWProcessor->GetState() == CCBCNetWorkProcessor::game)
	{
		//֪ͨ�����û��Լ��л��˵���˳��
		m_gameColl[GetID()]->ActionShootBubble(m_gameColl[GetID()]->GetCurrentShootAngle());
	}
}

void CCBCGameRun::OnCBGMEventUsingToolForAddBadBubble(void* pCallBackParam, const BLUEDWORD dwNumber)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
	{
		//���Լ��������ݣ��Ե��˲���������
		if (m_nSelID == GetID())
			m_pNWProcessor->GameToolEraseBubble(m_nSelID, dwNumber);
		else
			m_pNWProcessor->GameToolThrowBadBubble(m_nSelID, dwNumber);
	}
}

void CCBCGameRun::OnCBGMEventUsingToolForLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameToolThrowLineBadBubble(m_nSelID);
}

void CCBCGameRun::OnCBGMEventUsingToolForAddOneLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameToolThrowBadBubble(m_nSelID, 8);
}

void CCBCGameRun::OnCBGMEventUsingToolForSubOneLineBadBubble(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameToolEraseBubble(m_nSelID, 8);
}

void CCBCGameRun::OnCBGMEventUsingToolForUnknowBubbleColor(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);
	
	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameToolUnknowBubbleColor(m_nSelID);
}

void CCBCGameRun::OnCBGMEventUsingToolForThrowShit(void* pCallBackParam)
{
	//pCallBackParam���������û���ID��
	int nID = (int)pCallBackParam;
	BLUEASSERT(nID >= 0 && nID < GAMEMACHINENUM);
	BLUEASSERT(m_pNWProcessor);

	//���������ҵķ������¼�
	if (nID == GetID())
		m_pNWProcessor->GameToolThrowShit(m_nSelID);

}
#pragma warning(pop)