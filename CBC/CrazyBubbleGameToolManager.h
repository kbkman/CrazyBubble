#pragma once
#include "BLUEDirectX/engine2d.h"
#include <queue>

/**
���������߹�����
���ܣ���
˵������
�汾��1.0.0.1
���ߣ�BLUE
*/
class CCrazyBubbleGameToolManager
{
public:
	/*
	ö��������������
	���ܣ���
	˵������
	*/
	enum TOOL
	{
		linebadbubble,                 //��2�л�����
		addonelinebadbubble,          //��1�л�����
		subonelinebaddbubble,  //��1������
		unknowbubblecolor,       //��֪��������ɫ
		throwshit                        //������
	};

private:
	typedef std::queue<TOOL> TOOLQUEUE;//����ջ����

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
	����������յ���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void Clear(void);

	/*
	����������ȡ����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CDirectDrawSurface7& GetCanvas(void) {return m_surfaceCanvas;}

	/*
	��������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void AddThrowBubble(const BLUEDWORD dwNumber);

	/*
	��������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	BLUEDWORD RemoveThrowBubble(void);

	/*
	���������Ƿ��е��߿���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool IsHasTool(void) const {return !m_toolQueue.empty();}

	/*
	��������ѹ�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void AddTool(const TOOL tool)
	{
		m_toolQueue.push(tool);
		DrawCanvas();
	}

	/*
	����������������
	���ܣ���
	˵������
	��������
	����ֵ���Ƿ��е���
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
	���������Ƿ���ʹ�õ���
	���ܣ���
	˵������
	��������
	����ֵ���������false��ʾ��ʹ������
	*/
	bool IsUsingTool(void) const {return m_bIsUsingTool;}

	/*
	��������ʹ�õ������
	���ܣ���
	˵������
	������b ���Ϊfalse��ʾʹ�����ݣ�Ϊtrue��ʾʹ�õ���
	����ֵ����
	*/
	void UsingTool(bool b);

	/*
	���������л�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	bool AdjustToolOrder(void);


private:
	void DrawCanvas(void);


private:
	/*
	�����������߶���
	���ܣ���
	˵������
	*/
	TOOLQUEUE m_toolQueue;

	/*
	����������С�����ݸ���
	���ܣ���
	˵������
	*/
	BLUEDWORD m_dwBigBubbleNumber;
	BLUEDWORD m_dwSmallBubbleNumber;

	/*
	���������Ƿ���ʹ�õ��ߣ�����IsUsingTool����˵��
	���ܣ���
	˵������
	*/
	bool m_bIsUsingTool;

	/*
	������������
	���ܣ���
	˵������
	*/
	CDirectDrawSurface7 m_surfaceCanvas;

	/*
	�����������������Դͼ����
	���ܣ���
	˵������
	*/
	const CDirectDrawSurface7& m_surfaceToolBg;
	const CDirectDrawSurface7& m_surfaceToolBigBubble;
	const CDirectDrawSurface7& m_surfaceToolSmallBubble;

	CDDrawSprite m_spriteTool;

};
