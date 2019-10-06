#include "BLUEDef.h"
#include "BLUEDebug.h"
#include "ToolKit.h"
#include "BLUEXMLOperation.h"
#include <math.h>

///////////全局变量//////////////
CBLUERandom g_random;
CBLUEMath g_math;

///////////类////////////////////////
//CBLUEMath
CBLUEMath::CBLUEMath(void)
{
	m_pSinArray = new float[361];
	m_pCosArray = new float[361];

	for (int i = 0; i < 361; i++)
	{
		float fAngle = 3.14159265358979323846f / 180 * i;
		m_pSinArray[i] = ::sin(fAngle);
		m_pCosArray[i] = ::cos(fAngle);
	}
}

CBLUEMath::~CBLUEMath(void)
{
	delete[] m_pCosArray;
	delete[] m_pSinArray;
}

float CBLUEMath::GetSinValue(int nAngle)
{
	BLUEASSERT(nAngle >= -360 && nAngle <= 360);
	if (nAngle < 0)
		return -m_pSinArray[-nAngle];

	return m_pSinArray[nAngle];
}

float CBLUEMath::GetCosValue(int nAngle)
{
	BLUEASSERT(nAngle >= -360 && nAngle <= 360);
	if (nAngle < 0)
		return m_pCosArray[-nAngle];

	return m_pCosArray[nAngle];
}

//CBLUETextImage类
CBLUETextImage::CBLUETextImage(const BLUEString& strImage) :
m_pData(NULL)
{
	SetImage(strImage);
}

CBLUETextImage::CBLUETextImage(const int nWidth, const int nHeight) :
m_pData(NULL)
{
	SetImage(nWidth, nHeight);
}

CBLUETextImage::CBLUETextImage(void) :
m_pData(NULL)
{
}

CBLUETextImage::~CBLUETextImage(void)
{
	if (m_pData)
		delete[] m_pData;
}

void CBLUETextImage::SetImage(void)
{
	if (m_pData)
		::memset(m_pData, NULL, m_nWidth * m_nHeight * sizeof(int));
}

void CBLUETextImage::SetImage(BLUETCHAR ch)
{
	if (m_pData)
	{
		//将指字符内容填充到内存
		const int row = m_nHeight;//有多少行
		const int col = m_nWidth;//有多少列
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				m_pData[i * m_nWidth + j] = ch;
	}
}

void CBLUETextImage::SetImage(const int nWidth, const int nHeight)
{
	//释放上一次内存
	if (m_pData)
		delete[] m_pData;

	BLUEASSERT(nWidth > 0 && nHeight > 0);

	//成员赋值
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	//计算出需要的内存容量
	const size_t nLength = nWidth * nHeight;
	//申请足够的容量
	m_pData = new int[nLength];
	//默认都填NULL
	SetImage();
}

void CBLUETextImage::SetImage(const BLUEString& strImage)
{
	//释放上一次内存
	if (m_pData)
		delete[] m_pData;

	//如果字符串为空，则无需要绘制，有效数据全部设置为0
	if (strImage.empty())
	{
		BLUETRACE(_BLUET("SetImage的字符串为空\n"));
		m_pData = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
		return;
	}


	//字符串长度
	const size_t nStrLength = (size_t)strImage.length();

	//先确认高度和宽度，以便申请空间
	int nLastWidth = 0;
	m_nWidth = 1;
	m_nHeight = 1;
	for (size_t i = 0; i < nStrLength; i++)
	{
		if (strImage[i] == _BLUET('\n'))
		{
			m_nHeight++;
			nLastWidth = 0;
		}
		else
		{
			if (++nLastWidth > m_nWidth)
				m_nWidth = nLastWidth;
		}
	}

	//根据高度和宽度申请内存
	m_pData = new int[m_nWidth * m_nHeight];
	SetImage();

	//将字符串内容填充到内存
	int nCurRow = 0;
	int nCurCol = 0;
	for (size_t i = 0; i < nStrLength; i++)
	{
		if (strImage[i] == _BLUET('\n'))//换行不作填充
		{
			nCurRow++;//换行后行数递增
			nCurCol = 0;//列数归0
		}
		else
		{
			//如果复制进去的空格
			m_pData[nCurRow * m_nWidth + nCurCol] = strImage[i];
			nCurCol++;//列数递增，行数不变
		}
	}
}

