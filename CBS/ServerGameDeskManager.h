#pragma once

#include "ServerGameDesk.h"
#include <map>

class CServerGameDeskManager
{
	//��Ϸ���������ͣ���ID����Ϸ������ָ�����
	typedef std::map<int, CServerGameDesk*> GAMEDESKCOLL;

public:
	typedef GAMEDESKCOLL::iterator GAMEDESKITERATOR;

public:
	CServerGameDeskManager(CBLUENetWorkIODataPackageManager& nwIODPM);
	~CServerGameDeskManager(void);

public:
	/*
	��������ͨ����Ϸ��ID�Ż�ȡ��Ϸ������
	���ܣ���
	˵����
	��������
	����ֵ����
	*/
	CServerGameDesk* GetGameDesk(const int nID);

	/*
	���������ⲿ����������Ϸ������������
	���ܣ���
	˵����
	��������
	����ֵ����
	*/
	GAMEDESKITERATOR GetGameDeskIterator(void) {return m_gameDeskColl.begin();}
	bool IsEndGameDeskIterator(const GAMEDESKITERATOR& it) const {return it == m_gameDeskColl.end();}
	CServerGameDesk* GetNextGameDesk(GAMEDESKITERATOR& it, int& nID)
	{
		CServerGameDesk* pServerGameDesk = it->second;
		nID = it->first;
		it++;
		return pServerGameDesk;
	}

	/*
	��������ÿ����Ϸ��ִ���ض�����
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	template<class FN>
	void ForEach(FN fn)
	{
		GAMEDESKITERATOR it = m_gameDeskColl.begin(), itEnd = m_gameDeskColl.end();
		while (it != itEnd)
		{
			fn(it->first, it->second);
			it++;
		}
	}

	/*
	����������һ���û����ͷ����б�
	���ܣ���
	˵������
	��������
	����ֵ����
	*/
	void SendRLToUser(const CServerUser& serverUser);

private:
	GAMEDESKCOLL m_gameDeskColl;
	CBLUENetWorkIODataPackageManager& m_nwIODPM;
};
