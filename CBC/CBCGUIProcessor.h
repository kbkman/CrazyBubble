#pragma once

#include "BLUEDirectX/gui2d.h"
#include "BLUEDirectX/gui2dList.h"


/**
������CBC����UI������
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class ICBCGUIProcessorListener
{
public:
	virtual void OnGUIPChangeUIType(int uiType) = 0;
	virtual void OnGUIPLConnectServer(const BLUEString& strIP, BLUEWORD nPort, const BLUEString& strUserName, const BLUEString& strPassWord) = 0;//��¼�������¼���strIPΪ������IP��ַ��nPortΪ�������˿�
	virtual void OnGUIPLPrevRLPage(void) = 0;//�û��������һҳ�����б�ť
	virtual void OnGUIPLNextRLPage(void) = 0;//�û��������һҳ�����б�ť
	virtual void OnGUIPLEnterRoom(int nRoomID) = 0;//�û��������ĳ���䣬nRoomIDΪ�����ID��
	virtual void OnGUIPLPrepareGame(void) = 0;//�û����׼����ʼ��Ϸ��ť
	virtual void OnGUIPLQuitRoom(void) = 0;//�û�����뿪���䰴ť
	virtual void OnGUIPLHelp(void) = 0;//�û����������ť
	virtual void OnGUIPLAbout(void) = 0;//�û�������ڰ�ť
};

/**
������CBC�û����洦����
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCBCGUIProcessor : private IDXGCommandListener
{
public:
	/*
	ö��������������
	���ܣ���
	˵������
	*/
	enum UITYPE {
		none,           //�޽���
		logo,            //logo����
		login,           //��¼����
		roomlist,      //�����б����
		game           //��Ϸ����
	};

public:
	CCBCGUIProcessor(ICBCGUIProcessorListener* pListener, const CDirectDraw7& dd7, const CDirectDrawSurface7& ddsCanvas);
	~CCBCGUIProcessor(void);

public:
	bool Create(void);
	void Destroy(void);
	bool IsCreate(void) const {return m_pListener != BLUENULL;}

