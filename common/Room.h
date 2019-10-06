#pragma once

#include "BLUEDef.h"

/**
������������
*/
class CRoom
{
public:
	CRoom(void);
	virtual ~CRoom(void);

public:
	//��������
	const BLUEString& GetName(void) const {return m_strName;}
	void SetName(const BLUEString& strName) {m_strName = strName;}
	//��������
	int GetCurClientNum(void) const {return m_nCurClientNum;}
	int GetMaxClientNum(void) const {return m_nMaxClientNum;}

protected:
	BLUEString m_strName;//��������
	int m_nCurClientNum;//��ǰ����������
	int m_nMaxClientNum;//�����������������
};
