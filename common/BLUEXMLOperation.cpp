#include "BLUEXMLOperation.h"


CBLUEXMLOperation::CBLUEXMLOperation(void)
{
}

CBLUEXMLOperation::~CBLUEXMLOperation(void)
{
	Close();
}

HRESULT CBLUEXMLOperation::Open(BLUELPCTSTR lpstrXMLFilePathName)
{
	CComVariant varFileName;
	VARIANT_BOOL isSuccessful;
	HRESULT hr;

	//读取配置文件
	varFileName = lpstrXMLFilePathName;
	if (FAILED(hr = m_spDomDocument.CoCreateInstance(CLSID_DOMDocument)))
		return hr;
	if (FAILED(hr = m_spDomDocument->put_async(VARIANT_FALSE)))//使用同步
		return hr;
	if (FAILED(hr = m_spDomDocument->load(varFileName, &isSuccessful)))//读取文件
		return hr;
	if (isSuccessful == VARIANT_FALSE)
		return hr;

	return S_OK;
}

void CBLUEXMLOperation::Close(void)
{
}

HRESULT CBLUEXMLOperation::SearchConfigValue(
	BLUELPCTSTR lpstrTag,
	CComBSTR& bstrValue
	) const
{
	CComPtr<IXMLDOMNodeList> spXMLDomNodeList;
	CComPtr<IXMLDOMNode> spXMLDomNode;
	HRESULT hr;
	long l;

	if (FAILED(hr = m_spDomDocument->getElementsByTagName(CComBSTR(lpstrTag), &spXMLDomNodeList)))
		return hr;

	if (FAILED(hr = spXMLDomNodeList->get_length(&l)))
		return hr;

	if (l <= 0) return S_FALSE;
	
	if (FAILED(hr = spXMLDomNodeList->get_item(0, &spXMLDomNode)))
		return hr;

	if (FAILED(hr = spXMLDomNode->get_text(&bstrValue)))
		return hr;

	return S_OK;
}

HRESULT CBLUEXMLOperation::SearchConfigValue(
	BLUELPCTSTR lpstrTag,
	BLUEString& strValue
	) const
{
	CComBSTR bstr;
	HRESULT hr;

	if ((hr = SearchConfigValue(lpstrTag, bstr)) == S_OK)
	{
#ifdef UNICODE
		strValue = bstr;//BSTR本身就是Unicode，所以在Unicode环境下不需要转换
#else
		strValue = ::CW2A(bstr);
#endif
	}
	return hr;

}

HRESULT CBLUEXMLOperation::SearchConfigValue(
	BLUELPCTSTR lpstrTag,
	int& iValue
	) const
{
	CComBSTR bstr;
	CComVariant v;
	HRESULT hr;

	if (FAILED(hr = SearchConfigValue(lpstrTag, bstr)))
		return hr;

	v = bstr;
	if (FAILED(hr = v.ChangeType(VT_I4)))
		return hr;

	iValue = v.lVal;
	return hr;
}

HRESULT CBLUEXMLOperation::SearchConfigValue(
	BLUELPCTSTR lpstrTag,
	BLUELPCTSTR lpstrAttr,
	CComBSTR& bstrValue
	) const
{
	CComPtr<IXMLDOMNodeList> spXMLDomNodeList;
	CComPtr<IXMLDOMNode> spXMLDomNode, spXMLDomAttrNode;
	CComPtr<IXMLDOMNamedNodeMap> spAttrMap;
	HRESULT hr;
	long l;

	if (FAILED(hr = m_spDomDocument->getElementsByTagName(CComBSTR(lpstrTag), &spXMLDomNodeList)))
		return hr;

	if (FAILED(hr = spXMLDomNodeList->get_length(&l)))
		return hr;

	if (l <= 0) return S_FALSE;
	
	if (FAILED(hr = spXMLDomNodeList->get_item(0, &spXMLDomNode)))
		return hr;

	if (FAILED(hr = spXMLDomNode->get_attributes(&spAttrMap)))
		return hr;

	if (FAILED(hr = spAttrMap->getNamedItem(CComBSTR(lpstrAttr), &spXMLDomAttrNode)))
		return hr;

	if (hr == S_FALSE)//不存在该属性
		return hr;

	if (FAILED(hr = spXMLDomAttrNode->get_text(&bstrValue)))
		return hr;

	return hr;
}

HRESULT CBLUEXMLOperation::SearchConfigValue(
	BLUELPCTSTR lpstrTag,
	BLUELPCTSTR lpstrAttr,
	BLUEString& strValue
	) const
{
	CComBSTR bstr;
	HRESULT hr;

	if ((hr = SearchConfigValue(lpstrTag, lpstrAttr, bstr)) == S_OK)
	{
#ifdef UNICODE
		strValue = bstr;//BSTR本身就是Unicode，所以在Unicode环境下不需要转换
#else
		strValue = ::CW2A(bstr);
#endif
	}
	return hr;
}

HRESULT CBLUEXMLOperation::SearchConfigValue(
	BLUELPCTSTR lpstrTag,
	BLUELPCTSTR lpstrAttr,
	int& iValue
	) const
{
	CComBSTR bstr;
	CComVariant v;
	HRESULT hr;

	if (FAILED(hr = SearchConfigValue(lpstrTag, lpstrAttr, bstr)))
		return hr;

	v = bstr;
	if (FAILED(hr = v.ChangeType(VT_I4)))
		return hr;

	iValue = v.lVal;
	return hr;
}

bool CBLUEXMLOperation::ReadKey(BLUELPCTSTR lpstrKeyName, BLUEString& strValue) const
{
	return SearchConfigValue(lpstrKeyName, strValue) == S_OK;
}

bool CBLUEXMLOperation::ReadKey(BLUELPCTSTR lpstrKeyName, int& iValue) const
{
	return SearchConfigValue(lpstrKeyName, iValue) == S_OK;
}

bool CBLUEXMLOperation::ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, BLUEString& strValue) const
{
	return SearchConfigValue(lpstrKeyName, lpstrAttr, strValue) == S_OK;
}

bool CBLUEXMLOperation::ReadKey(BLUELPCTSTR lpstrKeyName, BLUELPCTSTR lpstrAttr, int& iValue) const
{
	return SearchConfigValue(lpstrKeyName, lpstrAttr, iValue) == S_OK;
}