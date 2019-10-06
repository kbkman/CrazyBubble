#include "BLUEDef.h"
#include "BLUEDebug.h"
#include "ToolKit.h"
#include "BLUEXMLOperation.h"
#include <math.h>

///////////ȫ�ֱ���//////////////
CBLUERandom g_random;
CBLUEMath g_math;

///////////��////////////////////////
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

//CBLUETextImage��
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
		//��ָ�ַ�������䵽�ڴ�
		const int row = m_nHeight;//�ж�����
		const int col = m_nWidth;//�ж�����
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				m_pData[i * m_nWidth + j] = ch;
	}
}

void CBLUETextImage::SetImage(const int nWidth, const int nHeight)
{
	//�ͷ���һ���ڴ�
	if (m_pData)
		delete[] m_pData;

	BLUEASSERT(nWidth > 0 && nHeight > 0);

	//��Ա��ֵ
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	//�������Ҫ���ڴ�����
	const size_t nLength = nWidth * nHeight;
	//�����㹻������
	m_pData = new int[nLength];
	//Ĭ�϶���NULL
	SetImage();
}

void CBLUETextImage::SetImage(const BLUEString& strImage)
{
	//�ͷ���һ���ڴ�
	if (m_pData)
		delete[] m_pData;

	//����ַ���Ϊ�գ�������Ҫ���ƣ���Ч����ȫ������Ϊ0
	if (strImage.empty())
	{
		BLUETRACE(_BLUET("SetImage���ַ���Ϊ��\n"));
		m_pData = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
		return;
	}


	//�ַ�������
	const size_t nStrLength = (size_t)strImage.length();

	//��ȷ�ϸ߶ȺͿ�ȣ��Ա�����ռ�
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

	//���ݸ߶ȺͿ�������ڴ�
	m_pData = new int[m_nWidth * m_nHeight];
	SetImage();

	//���ַ���������䵽�ڴ�
	int nCurRow = 0;
	int nCurCol = 0;
	for (size_t i = 0; i < nStrLength; i++)
	{
		if (strImage[i] == _BLUET('\n'))//���в������
		{
			nCurRow++;//���к���������
			nCurCol = 0;//������0
		}
		else
		{
			//������ƽ�ȥ�Ŀո�
			m_pData[nCurRow * m_nWidth + nCurCol] = strImage[i];
			nCurCol++;//������������������
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
		const int row = m_nHeight;//�ж�����
		const int col = m_nWidth;//�ж�����
		for (int i = 0; i < row; i++)
		{
			int nDesPosY = i + nStartPosY;//�õ�Ŀ��ͼ���ϵĻ��Ƶ�Y����
			if (nDesPosY > desBTI.m_nHeight - 1)//����Ѿ�����ͼ��������ֱ���˳������ٻ���
				break;
			if (nDesPosY < 0)//�������ͼ����࣬����Ըõ�
				continue;

			for (int j = 0; j < col; j++)
			{
				int nDesPosX = j + nStartPosX;//�õ�Ŀ��ͼ���ϵĻ��Ƶ�X����
				if (nDesPosX > desBTI.m_nWidth - 1)//����Ѿ�����ͼ��������һ��
					break;
				if (nDesPosX < 0)//�������ͼ����࣬����Ըõ�
					continue;

				//��ȡԴ��Ŀ��ͼ��ǰ���Ƶ������
				int nSrcIndex = i * m_nWidth + j;
				int nDesIndex = nDesPosY * desBTI.m_nWidth + nDesPosX;

				//�ж��Ƿ񸲸�
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
		const int row = m_nHeight;//�ж�����
		const int col = m_nWidth;//�ж�����
		for (int i = 0; i < row; i++)
		{
			int nDesPosY = i + nStartPosY;//�õ�Ŀ��ͼ���ϵĻ��Ƶ�Y����
			if (nDesPosY > desBTI.m_nHeight - 1)//����Ѿ�����ͼ��������ֱ���˳������ٻ���
				break;
			if (nDesPosY < 0)//�������ͼ����࣬����Ըõ�
				continue;

			for (int j = 0; j < col; j++)
			{
				int nDesPosX = j + nStartPosX;//�õ�Ŀ��ͼ���ϵĻ��Ƶ�X����
				if (nDesPosX > desBTI.m_nWidth - 1)//����Ѿ�����ͼ��������һ��
					break;
				if (nDesPosX < 0)//�������ͼ����࣬����Ըõ�
					continue;

				//��ȡԴ��Ŀ��ͼ��ǰ���Ƶ������
				int nSrcIndex = i * m_nWidth + j;
				int nDesIndex = nDesPosY * desBTI.m_nWidth + nDesPosX;

				//���ƻ��Ƶ�
				if (IsAvailableChar(m_pData[nSrcIndex]))//��Ч�ַ��Ÿ��ƣ���Ȼ����ԭ��
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

		const int row = m_nHeight;//�ж�����
		const int col = m_nWidth;//�ж�����
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

//////////ȫ�ֺ���////////////////
bool InitToolKit(void)
{
#ifdef UNICODE
	::setlocale(LC_ALL, "chs");//����ǿ��ַ�����Ҫ���øþ䣬�����޷���ȷ��ʾ���ĵ��ַ�
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
		::Sleep(nSleepTime);//���ߣ�ʵ�ֶ���Ч��
	}
}

IBLUETextConfigFile* GetTextConfigFile(BLUELPCTSTR lpstrFileName)
{
	CBLUEXMLOperation* pXMLOp = new CBLUEXMLOperation();

	//�������ļ�
	if (pXMLOp->Open(lpstrFileName) == S_OK)
		return pXMLOp;

	//�����ʧ�ܣ���ֱ���ͷ��ڴ沢����NULL
	delete pXMLOp;
	return NULL;
}

void ReleaseTextConfigFile(const IBLUETextConfigFile* pBTCF)
{
	BLUEASSERT(pBTCF);
	delete pBTCF;
}