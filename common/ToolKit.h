#pragma once

/***
各种工具类和工具函数的集合
版本：1.0.0.1
作者：BLUE
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
所有预定义和宏
***/

/**********************
所有工具类
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
类名：文本配置文件操作器接口
功能：操作以文件格式为基础的配置文件
详细：文件以键和值对应为关系使用配置文件
　　　内部实现可能使用XML也可能使用Windows profile格式等等(类似.ini文件)
*/
class IBLUETextConfigFile
{
public:
	virtual ~IBLUETextConfigFile(void) {}//非正规COM，应该提供虚拟析构

public:
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUEString& strValue) const = 0;//读取一个键值（以字符串返回）
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, int& iValue) const = 0;//读取一个键值（以整数形式返回）
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const = 0;//读取一个键值（以字符串返回）
	virtual bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, int& iValue) const = 0;//读取一个键值（以整数形式返回）
};


/**
类名：文本型图片
功能：一般用于文字类游戏中显示图片
说明：图片完全由文字组成
版本：1.0.0.1
*/
class CBLUETextImage
{
public:
	CBLUETextImage(const BLUEString& strImage);//用一串字符直接构造图片
	CBLUETextImage(const int nWidth, const int nHeight);//预设图片的宽和高，默认全以空格为内容
	CBLUETextImage(void);//默认构造
	virtual ~CBLUETextImage(void);

public:
	static bool IsAvailableChar(int ch) {return (ch != NULL && ch != _BLUET(' '));}//判断一个字符是不是有效的（即是不是图形的有效部分）
	void SetImage(void);//以空为值设置图片（清空）
	void SetImage(const BLUETCHAR ch);//以特定字符填充图片
	void SetImage(const int nWidth, const int nHeight);//根据宽和高设置图像
	void SetImage(const BLUEString& strImage);//设置字符串图片
	int GetWidth() const {return m_nWidth;}//获取图片宽
	int GetHeight() const {return m_nHeight;}//获取图片高

	/**
	功能：从某一坐标开始是否覆盖另一幅文本形图片
	参数：desBTI 目标文本形图片
	　　　nStartPosX 开始位置横坐标
	   　　nStartPosY 开始位置纵坐标
	 返回：有覆盖返回true，相反为false
	 说明：该方法与Draw方法（带参数）相似，但它是用来判断是否会
	 　　　覆盖掉原图里的字符，如果原图全是NULL，那自然肯定不会覆盖
	　　　只要有一个字符覆盖就返回true，否则为false。该方法可用于碰撞测试
	*/
	bool IsOverlay(const CBLUETextImage& desBTI,
		const int nStartPosX = 0,
		const int nStartPosY = 0) const;
	/*
	功能：输出到另一个文本形图片
	参数：desBTI 目标文本形图片
	　　　nStartPosX 开始绘制的位置横坐标
	   　　nStartPosY 开始绘制的位置纵坐标
	*/
	void Draw(CBLUETextImage& desBTI,
		const int nStartPosX = 0,
		const int nStartPosY = 0) const;
	/*
	功能：普通输出（直接在屏幕上)
	*/
	void Draw(void);

private:
	int* m_pData;//图片数据
	int m_nWidth;//图片宽
	int m_nHeight;//图片高
};


/***************
**全局工具变量***
注：建议不要直接使用全局变量
　　避免破坏封装性
***************/
extern CBLUERandom g_random;
extern CBLUEMath g_math;


/**********************
******所有工具函数******
**********************/
/**
函数名：初始化工具箱（建议在程序运行开始处调用）
*/
bool InitToolKit(void);

/**
函数名：释放工具箱（建议在程序结束前调用）
*/
void ReleaseToolKit(void);

/**
函数名：GetRandInt
功能：获取随机数
*/
inline int GetRandInt()
{
	return ::rand();
}

/**
函数名：GetRandInt
功能：获取一个从0到|n|-1的随机数
参数：n 生成的随机数会从0到|n|-1
注意：n不能为0
*/
inline int GetRandInt(const int n)
{
	return GetRandInt() % n;
}

/*
函数名：
功能：略
说明：无
参数：无
返回值：无
*/
inline float GetSinValue(const int nAngle)
{
	return ::g_math.GetSinValue(nAngle);
}

/*
函数名：
功能：略
说明：无
参数：无
返回值：无
*/
inline float GetCosValue(const int nAngle)
{
	return ::g_math.GetCosValue(nAngle);
}

/**
函数名：清理屏幕
*/
inline void ClearScreen(void)
{
	::system("cls");
}

/**
函数名：格式化字符串
*/
void BLUEFormatString(BLUEString& strDest, BLUELPCTSTR lpstrFormat, ...);

/**
函数名：输出文本
功能：输出一个字符
参数：ch 字符
*/
inline void OutputText(const BLUETCHAR ch)
{
	::_tprintf(_BLUET("%c"), ch);
}

/**
函数名：输出文本
功能：输出一个C形式的字符串
参数：lpstr 字符串
*/
inline void OutputText(BLUELPCTSTR lpstr, bool bNewLine = false)
{
	::_tprintf(_BLUET("%s"), lpstr);
	if (bNewLine)
		::_tprintf(_BLUET("\n"));
}

/**
函数名：输出文本
功能：输出一个C++形式的字符串
参数：str 字符串对象
*/
inline void OutputText(const BLUEString& str, bool bNewLine = false)
{
	::OutputText(str.c_str(), bNewLine);
}

/**
函数名：输出文本
功能：输出一个换行
*/
inline void OutputTextln()
{
	::OutputText(_BLUET('\n'));
}

/**
函数名：以动画形式输出一串文本
功能：以动画形式输出一串文本
参数：strText 动画文本
*/
void OutputAnimationText(const BLUEString& strText, unsigned long nSleepTime = 30);

/**
函数名：获取一个字符
功能：获取一个字符，当用户输入一个字符后立即返回
*/
inline BLUETCHAR BLUEGetChar(void)
{
	return ::_getch();
}

/**
函数名：获取一个字符
功能：获取一个字符，如果用户键入的字符包含在strCheck中，
　　　则返回键入的字符
*/
BLUETCHAR BLUEGetChar(const BLUEString& strCheck);


/**
函数名：获取一个字符
功能：先输出一段提示，然后获取一个字符，如果用户键入的字符包含在strCheck中，
　　　则返回键入的字符
*/
inline BLUETCHAR BLUEGetChar(const BLUEString& strCheck, const BLUEString& strPrompt)
{
	::OutputText(strPrompt);
	return ::BLUEGetChar(strCheck);
}

/**
函数名：获取配置文件
功能：获取一个配置文件，以IBLUETextConfigFile接口返回
参数：lpstrFileName 文件路径
返回：IBLUETextConfigFile接口，如果返回NULL表示获取失败（可能文件不存在）
注意：获取后如果不再使用，请使用ReleaseTextConfigFile释放，否则可能内存泄漏
*/
IBLUETextConfigFile* GetTextConfigFile(BLUELPCTSTR lpstrFileName);

/**
函数名：释放配置文件
功能：释放由GetTextConfigFile获取的接口
参数：pBTCF 配置文件操作器接口
*/
void ReleaseTextConfigFile(const IBLUETextConfigFile* pBTCF);