public:
	/*
	�����������Ʊ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void FillBack(void);

	//��������б���Ϣ
	void ClearRoomListInfo(void);

	//��ӷ�����Ϣ
	void AddRoomInfo(
		int nID,
		const BLUEString& strRoomName,
		int nCurClientNum,
		int nMaxClientNum
		);

	int GetRoomListNum(void) const
	{
		if (m_pGUIListRoomList)
			return m_pGUIListRoomList->GetSize();
		return 0;
	}

public:
	//���ģʽ�Ի���
	void AddModelDialog(
		const CBLUERect& rcDlg,               //�Ի���λ��
		const CBLUEPoint& ptOKBtnPos,  //ȷ����ť����ڶԻ���λ��
		const CBLUERect& rcImg,             //ͼƬλ��
		const CBLUERect& rcText,            //�ı�λ��
		const BLUEString& strText,           //�ı�����
		const CDirectDrawSurface7* pSurfaceImg = BLUENULL   //ͼƬ��Դ����
		);

	//UITYPE��������
	UITYPE GetUIType(void) const {return m_uiType;}
	bool ChangeUIType(UITYPE uiType);

	//Windows��Ϣ����
	void ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//������ǰ���ڵ�GUI����������UI��Ϣ
		if (m_pCurrentGUIManager)
			m_pCurrentGUIManager->ProcessMessage(uMsg, wParam, lParam);
	}

	//����׼����ť�Ƿ����
	bool SetPrepareBtnEnable(bool b)
	{
		if (m_pGUIBtnPerpareGame)
		{
			m_pGUIBtnPerpareGame->SetEnabled(b);
			return true;
		}
		return false;
	}

	//��ȾUI
	void Render(void) {BLUEASSERT(m_pCurrentGUIManager);m_pCurrentGUIManager->Render();}

private:
	bool CreateLogoUI(void);
	bool CreateLoginUI(void);
	bool CreateRoomListUI(void);
	bool CreateGameUI(void);

	void DestroyLogoUI(void);
	void DestroyLoginUI(void);
	void DestroyRoomListUI(void);
	void DestroyGameUI(void);

	virtual void OnDXGCommand(CDXGWindow* pWindow, unsigned int nEvent, int nDetail);

private:
	/*
	�������������߽ӿ�
	���ܣ���
	˵������
	*/
	ICBCGUIProcessorListener* m_pListener;

	/*
	��������DDraw����
	���ܣ���
	˵������
	*/
	const CDirectDraw7& m_dd7;

	/*
	�����������Ƶı���
	���ܣ���
	˵������
	*/
	const CDirectDrawSurface7& m_ddsCanvas;

	/*
	����������������
	���ܣ���
	˵������
	*/
	UITYPE m_uiType;

	//���в�ͬ����Ľ��������
	CDXGManager* m_pCurrentGUIManager;//��ǰ���������ָ��
	CDXGManager* m_pLogoGUIManager;//logo���������
	CDXGManager* m_pLoginGUIManager;//��¼���������
	CDXGManager* m_pRoomListGUIManager;//�����б���������
	CDXGManager* m_pGameGUIManager;//��Ϸ���������

	//����UI�ؼ�����Դ
	//������Դ
	CDirectDrawSurface7 m_surfaceBackground;
	CDDrawSprite m_spriteBg_1;
	CDDrawSprite m_spriteBg_2;

	//ģʽ�Ի������ÿؼ�����Դ
	CDXGWindow2D* m_pGUIModelDialog;//ģʽ�Ի���
	CDirectDrawSurface7 m_surfaceOKBtn;//��¼��ťͼ��Դ
	CDDrawSprite m_spriteOKBtn;//ȷ����ť����
	CDXGButton2D* m_pGUIBtnOK;//ȷ����ť�ؼ�
	CDXGImage2D* m_pGUIModelDlgImg;//ģʽ�Ի�����ͼƬ�ؼ�
	CDXGStatic2D m_guiModelDlgStatic;//ģʽ�Ի����б�ǩ�ؼ�

	//LOGO����UI�ؼ�����Դ
	CDirectDrawSurface7 m_surfaceLogo;//Logoͼ

	//��¼����UI�ؼ�����Դ
	CDirectDrawSurface7 m_surfaceLoginBack;//��¼����ͼ��Դ
	CDirectDrawSurface7 m_surfaceLoginBtn;//��¼��ťͼ��Դ
	CDDrawSprite m_spriteLoginBtn;//��ťͼ��װ�ɾ���
	CDXGImage2D* m_pGUIImgLoginBack;//������ʾ��¼������ͼ��ؼ�
	CDXGEdit2D* m_pGUIEditIP;//�༭IP��ַ�ı༭�ؼ�
	CDXGEdit2D* m_pGUIEditUserName;//�༭�û����ı༭�ؼ�
	CDXGEdit2D* m_pGUIEditPassWord;//�༭�û�����ı༭�ؼ�
	CDXGButton2D* m_pGUIBtnLogin;//��¼��ť�ؼ�

	//�����б����UI�ؼ�����Դ
	CDirectDrawSurface7 m_surfaceRLBg;//�����б���ͼ��Դ
	CDXGImage2D* m_pGUIImgRLBg;//�����б���ͼ�ؼ�
	//--
	CDirectDrawSurface7 m_surfacePrevRLPageBtn;//��һҳ�����б�ť��Դ
	CDDrawSprite m_spritePrevRLPageBtn;//��һҳ�����б�ť����
	CDXGButton2D* m_pGUIBtnPrevRLPageBtn;//��һҳ�����б�ť�ؼ�
	//--
	CDirectDrawSurface7 m_surfaceNextRLPageBtn;//��һҳ�����б�ť��Դ
	CDDrawSprite m_spriteNextRLPageBtn;//��һҳ�����б�ť����
	CDXGButton2D* m_pGUIBtnNextRLPageBtn;//��һҳ�����б�ť�ؼ�
	//--
	CDirectDrawSurface7 m_surfaceEnterRoomBtn;
	CDXGList2D* m_pGUIListRoomList;//�����б�ؼ�


	//��Ϸ����UI�ؼ�����Դ
	CDirectDrawSurface7 m_surfacePerpareGameBtn;//׼����ʼ��Ϸ��ť��Դ
	CDDrawSprite m_spritePerpareGameBtn;//׼����ʼ��Ϸ��ť����
	CDXGButton2D* m_pGUIBtnPerpareGame;//׼����ʼ��Ϸ��ť�ؼ�
	CDirectDrawSurface7 m_surfaceQuitRoomBtn;//�뿪���䰴ť��Դ
	CDDrawSprite m_spriteQuitRoomBtn;//�뿪���䰴ť����
	CDXGButton2D* m_pGUIBtnQuitRoom;//�뿪���䰴ť�ؼ�
	CDirectDrawSurface7 m_surfaceHelpBtn;//������ť��Դ
	CDDrawSprite m_spriteHelpBtn;//������ť����
	CDXGButton2D* m_pGUIBtnHelp;//������ť�ؼ�
	CDirectDrawSurface7 m_surfaceAboutBtn;//���ڰ�ť��Դ
	CDDrawSprite m_spriteAboutBtn;//���ڰ�ť����
	CDXGButton2D* m_pGUIBtnAbout;//���ڰ�ť�ؼ�
};
