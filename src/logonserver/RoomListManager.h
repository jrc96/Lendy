#ifndef ROOM_LIST_MANAGER_H
#define ROOM_LIST_MANAGER_H

#include "Struct.h"
#include <map>
#include <vector>

namespace Logon
{
	using namespace Comm;
	typedef std::vector<tagGameRoom*>			FreeRoomItemContainer;
	typedef std::map<uint16, tagGameRoom*>		RoomItemMap;
	typedef RoomItemMap::iterator				RIM_IT;

	typedef std::map<uint16, tagGameKind*>		KindItemMap;
	typedef KindItemMap::iterator				KIM_IT;

	class CRoomListManager
	{
	protected:
		RoomItemMap					m_RoomList;
		KindItemMap					m_KindList;

		FreeRoomItemContainer		m_FreeRoomItemArray;

		//��������
	public:
		//���캯��
		CRoomListManager();
		//��������
		virtual ~CRoomListManager();

	public:
		//��������
		void DisuseRoomItem();

		//���ҽӿ�
	public:
		//��������
		tagGameKind * SearchGameKind(uint16 wKindID);

		//���ҷ���
		tagGameRoom * SearchRoomServer(uint16 wServerID);

		//����ӿ�
	public:
		//���뷿��
		bool InsertGameServer(tagGameRoom * pGameRoom);

	};
}

#endif