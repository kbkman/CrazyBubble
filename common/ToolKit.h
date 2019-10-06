#pragma once

/***
���ֹ�����͹��ߺ����ļ���
�汾��1.0.0.1
���ߣ�BLUE
***/

#include "BLUEDef.h"
#include "BLUEDebug.h"
#include "BLUERandom.h"
#include <tchar.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <conio.h>
using namespace std;

/***
����Ԥ����ͺ�
***/

/**********************
���й�����
**********************/

class CBLUEMath
{
public:
	CBLUEMath(void);
	~CBLUEMath(void);
	
public:
	float GetSinValue(int nAngle);
	float GetCosValue(int nAngle);

private:
	float* m_pSinArray;
	float* m_pCosArray;
};

/**
�������ı������ļ��������ӿ�
���ܣ��������ļ���ʽΪ�����������ļ�
��ϸ���ļ��Լ���ֵ��ӦΪ��ϵʹ�������ļ�
�������ڲ�ʵ�ֿ���ʹ��XMLҲ����ʹ��Windows profile��ʽ�ȵ�(����.ini�ļ�)
*/
class IBLUETextConfigFile
{
public:
	virtual ~IBLUETextConfigFile(void) {}//������COM��Ӧ���ṩ��������

public:
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUEString& strValue) const = 0;//��ȡһ����ֵ�����ַ������أ�
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, int& iValue) const = 0;//��ȡһ����ֵ����������ʽ���أ�
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const = 0;//��ȡһ����ֵ�����ַ������أ�
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, int& iValue) const = 0;//��ȡһ����ֵ����������ʽ���أ�
};


/**
�������ı���ͼƬ
���ܣ�һ��������������Ϸ����ʾͼƬ
˵����ͼƬ��ȫ���������
�汾��1.0.0.1
*/
class CBLUETextImage
{
public:
	CBLUETextImage(const BLUEString& strImage);//��һ���ַ�ֱ�ӹ���ͼƬ
	CBLUETextImage(const int nWidth, const int nHeight);//Ԥ��ͼƬ�Ŀ�͸ߣ�Ĭ��ȫ�Կո�Ϊ����
	CBLUETextImage(void);//Ĭ�Ϲ���
	virtual ~CBLUETextImage(void);

public:
	static bool IsAvailableChar(int ch) {return (ch != NULL && ch != _BLUET(' '));}//�ж�һ���ַ��ǲ�����Ч�ģ����ǲ���ͼ�ε���Ч���֣�
	void SetImage(void);//�Կ�Ϊֵ����ͼƬ����գ�
	void SetImage(const BLUETCHAR ch);//���ض��ַ����ͼƬ
	void SetImage(const int nWidth, const int nHeight);//���ݿ�͸�����ͼ��
	void SetImage(const BLUEString& strImage);//�����ַ���ͼƬ
	int GetWidth() const {return m_nWidth;}//��ȡͼƬ��
	int GetHeight() const {return m_nHeight;}//��ȡͼƬ��

	/**
	���ܣ���ĳһ���꿪ʼ�Ƿ񸲸���һ���ı���ͼƬ
	������desBTI Ŀ���ı���ͼƬ
	������nStartPosX ��ʼλ�ú�����
	   ����nStartPosY ��ʼλ��������
	 ���أ��и��Ƿ���true���෴Ϊfalse
	 ˵�����÷�����Draw�����������������ƣ������������ж��Ƿ��
	 ���������ǵ�ԭͼ����ַ������ԭͼȫ��NULL������Ȼ�϶����Ḳ��
	������ֻҪ��һ���ַ����Ǿͷ���true������Ϊfalse���÷�����������ײ����
	*/
	bool IsOverlay(const CBLUETextImage& desBTI,
		const int nStartPosX = 0,
		const int nStartPosY = 0) const;
	/*
	���ܣ��������һ���ı���ͼƬ
	������desBTI Ŀ���ı���ͼƬ
	������nStartPosX ��ʼ���Ƶ�λ�ú�����
	   ����nStartPosY ��ʼ���Ƶ�λ��������
	*/
	void Draw(CBLUETextImage& desBTI,
		const int nStartPosX = 0,
		const int nStartPosY = 0) const;
	/*
	���ܣ���ͨ�����ֱ������Ļ��)
	*/
	void Draw(void);

private:
	int* m_pData;//ͼƬ����
	int m_nWidth;//ͼƬ��
	int m_nHeight;//ͼƬ��
};


