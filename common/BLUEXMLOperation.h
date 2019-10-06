#pragma once

#include <atlbase.h>
#include "Toolkit.h"

/**
类名：XML文件操作器
功能：从XML文件中操作（读入，写入等）相应的键值
接口：IBLUETextConfigFile
版本：1.0.0.2
*/
class CBLUEXMLOperation : public IBLUETextConfigFile
{
public:
	/*
	构造
	*/
	CBLUEXMLOperation(void);
	virtual ~CBLUEXMLOperation(void);

public:
	/*
	基本
	*/
	HRESULT Open(BLUELPCTSTR lpstrXMLFilePathName);
	void Close(void);

	/*
	搜索值
	*/
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, CComBSTR& bstrValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUEString& strValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, int& iValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUELPCTSTR lpstrAttr, CComBSTR& bstrValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const;
	HRESULT SearchConfigValue(BLUELPCTSTR lpstrTag, BLUELPCTSTR lpstrAttr, int& iValue) const;

	/*
	IBLUETextConfigFile接口实现
	*/
	bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUEString& strValue) const;//读取一个键值（以字符串返回）
	bool ReadKey(BLUELPCTSTR lpstrKeyName, int& iValue) const;//读取一个键值（以整数形式返回）
	bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const;//读取一个键值（以字符串返回）
	bool ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, int& iValue) const;//读取一个键值（以整数形式返回）

private:
	CComPtr<IXMLDOMDocument> m_spDomDocument;
};
