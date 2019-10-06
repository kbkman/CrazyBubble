#pragma once

#include "BLUEDef.h"

/**
�������û���
*/
class CUser
{
public:
	CUser(void);
	virtual ~CUser(void);


public:
	/*
	����������ȡ�û���
	*/
	const BLUEString& GetName(void) const {return m_strName;}

	/*
	���������Ƿ�׼���ÿ�ʼ��Ϸ
	���ܣ���
	˵������
	*/
	bool IsPerpareGame(void) const {return m_bPerpareGame;}

	/*
	�������������Ƿ�׼���ÿ�ʼ��Ϸ
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SetPerpareGame(bool bPerpare) {m_bPerpareGame = bPerpare;}


protected:
	/*
	��Ա�����û���
	˵�����û��ĵ�¼��
	*/
	BLUEString m_strName;

	/*
	���������Ƿ�׼���ÿ�ʼ��Ϸ
	���ܣ���
	˵������
	*/
	bool m_bPerpareGame;
};
