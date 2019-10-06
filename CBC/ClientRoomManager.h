#pragma once

#include "ClientRoom.h"
#include <map>

class CClientRoomManager
{
	//���伯�����ͣ���ID�ͷ������ָ�����
	typedef std::map<int, CClientRoom*> ROOMCOLL;

public:
	CClientRoomManager(void);
	~CClientRoomManager(void);

public:
	/*
	��������ͨ������ID�Ż�ȡ�������
	���ܣ���
	˵����
	��������
	����ֵ����
	*/
	CClientRoom* GetRoom(const int nID);

	/*
	����������ȡ�ϣ��£�һ������
	���ܣ���
	˵�������ݵ�ǰ��������ȡ�ϣ��£�һ������
	��������
	����ֵ����
	*/
	CClientRoom* GetPrevRoom(int& nID);
	CClientRoom* GetNextRoom(int& nID);

	/*
	�����������һ������
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	CClientRoom* AddRoom(const int nID, const BLUEString& strName, int nCurClientNum, int nMaxClientNum);

	/*
	��������������з���
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void Clear(void);

	//�����ڲ�������
	void ResetInnerIt(void) {m_itCur = m_roomColl.begin();}

private:
	ROOMCOLL m_roomColl;
	ROOMCOLL::iterator m_itCur;//��ǰ������
};
