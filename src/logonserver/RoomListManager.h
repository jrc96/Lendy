#ifndef ROOM_LIST_MANAGER_H
#define ROOM_LIST_MANAGER_H

#include "Struct.h"
#include <map>

namespace Logon
{
	using namespace comm;
	typedef std::map<uint16, tagGameServer*>	RoomItemMap;
	typedef RoomItemMap::iterator				RIM_IT;

	typedef std::map<uint16, tagGameKind*>		KindItemMap;
	typedef KindItemMap::iterator				KIM_IT;

	class CRoomListManager
	{
	protected:
		RoomItemMap			m_RoomList;
		KindItemMap			m_KindList;

		//��������
	public:
		//���캯��
		CRoomListManager();
		//��������
		virtual ~CRoomListManager();

		//���ҽӿ�
	public:
		//��������
		tagGameKind * SearchGameKind(uint16 wKindID);

		//���ҷ���
		tagGameServer * SearchRoomServer(uint16 wServerID);

		

	};
}

#endif