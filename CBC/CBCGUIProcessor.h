#pragma once

#include "BLUEDirectX/gui2d.h"
#include "BLUEDirectX/gui2dList.h"


/**
类名：CBC界面UI监听者
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class ICBCGUIProcessorListener
{
public:
	virtual void OnGUIPChangeUIType(int uiType) = 0;
	virtual void OnGUIPLConnectServer(const BLUEString& strIP, BLUEWORD nPort, const BLUEString& strUserName, const BLUEString& strPassWord) = 0;//登录服务器事件，strIP为服务器IP地址，nPort为服务器端口
	virtual void OnGUIPLPrevRLPage(void) = 0;//用户点击了上一页房间列表按钮
	virtual void OnGUIPLNextRLPage(void) = 0;//用户点击了下一页房间列表按钮
	virtual void OnGUIPLEnterRoom(int nRoomID) = 0;//用户点击进入某房间，nRoomID为房间的ID号
	virtual void OnGUIPLPrepareGame(void) = 0;//用户点击准备开始游戏按钮
	virtual void OnGUIPLQuitRoom(void) = 0;//用户点击离开房间按钮
	virtual void OnGUIPLHelp(void) = 0;//用户点击帮助按钮
	virtual void OnGUIPLAbout(void) = 0;//用户点击关于按钮
};

/**
类名：CBC用户界面处理器
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCBCGUIProcessor : private IDXGCommandListener
{
public:
	/*
	枚举名：界面类型
	功能：无
	说明：无
	*/
	enum UITYPE {
		none,           //无界面
		logo,            //logo界面
		login,           //登录界面
		roomlist,      //房间列表界面
		game           //游戏界面
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
	函数名：绘制背景
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void FillBack(void);

	//清除房间列表信息
	void ClearRoomListInfo(void);

	//添加房间信息
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
	//添加模式对话框
	void AddModelDialog(
		const CBLUERect& rcDlg,               //对话框位置
		const CBLUEPoint& ptOKBtnPos,  //确定按钮相对于对话框位置
		const CBLUERect& rcImg,             //图片位置
		const CBLUERect& rcText,            //文本位置
		const BLUEString& strText,           //文本内容
		const CDirectDrawSurface7* pSurfaceImg = BLUENULL   //图片资源表面
		);

	//UITYPE界面类型
	UITYPE GetUIType(void) const {return m_uiType;}
	bool ChangeUIType(UITYPE uiType);

	//Windows消息处理
	void ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//交给当前存在的GUI管理器处理UI消息
		if (m_pCurrentGUIManager)
			m_pCurrentGUIManager->ProcessMessage(uMsg, wParam, lParam);
	}

	//设置准备按钮是否可用
	bool SetPrepareBtnEnable(bool b)
	{
		if (m_pGUIBtnPerpareGame)
		{
			m_pGUIBtnPerpareGame->SetEnabled(b);
			return true;
		}
		return false;
	}

	//渲染UI
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
	变量名：监听者接口
	功能：略
	说明：无
	*/
	ICBCGUIProcessorListener* m_pListener;

	/*
	变量名：DDraw对象
	功能：略
	说明：无
	*/
	const CDirectDraw7& m_dd7;

	/*
	变量名：绘制的表面
	功能：略
	说明：无
	*/
	const CDirectDrawSurface7& m_ddsCanvas;

	/*
	变量名：界面类型
	功能：略
	说明：无
	*/
	UITYPE m_uiType;

	//所有不同界面的界面管理器
	CDXGManager* m_pCurrentGUIManager;//当前界面管理器指针
	CDXGManager* m_pLogoGUIManager;//logo界面管理器
	CDXGManager* m_pLoginGUIManager;//登录界面管理器
	CDXGManager* m_pRoomListGUIManager;//房间列表界面管理器
	CDXGManager* m_pGameGUIManager;//游戏界面管理器

	//所有UI控件及资源
	//背景资源
	CDirectDrawSurface7 m_surfaceBackground;
	CDDrawSprite m_spriteBg_1;
	CDDrawSprite m_spriteBg_2;

	//模式对话框所用控件及资源
	CDXGWindow2D* m_pGUIModelDialog;//模式对话框
	CDirectDrawSurface7 m_surfaceOKBtn;//登录按钮图资源
	CDDrawSprite m_spriteOKBtn;//确定按钮精灵
	CDXGButton2D* m_pGUIBtnOK;//确定按钮控件
	CDXGImage2D* m_pGUIModelDlgImg;//模式对话框中图片控件
	CDXGStatic2D m_guiModelDlgStatic;//模式对话框中标签控件

	//LOGO界面UI控件及资源
	CDirectDrawSurface7 m_surfaceLogo;//Logo图

	//登录界面UI控件及资源
	CDirectDrawSurface7 m_surfaceLoginBack;//登录背景图资源
	CDirectDrawSurface7 m_surfaceLoginBtn;//登录按钮图资源
	CDDrawSprite m_spriteLoginBtn;//按钮图包装成精灵
	CDXGImage2D* m_pGUIImgLoginBack;//用于显示登录背景的图像控件
	CDXGEdit2D* m_pGUIEditIP;//编辑IP地址的编辑控件
	CDXGEdit2D* m_pGUIEditUserName;//编辑用户名的编辑控件
	CDXGEdit2D* m_pGUIEditPassWord;//编辑用户密码的编辑控件
	CDXGButton2D* m_pGUIBtnLogin;//登录按钮控件

	//房间列表界面UI控件及资源
	CDirectDrawSurface7 m_surfaceRLBg;//房间列表背景图资源
	CDXGImage2D* m_pGUIImgRLBg;//房间列表背景图控件
	//--
	CDirectDrawSurface7 m_surfacePrevRLPageBtn;//上一页房间列表按钮资源
	CDDrawSprite m_spritePrevRLPageBtn;//上一页房间列表按钮精灵
	CDXGButton2D* m_pGUIBtnPrevRLPageBtn;//上一页房间列表按钮控件
	//--
	CDirectDrawSurface7 m_surfaceNextRLPageBtn;//上一页房间列表按钮资源
	CDDrawSprite m_spriteNextRLPageBtn;//上一页房间列表按钮精灵
	CDXGButton2D* m_pGUIBtnNextRLPageBtn;//上一页房间列表按钮控件
	//--
	CDirectDrawSurface7 m_surfaceEnterRoomBtn;
	CDXGList2D* m_pGUIListRoomList;//房间列表控件


	//游戏界面UI控件及资源
	CDirectDrawSurface7 m_surfacePerpareGameBtn;//准备开始游戏按钮资源
	CDDrawSprite m_spritePerpareGameBtn;//准备开始游戏按钮精灵
	CDXGButton2D* m_pGUIBtnPerpareGame;//准备开始游戏按钮控件
	CDirectDrawSurface7 m_surfaceQuitRoomBtn;//离开房间按钮按源
	CDDrawSprite m_spriteQuitRoomBtn;//离开房间按钮精灵
	CDXGButton2D* m_pGUIBtnQuitRoom;//离开房间按钮控件
	CDirectDrawSurface7 m_surfaceHelpBtn;//帮助按钮按源
	CDDrawSprite m_spriteHelpBtn;//帮助按钮精灵
	CDXGButton2D* m_pGUIBtnHelp;//帮助按钮控件
	CDirectDrawSurface7 m_surfaceAboutBtn;//关于按钮按源
	CDDrawSprite m_spriteAboutBtn;//关于按钮精灵
	CDXGButton2D* m_pGUIBtnAbout;//关于按钮控件
};
