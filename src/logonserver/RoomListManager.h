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

	typedef std::vector<tagGameKind*>			FreeKindItemContainer;
	typedef std::map<uint16, tagGameKind*>		KindItemMap;
	typedef KindItemMap::iterator				KIM_IT;

	class CRoomListManager
	{
	protected:
		RoomItemMap					m_RoomList;
		KindItemMap					m_KindList;

		FreeRoomItemContainer		m_FreeRoomItemArray;
		FreeKindItemContainer		m_FreeKindItemArray;

		//��������
	public:
		//���캯��
		CRoomListManager();
		//��������
		virtual ~CRoomListManager();

	public:
		//��������
		void DisuseRoomItem();

		//�����ӿ�
	public:
		RoomItemMap &TraverseRoomList();

		KindItemMap &TraverseKindList();

		//���ҽӿ�
	public:
		//��������
		tagGameKind * SearchGameKind(uint16 wKindID);

		//���ҷ���
		tagGameRoom * SearchGameRoom(uint16 wServerID);

		//����ӿ�
	public:
		//���뷿��
		bool InsertGameRoom(tagGameRoom * pGameRoom);
		//���뷿��
		bool InsertGameKind(tagGameKind * pGameKind);

		//ɾ���ӿ�
	public:
		//ɾ������
		bool DeleteGameRoom(uint16 wServerID);
	};
}

#endif