#ifndef GLOBAL_INFO_MANAGER_H
#define GLOBAL_INFO_MANAGER_H

#include "Define.h"
#include "Struct.h"
#include <map>
#include <vector>

namespace Correspond
{
	using namespace Comm;

	typedef std::vector<uint16>				GameRoomIDArray;
	typedef GameRoomIDArray::iterator		G_IT;
	typedef std::vector<uint32>				UserIDArray;
	typedef UserIDArray::iterator			U_IT;

	//�û���Ϣ
	struct tagGlobalUserItem
	{
		tagUserInfo						gUserInfo;
		GameRoomIDArray					vGameRoomID;		

		//����״̬
		void UpdateStatus(const uint16 wTableID, const uint16 wChairID, const uint8 cbUserStatus)
		{
			gUserInfo.wTableID = wTableID;
			gUserInfo.wChairID = wChairID;
			
			gUserInfo.cbUserStatus = cbUserStatus;
		}
	};

	struct tagGlobalRoomItem
	{
		tagGameRoom						gGameRoom;
		UserIDArray						vUserID;
	};

	//ȫ����Ϣ
	class CGlobalInfoManager
	{
		typedef std::map<uint16, tagGameLogon*>	ActiveGameLogonContainer;
		typedef std::map<uint16, tagGameLogon*>::iterator	AGLC_IT;
		typedef std::vector<tagGameLogon*>		FreeGameLogonContainer;

	public:
		typedef std::map<uint16, tagGlobalRoomItem*>			ActiveGameRoomContainer;
		typedef std::map<uint16, tagGlobalRoomItem*>::iterator	AGRC_IT;
		typedef std::vector<tagGlobalRoomItem*>					FreeGameRoomContainer;

		typedef std::map<uint32, tagGlobalUserItem*>			ActiveUserContainer;
		typedef std::map<uint32, tagGlobalUserItem*>::iterator	AUC_IT;
		typedef std::vector<tagGlobalUserItem*>					FreeUserContainer;

		//�㳡����
	public:
		//ɾ���㳡
		bool DeleteLogonItem(uint16 wPlazaID);
		//����㳡
		bool ActiveLogonItem(tagGameLogon &GamePlaza);

		//�������
	public:
		//ɾ������
		bool DeleteRoomItem(uint16 wServerID);
		//�����
		bool ActiveRoomItem(tagGameRoom & GameServer);
		//��������
		ActiveGameRoomContainer &TraverseGameRoom();
		//Ѱ�ҷ���
		tagGameRoom * SearchRoomItem(uint16 wServerID);

		//�û�����
	public:
		//ɾ���û�
		bool DeleteUserItem(uint32 dwUserID, uint16 wServerID);
		//�����û�
		bool ActiveUserItem(tagGlobalUserItem &GlobalUserInfo, uint16 wServerID);
		
		//�û�����
	public:
		//Ѱ���û�
		tagGlobalUserItem * SearchUserItemByUserID(uint32 dwUserID);

	private:
		tagGameLogon * CreateGlobalLogonItem();
		tagGlobalRoomItem * CreateGlobalRoomItem();
		tagGlobalUserItem * CreateGlobalUserItem();

		//�ͷź���
	private:
		//�ͷ��û�
		bool FreeGlobalUserItem(tagGlobalUserItem * pGlobalUserItem);

	protected:
		ActiveGameLogonContainer			m_ActiveGameLogon;
		FreeGameLogonContainer				m_FreeGameLogon;

		ActiveGameRoomContainer				m_ActiveGameRoom;
		FreeGameRoomContainer				m_FreeGameRoom;

		ActiveUserContainer					m_ActiveUserItem;
		FreeUserContainer					m_FreeUserItem;
	};

}

#endif