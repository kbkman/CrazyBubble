#pragma once

#include "bass.h"

#define MUSICLINE_EFFECT_COUNT     12
#define MUSICLINE_BK_COUNT             2

#define MUSICLINE_BUBBLESHOOT       0
#define MUSICLINE_BUBBLEKICKWALL  1
#define MUSICLINE_BUBBLEFIX             2
#define MUSICLINE_BUBBLEERASE        3
#define MUSICLINE_BUBBLEDROP         4
#define MUSICLINE_READY                    5      //׼��
#define MUSICLINE_GAMESTART            6      //��Ϸ��ʼ
#define MUSICLINE_CHANGETOOL        7      //�л�����
#define MUSICLINE_GAMEWIN              8      //��Ϸʤ��
#define MUSICLINE_GAMELOST             9      //��Ϸʧ��
#define MUSICLINE_USETOOL               10    //ʹ�õ���
#define MUSICLINE_SHIT                      11    //�����

#define MUSICLINE_BK_NORMAL           0
#define MUSICLINE_BK_GAME               1



class CCBCAudioProcessor
{
	struct MUSICLINE
	{
		union {
			HSTREAM hStream;
			int midiID;
		};
		BLUEStringA strFileName;
	};

public:
	CCBCAudioProcessor(void);
	~CCBCAudioProcessor(void);

public:
	bool Create(HWND hWnd);
	void Destroy(void);

	//������Ч
	bool PlayEffect(int nWhich)
	{
		BLUEASSERT(nWhich >= 0 && nWhich < MUSICLINE_EFFECT_COUNT);
		BLUEASSERT(m_pMLEffect);
		if (m_pMLEffect[nWhich].hStream)
			return ::BASS_ChannelPlay(m_pMLEffect[nWhich].hStream, TRUE) == TRUE;
		return false;
	}

	bool PlayBackground(int nWhich);//���ű�������

private:
	MUSICLINE* m_pMLEffect;
	MUSICLINE* m_pMLBk;
	int m_curMidiID;//��ǰ����midi��id
};