/***************
**ȫ�ֹ��߱���***
ע�����鲻Ҫֱ��ʹ��ȫ�ֱ���
���������ƻ���װ��
***************/
extern CBLUERandom g_random;
extern CBLUEMath g_math;


/**********************
******���й��ߺ���******
**********************/
/**
����������ʼ�������䣨�����ڳ������п�ʼ�����ã�
*/
bool InitToolKit(void);

/**
���������ͷŹ����䣨�����ڳ������ǰ���ã�
*/
void ReleaseToolKit(void);

/**
��������GetRandInt
���ܣ���ȡ�����
*/
inline int GetRandInt()
{
	return ::rand();
}

/**
��������GetRandInt
���ܣ���ȡһ����0��|n|-1�������
������n ���ɵ���������0��|n|-1
ע�⣺n����Ϊ0
*/
inline int GetRandInt(const int n)
{
	return GetRandInt() % n;
}

/*
��������
���ܣ���
˵������
��������
����ֵ����
*/
inline float GetSinValue(const int nAngle)
{
	return ::g_math.GetSinValue(nAngle);
}

/*
��������
���ܣ���
˵������
��������
����ֵ����
*/
inline float GetCosValue(const int nAngle)
{
	return ::g_math.GetCosValue(nAngle);
}

/**
��������������Ļ
*/
inline void ClearScreen(void)
{
	::system("cls");
}

/**
����������ʽ���ַ���
*/
void BLUEFormatString(BLUEString& strDest, BLUELPCTSTR lpstrFormat, ...);

/**
������������ı�
���ܣ����һ���ַ�
������ch �ַ�
*/
inline void OutputText(const BLUETCHAR ch)
{
	::_tprintf(_BLUET("%c"), ch);
}

/**
������������ı�
���ܣ����һ��C��ʽ���ַ���
������lpstr �ַ���
*/
inline void OutputText(BLUELPCTSTR lpstr, bool bNewLine = false)
{
	::_tprintf(_BLUET("%s"), lpstr);
	if (bNewLine)
		::_tprintf(_BLUET("\n"));
}

/**
������������ı�
���ܣ����һ��C++��ʽ���ַ���
������str �ַ�������
*/
inline void OutputText(const BLUEString& str, bool bNewLine = false)
{
	::OutputText(str.c_str(), bNewLine);
}

/**
������������ı�
���ܣ����һ������
*/
inline void OutputTextln()
{
	::OutputText(_BLUET('\n'));
}

/**
���������Զ�����ʽ���һ���ı�
���ܣ��Զ�����ʽ���һ���ı�
������strText �����ı�
*/
void OutputAnimationText(const BLUEString& strText, unsigned long nSleepTime = 30);

/**
����������ȡһ���ַ�
���ܣ���ȡһ���ַ������û�����һ���ַ�����������
*/
inline BLUETCHAR BLUEGetChar(void)
{
	return ::_getch();
}

/**
����������ȡһ���ַ�
���ܣ���ȡһ���ַ�������û�������ַ�������strCheck�У�
�������򷵻ؼ�����ַ�
*/
BLUETCHAR BLUEGetChar(const BLUEString& strCheck);


/**
����������ȡһ���ַ�
���ܣ������һ����ʾ��Ȼ���ȡһ���ַ�������û�������ַ�������strCheck�У�
�������򷵻ؼ�����ַ�
*/
inline BLUETCHAR BLUEGetChar(const BLUEString& strCheck, const BLUEString& strPrompt)
{
	::OutputText(strPrompt);
	return ::BLUEGetChar(strCheck);
}

/**
����������ȡ�����ļ�
���ܣ���ȡһ�������ļ�����IBLUETextConfigFile�ӿڷ���
������lpstrFileName �ļ�·��
���أ�IBLUETextConfigFile�ӿڣ��������NULL��ʾ��ȡʧ�ܣ������ļ������ڣ�
ע�⣺��ȡ���������ʹ�ã���ʹ��ReleaseTextConfigFile�ͷţ���������ڴ�й©
*/
IBLUETextConfigFile* GetTextConfigFile(BLUELPCTSTR lpstrFileName);

/**
���������ͷ������ļ�
���ܣ��ͷ���GetTextConfigFile��ȡ�Ľӿ�
������pBTCF �����ļ��������ӿ�
*/
void ReleaseTextConfigFile(const IBLUETextConfigFile* pBTCF);