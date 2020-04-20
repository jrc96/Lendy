#ifndef GLOBAL_INFO_MANAGER_H
#define GLOBAL_INFO_MANAGER_H

#include "Define.h"
#include "Struct.h"
#include <map>
#include <vector>

namespace Correspond
{
	using namespace Comm;

	//ȫ����Ϣ
	class CGlobalInfoManager
	{
		typedef std::map<uint16, tagGameLogon*>	ActiveGameLogonContainer;
		typedef std::map<uint16, tagGameLogon*>::iterator	AGLC_IT;
		typedef std::vector<tagGameLogon*>		FreeGameLogonContainer;

	public:
		typedef std::map<uint16, tagGameRoom*>	ActiveGameRoomContainer;
		typedef std::map<uint16, tagGameRoom*>::iterator	AGRC_IT;
		typedef std::vector<tagGameRoom*>		FreeGameRoomContainer;

		//�㳡����
	public:
		//ɾ���㳡
		bool DeleteLogonItem(uint16 wPlazaID);
		//����㳡
		bool ActiveLogonItem(uint16 wBindIndex,tagGameLogon &GamePlaza);

		//�������
	public:
		//ɾ������
		bool DeleteRoomItem(uint16 wServerID);
		//�����
		bool ActiveRoomItem(uint16 wBindIndex, tagGameRoom & GameServer);
		//��������
		ActiveGameRoomContainer &TraverseGameRoom();
		//Ѱ�ҷ���
		tagGameRoom * SearchRoomItem(uint16 wServerID);

	private:
		tagGameLogon * CreateGlobalLogonItem();
		tagGameRoom * CreateGlobalRoomItem();

	protected:
		ActiveGameLogonContainer			m_ActiveGameLogon;
		FreeGameLogonContainer				m_FreeGameLogon;

		ActiveGameRoomContainer				m_ActiveGameRoom;
		FreeGameRoomContainer				m_FreeGameRoom;
	};

}

#endif