#pragma once

#include <atlbase.h>
#include "Toolkit.h"

/**
������XML�ļ�������
���ܣ���XML�ļ��в��������룬д��ȣ���Ӧ�ļ�ֵ
�ӿڣ�IBLUETextConfigFile
�汾��1.0.0.2
*/
class CBLUEXMLOperation : public IBLUETextConfigFile
{
public:
	/*
	����
	*/
	CBLUEXMLOperation(void);
	virtual ~CBLUEXMLOperation(void);

public:
	/*
	����
	*/
	HRESULT Open(BLUELPCTSTR lpstrXMLFilePathName);
	void Close(void);

	/*
	����ֵ
	*/
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, CComBSTR& bstrValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUEString& strValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, int& iValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUELPCTSTR lpstrAttr, CComBSTR& bstrValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUELPCTSTR lpstrAttr, int& iValue) const;

	/*
	IBLUETextConfigFile�ӿ�ʵ��
	*/
	bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUEString& strValue) const;//��ȡһ����ֵ�����ַ������أ�
	bool ReadKey(BLUELPCTSTR lpstrKeyName, int& iValue) const;//��ȡһ����ֵ����������ʽ���أ�
	bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const;//��ȡһ����ֵ�����ַ������أ�
	bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, int& iValue) const;//��ȡһ����ֵ����������ʽ���أ�

private:
	CComPtr<IXMLDOMDocument> m_spDomDocument;
};