bool CBLUETextImage::IsOverlay(
							const CBLUETextImage& desBTI,
							const int nStartPosX,
							const int nStartPosY
							) const
{
	if (m_pData && desBTI.m_pData)
	{
		const int row = m_nHeight;//有多少行
		const int col = m_nWidth;//有多少列
		for (int i = 0; i < row; i++)
		{
			int nDesPosY = i + nStartPosY;//得到目标图像上的绘制点Y坐标
			if (nDesPosY > desBTI.m_nHeight - 1)//如果已经超出图像右则，则直接退出，不再绘制
				break;
			if (nDesPosY < 0)//如果超出图像左侧，则忽略该点
				continue;

			for (int j = 0; j < col; j++)
			{
				int nDesPosX = j + nStartPosX;//得到目标图像上的绘制点X坐标
				if (nDesPosX > desBTI.m_nWidth - 1)//如果已经超出图像右则，则换一行
					break;
				if (nDesPosX < 0)//如果超出图像左侧，则忽略该点
					continue;

				//获取源与目标图像当前绘制点的索引
				int nSrcIndex = i * m_nWidth + j;
				int nDesIndex = nDesPosY * desBTI.m_nWidth + nDesPosX;

				//判断是否覆盖
				if (IsAvailableChar(desBTI.m_pData[nDesIndex]) && IsAvailableChar(m_pData[nSrcIndex]))
					return true;
			}
		}
	}

	return false;
}

void CBLUETextImage::Draw(
							CBLUETextImage& desBTI,
							const int nStartPosX,
							const int nStartPosY
							) const
{
	if (m_pData && desBTI.m_pData)
	{
		const int row = m_nHeight;//有多少行
		const int col = m_nWidth;//有多少列
		for (int i = 0; i < row; i++)
		{
			int nDesPosY = i + nStartPosY;//得到目标图像上的绘制点Y坐标
			if (nDesPosY > desBTI.m_nHeight - 1)//如果已经超出图像右则，则直接退出，不再绘制
				break;
			if (nDesPosY < 0)//如果超出图像左侧，则忽略该点
				continue;

			for (int j = 0; j < col; j++)
			{
				int nDesPosX = j + nStartPosX;//得到目标图像上的绘制点X坐标
				if (nDesPosX > desBTI.m_nWidth - 1)//如果已经超出图像右则，则换一行
					break;
				if (nDesPosX < 0)//如果超出图像左侧，则忽略该点
					continue;

				//获取源与目标图像当前绘制点的索引
				int nSrcIndex = i * m_nWidth + j;
				int nDesIndex = nDesPosY * desBTI.m_nWidth + nDesPosX;

				//复制绘制点
				if (IsAvailableChar(m_pData[nSrcIndex]))//有效字符才复制，不然保持原样
					desBTI.m_pData[nDesIndex] = m_pData[nSrcIndex];
			}
		}
	}
}

void CBLUETextImage::Draw(void)
{
	if (m_pData)
	{
		BLUEASSERT(m_nWidth > 0 && m_nHeight > 0);
		//BLUETRACE(_BLUET("width=%d, height=%d\n"), GetWidth(), GetHeight());

		const int row = m_nHeight;//有多少行
		const int col = m_nWidth;//有多少列
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				BLUETCHAR c = (BLUETCHAR)m_pData[i * m_nWidth + j];
				if (IsAvailableChar(c))
					::OutputText(c);
				else
					::OutputText(_BLUET(" "));
			}
			::OutputTextln();
		}
	}
}

//////////全局函数////////////////
bool InitToolKit(void)
{
#ifdef UNICODE
	::setlocale(LC_ALL, "chs");//如果是宽字符则需要设置该句，否则无法正确显示中文等字符
#endif
	::srand((unsigned int)time(0));
	return SUCCEEDED(::CoInitialize(NULL));
}

void ReleaseToolKit(void)
{
	::CoUninitialize();
}

BLUETCHAR BLUEGetChar(const BLUEString& strCheck)
{
	while (true)
	{
		BLUETCHAR chResult = ::BLUEGetChar();
		for (size_t i = 0; i < strCheck.length(); i++)
		{
			if (strCheck[i] == chResult)
			{
				::_putch(strCheck[i]);
				return chResult;
			}
		}
	}
}

void BLUEFormatString(BLUEString& strDest, BLUELPCTSTR lpstrFormat, ...)
{
	BLUETCHAR strTempDest[1024] = {0};
	va_list ptr;
	va_start(ptr, lpstrFormat);
	::_vstprintf_s(strTempDest, lpstrFormat, ptr);
	va_end(ptr);
	strDest = strTempDest;
}

void OutputAnimationText(const BLUEString& strText, unsigned long nSleepTime)
{
	for (size_t i = 0; i < strText.length(); i++)
	{
		::OutputText(strText[i]);
		::Sleep(nSleepTime);//休眠，实现动画效果
	}
}

IBLUETextConfigFile* GetTextConfigFile(BLUELPCTSTR lpstrFileName)
{
	CBLUEXMLOperation* pXMLOp = new CBLUEXMLOperation();

	//打开配置文件
	if (pXMLOp->Open(lpstrFileName) == S_OK)
		return pXMLOp;

	//如果打开失败，则直接释放内存并返回NULL
	delete pXMLOp;
	return NULL;
}

void ReleaseTextConfigFile(const IBLUETextConfigFile* pBTCF)
{
	BLUEASSERT(pBTCF);
	delete pBTCF;
}