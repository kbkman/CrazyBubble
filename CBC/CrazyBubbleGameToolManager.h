#pragma once
#include "BLUEDirectX/engine2d.h"
#include <queue>

/**
类名：道具管理器
功能：无
说明：无
版本：1.0.0.1
作者：BLUE
*/
class CCrazyBubbleGameToolManager
{
public:
	/*
	枚举名：道具类型
	功能：无
	说明：无
	*/
	enum TOOL
	{
		linebadbubble,                 //加2行坏泡泡
		addonelinebadbubble,          //加1行坏泡泡
		subonelinebaddbubble,  //减1行泡泡
		unknowbubblecolor,       //不知道泡泡颜色
		throwshit                        //扔脏物
	};

private:
	typedef std::queue<TOOL> TOOLQUEUE;//道具栈定义

public:
	CCrazyBubbleGameToolManager(
		CDirectDraw7& dd7,
		const CDirectDrawSurface7& surfaceToolBg,
		const CDirectDrawSurface7& surfaceToolBigBubble,
		const CDirectDrawSurface7& surfaceToolSmallBubble,
		const CDirectDrawSurface7& surfaceTool
		);
	~CCrazyBubbleGameToolManager(void);

public:
	/*
	函数名：清空道具
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void Clear(void);

	/*
	函数名：获取画布
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	CDirectDrawSurface7& GetCanvas(void) {return m_surfaceCanvas;}

	/*
	函数名：
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void AddThrowBubble(const BLUEDWORD dwNumber);

	/*
	函数名：
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	BLUEDWORD RemoveThrowBubble(void);

	/*
	函数名：是否有道具可用
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool IsHasTool(void) const {return !m_toolQueue.empty();}

	/*
	函数名：压入道具
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	void AddTool(const TOOL tool)
	{
		m_toolQueue.push(tool);
		DrawCanvas();
	}

	/*
	函数名：弹出道具
	功能：略
	说明：无
	参数：无
	返回值：是否有道具
	*/
	TOOL RemoveTool(void)
	{
		BLUEASSERT(IsUsingTool());
		TOOL tool = m_toolQueue.front();
		m_toolQueue.pop();
		DrawCanvas();
		return tool;
	}

	/*
	函数名：是否在使用道具
	功能：略
	说明：无
	参数：无
	返回值：如果返回false表示在使用泡泡
	*/
	bool IsUsingTool(void) const {return m_bIsUsingTool;}

	/*
	函数名：使用道具与否
	功能：略
	说明：无
	参数：b 如果为false表示使用泡泡，为true表示使用道具
	返回值：无
	*/
	void UsingTool(bool b);

	/*
	函数名：切换道具
	功能：略
	说明：无
	参数：无
	返回值：无
	*/
	bool AdjustToolOrder(void);


private:
	void DrawCanvas(void);


private:
	/*
	变量名：道具队列
	功能：略
	说明：无
	*/
	TOOLQUEUE m_toolQueue;

	/*
	变量名：大（小）泡泡个数
	功能：略
	说明：无
	*/
	BLUEDWORD m_dwBigBubbleNumber;
	BLUEDWORD m_dwSmallBubbleNumber;

	/*
	变量名：是否在使用道具，参阅IsUsingTool函数说明
	功能：略
	说明：无
	*/
	bool m_bIsUsingTool;

	/*
	变量名：画布
	功能：略
	说明：无
	*/
	CDirectDrawSurface7 m_surfaceCanvas;

	/*
	变量名：道具相关资源图表面
	功能：略
	说明：无
	*/
	const CDirectDrawSurface7& m_surfaceToolBg;
	const CDirectDrawSurface7& m_surfaceToolBigBubble;
	const CDirectDrawSurface7& m_surfaceToolSmallBubble;

	CDDrawSprite m_spriteTool;

};
