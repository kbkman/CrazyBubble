#pragma once

#include "bass.h"

#define MUSICLINE_EFFECT_COUNT     12
#define MUSICLINE_BK_COUNT             2

#define MUSICLINE_BUBBLESHOOT       0
#define MUSICLINE_BUBBLEKICKWALL  1
#define MUSICLINE_BUBBLEFIX             2
#define MUSICLINE_BUBBLEERASE        3
#define MUSICLINE_BUBBLEDROP         4
#define MUSICLINE_READY                    5      //准备
#define MUSICLINE_GAMESTART            6      //游戏开始
#define MUSICLINE_CHANGETOOL        7      //切换道具
#define MUSICLINE_GAMEWIN              8      //游戏胜利
#define MUSICLINE_GAMELOST             9      //游戏失败
#define MUSICLINE_USETOOL               10    //使用道具
#define MUSICLINE_SHIT                      11    //便便声

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

	//播放音效
	bool PlayEffect(int nWhich)
	{
		BLUEASSERT(nWhich >= 0 && nWhich < MUSICLINE_EFFECT_COUNT);
		BLUEASSERT(m_pMLEffect);
		if (m_pMLEffect[nWhich].hStream)
			return ::BASS_ChannelPlay(m_pMLEffect[nWhich].hStream, TRUE) == TRUE;
		return false;
	}

	bool PlayBackground(int nWhich);//播放背景音乐

private:
	MUSICLINE* m_pMLEffect;
	MUSICLINE* m_pMLBk;
	int m_curMidiID;//当前播放midi的id
};
