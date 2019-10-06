#include "StdAfx.h"
#include "CBCAudioProcessor.h"
#include "MIDI.h"

#pragma comment(lib, "../Common/bass.lib")

CCBCAudioProcessor::CCBCAudioProcessor(void) :
m_pMLBk(BLUENULL),
m_pMLEffect(BLUENULL),
m_curMidiID(-1)
{
}

CCBCAudioProcessor::~CCBCAudioProcessor(void)
{
	Destroy();
	if (m_pMLEffect) delete[] m_pMLEffect;
	if (m_pMLBk) delete[] m_pMLBk;
}

bool CCBCAudioProcessor::Create(HWND hWnd)
{
	//如果已经创建则退出
	if (m_pMLBk)
		return false;

	//填充数字用
	char strNum[10];

	//初始背景音乐数组
	m_pMLBk = new MUSICLINE[MUSICLINE_BK_COUNT];
	for (int i = 0; i < MUSICLINE_BK_COUNT; i++)
	{
		::_itoa_s(i, strNum, sizeof(strNum), 10);
		m_pMLBk[i].midiID = -1;
		m_pMLBk[i].strFileName = BLUEStringA("res\\bk") + strNum + ".mid";
	}

	//初始音效条数组
	m_pMLEffect = new MUSICLINE[MUSICLINE_EFFECT_COUNT];
	for (int i = 0; i < MUSICLINE_EFFECT_COUNT; i++)
	{
		::_itoa_s(i, strNum, sizeof(strNum), 10);
		m_pMLEffect[i].hStream = BLUENULL;
		m_pMLEffect[i].strFileName = BLUEStringA("res\\e") + strNum + ".wav";
	}

	//加载所有midi背景音乐
	::DMusic_Init(hWnd);
	for (int i = 0; i < MUSICLINE_BK_COUNT; i++)
		m_pMLBk[i].midiID = ::DMusic_Load_MIDI(m_pMLBk[i].strFileName.c_str());

	//加载所有音效
	if (!::BASS_Init(-1, 44100, 0, 0, 0))
		return false;
	for (int i = 0; i < MUSICLINE_EFFECT_COUNT; i++)
		if (m_pMLEffect[i].hStream == BLUENULL)
			m_pMLEffect[i].hStream = ::BASS_StreamCreateFile(FALSE, m_pMLEffect[i].strFileName.c_str(), 0, 0, 0);

	return true;
}

void CCBCAudioProcessor::Destroy( void )
{
	if (m_pMLEffect == BLUENULL)
		return;

	::DMusic_Shutdown();

	for (int i = 0; i < MUSICLINE_EFFECT_COUNT; i++)
	{
		if (m_pMLEffect[i].hStream)
		{
			::BASS_StreamFree(m_pMLEffect[i].hStream);
			m_pMLEffect[i].hStream = BLUENULL;
		}
	}
	::BASS_Free();
}

bool CCBCAudioProcessor::PlayBackground(int nWhich)
{
	BLUEASSERT(nWhich >= 0 && nWhich < MUSICLINE_BK_COUNT);
	BLUEASSERT(m_pMLBk);
	//return 0;

	if (m_pMLBk[nWhich].midiID == -1)
		return false;

	//当前有背景音乐在播放，则先停止该音乐
	if (m_curMidiID != -1)
	{
		::DMusic_Stop(m_curMidiID);
		m_curMidiID = 0;
	}

	//播放背景音乐
	::DMusic_Play(m_pMLBk[nWhich].midiID);
	return false;
}