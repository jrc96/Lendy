#include "RoomUserManager.h"
//#include "Header.h"

namespace Game
{
	CRoomUserItem::CRoomUserItem()
	{
		memset(m_szLogonPass, 0, sizeof(m_szLogonPass));
	}
	CRoomUserItem::~CRoomUserItem()
	{
	}
	void * CRoomUserItem::QueryInterface(GGUID uuid)
	{
		return nullptr;
	}
	uint16 CRoomUserItem::GetBindIndex()
	{
		return m_wBindIndex;
	}
	uint64 CRoomUserItem::GetClientAddr()
	{
		return m_dwClientAddr;
	}
	char * CRoomUserItem::GetMachineID()
	{
		return m_szMachineID;
	}
	uint64 CRoomUserItem::GetDBQuestID()
	{
		return uint64();
	}
	uint64 CRoomUserItem::GetLogonTime()
	{
		return uint64();
	}
	uint64 CRoomUserItem::GetInoutIndex()
	{
		return uint64();
	}
	tagUserInfo * CRoomUserItem::GetUserInfo()
	{
		return &m_UserInfo;
	}
	uint8 CRoomUserItem::GetGender()
	{
		return m_UserInfo.cbGender;
	}
	uint32 CRoomUserItem::GetUserID()
	{
		return m_UserInfo.dwUserID;
	}
	uint32 CRoomUserItem::GetGameID()
	{
		return m_UserInfo.dwGameID;
	}
	char * CRoomUserItem::GetNickName()
	{
		return m_UserInfo.szNickName;
	}
	uint16 CRoomUserItem::GetTableID()
	{
		return m_UserInfo.wTableID;
	}
	uint16 CRoomUserItem::GetLastTableID()
	{
		return m_UserInfo.wLastTableID;
	}
	uint16 CRoomUserItem::GetChairID()
	{
		return m_UserInfo.wChairID;
	}
	uint8 CRoomUserItem::GetUserStatus()
	{
		return m_UserInfo.cbUserStatus;
	}
	bool CRoomUserItem::DetachBindStatus()
	{
		//Ч��״̬
		assert(m_UserInfo.dwUserID != 0);
		if (m_UserInfo.dwUserID == 0) return false;

		m_wBindIndex = INVALID_WORD;
		m_bClientReady = false;

		return true;
	}
	SCORE CRoomUserItem::GetUserScore()
	{
		return m_UserInfo.lScore;
	}
	uint16 CRoomUserItem::GetUserWinRate()
	{
		uint32 dwPlayCount = GetUserPlayCount();
		if (dwPlayCount != 0) return (uint16)(m_UserInfo.dwWinCount * 10000 / dwPlayCount);

		return 100;
	}
	uint16 CRoomUserItem::GetUserLostRate()
	{
		uint32 dwPlayCount = GetUserPlayCount();
		if (dwPlayCount != 0) return (uint16)(m_UserInfo.dwLostCount * 10000 / dwPlayCount);

		return 100;
	}
	uint16 CRoomUserItem::GetUserDrawRate()
	{
		uint32 dwPlayCount = GetUserPlayCount();
		if (dwPlayCount != 0) return (uint16)(m_UserInfo.dwDrawCount * 10000 / dwPlayCount);

		return 100;
	}
	uint16 CRoomUserItem::GetUserFleeRate()
	{
		uint32 dwPlayCount = GetUserPlayCount();
		if (dwPlayCount != 0) return (uint16)(m_UserInfo.dwFleeCount * 10000 / dwPlayCount);

		return 100;
	}
	uint16 CRoomUserItem::GetUserPlayCount()
	{
		return m_UserInfo.dwWinCount + m_UserInfo.dwLostCount + m_UserInfo.dwDrawCount + m_UserInfo.dwFleeCount;
	}

	bool CRoomUserItem::ContrastLogonPass(const char * szPassword)
	{
		return strcmp(m_szLogonPass, szPassword) == 0;
	}

	bool CRoomUserItem::IsTrusteeUser()
	{
		return m_bTrusteeUser;
	}
	void CRoomUserItem::SetTrusteeUser(bool bTrusteeUser)
	{
		m_bTrusteeUser = bTrusteeUser;
	}
	bool CRoomUserItem::IsClientReady()
	{
		return m_bClientReady;
	}
	void CRoomUserItem::SetClientReady(bool bClientReady)
	{
		m_bClientReady = bClientReady;
	}
	bool CRoomUserItem::SetUserStatus(uint8 cbUserStatus, uint16 wTableID, uint16 wChairID)
	{
		//Ч��״̬
		if (m_UserInfo.dwUserID == 0) return false;

		//��¼��Ϣ
		uint16 wOldTableID = m_UserInfo.wTableID;
		uint16 wOldChairID = m_UserInfo.wChairID;

		//���ñ���
		m_UserInfo.wTableID = wTableID;
		m_UserInfo.wChairID = wChairID;
		m_UserInfo.cbUserStatus = cbUserStatus;
		if (cbUserStatus == US_PLAYING)m_UserInfo.wLastTableID = wTableID;

		//����״̬
		assert(m_pIRoomUserItemSink != nullptr);
		if (m_pIRoomUserItemSink != nullptr) m_pIRoomUserItemSink->OnEventUserItemStatus(this, wOldTableID, wOldChairID);
	}

	bool CRoomUserItem::SetUserParameter(uint32 dwClientAddr, uint16 wBindIndex, const char szMachineID[LEN_MACHINE_ID], bool bClientReady)
	{
		assert(m_UserInfo.dwUserID != 0);
		if (m_UserInfo.dwUserID == 0) return false;

		m_bClientReady = bClientReady;
		m_wBindIndex = wBindIndex;
		m_dwClientAddr = dwClientAddr;
		snprintf(m_szMachineID, sizeof(m_szMachineID), "%s", szMachineID);

		return true;
	}

	bool CRoomUserItem::WriteUserScore(SCORE & lScore)
	{
		//Ч��״̬
		if (m_UserInfo.dwUserID == 0L) return false;

		//�޸�ʤ��
		if (lScore > 0)
		{
			++m_UserInfo.dwWinCount;
		}
		else if (lScore < 0)
		{
			++m_UserInfo.dwLostCount;
		}
		else
		{
			++m_UserInfo.dwDrawCount;
		}

		//���û���
		m_UserInfo.lScore += lScore;

		//����״̬
		assert(m_pIRoomUserItemSink != nullptr);
		if (m_pIRoomUserItemSink != nullptr) m_pIRoomUserItemSink->OnEventUserItemScore(this, 0);
		return true;
	}

	void CRoomUserItem::ResetUserItem()
	{
	}

	/////////////////////////////////////////////////////////////
	CRoomUserManager::CRoomUserManager()
	{
	}
	CRoomUserManager::~CRoomUserManager()
	{
	}
	void * CRoomUserManager::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(IRoomUserManager, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(IRoomUserManager, uuid);
		return nullptr;
	}
	bool CRoomUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
	{
		//���ýӿ�
		if (pIUnknownEx != nullptr)
		{
			//��ѯ�ӿ�
			assert(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IRoomUserItemSink) != nullptr);
			m_pIRoomUserItemSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IRoomUserItemSink);

			//�ɹ��ж�
			if (m_pIRoomUserItemSink == nullptr) return false;
		}
		else m_pIRoomUserItemSink = nullptr;
		return true;
	}
	IRoomUserItem * CRoomUserManager::SearchUserItem(uint32 dwUserID)
	{
		RUIM_IT it = m_UserItemMap.find(dwUserID);
		if (it != m_UserItemMap.end())
		{
			return it->second;
		}
		return nullptr;
	}
	IRoomUserItem * CRoomUserManager::SearchUserItem(char * pszNickName)
	{
		for (RUIM_IT it = m_UserItemMap.begin(); it != m_UserItemMap.end(); ++it)
		{
			if (strcmp(it->second->GetNickName(), pszNickName) == 0)
			{
				return it->second;
			}
		}
		return nullptr;
	}

	CRoomUserManager::CRoomUserItemMap & CRoomUserManager::TraverseRoomUserList()
	{
		return m_UserItemMap;
	}
	uint32 CRoomUserManager::GetAndroidCount()
	{
		return uint32();
	}
	uint32 CRoomUserManager::GetUserItemCount()
	{
		return uint32();
	}
	bool CRoomUserManager::DeleteUserItem()
	{
		return false;
	}
	bool CRoomUserManager::DeleteUserItem(IRoomUserItem * pIServerUserItem)
	{
		//Ч�����
		assert((pIServerUserItem != nullptr) && (pIServerUserItem->GetUserStatus() == US_NULL));
		if ((pIServerUserItem == nullptr) || (pIServerUserItem->GetUserStatus() != US_NULL)) return false;

		//��������
		uint32 dwUserID = pIServerUserItem->GetUserID();
		RUIM_IT it = m_UserItemMap.find(dwUserID);
		if (it != m_UserItemMap.end())
		{
			it->second->ResetUserItem();

			m_FreeUserItem.emplace_back(it->second);
			m_UserItemMap.erase(dwUserID);
			return true;
		}
		
		//�������
		assert(nullptr);
		return false;
	}
	bool CRoomUserManager::InsertUserItem(IRoomUserItem ** pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus &UserInfoPlus)
	{
		//��������
		CRoomUserItem * pServerUserItem = nullptr;
		if (m_FreeUserItem.empty())
		{
			try
			{
				pServerUserItem = new CRoomUserItem;
			}
			catch (...)
			{
				assert(nullptr);
				return false;
			}
		}
		else
		{
			pServerUserItem = m_FreeUserItem.back();
			m_FreeUserItem.pop_back();

			pServerUserItem->ResetUserItem();
		}

		//���ýӿ�
		pServerUserItem->m_pIRoomUserItemSink = m_pIRoomUserItemSink;
		memcpy(&pServerUserItem->m_UserInfo, &UserInfo, sizeof(UserInfo));
		
		//������Ϣ
		pServerUserItem->m_wBindIndex = UserInfoPlus.wBindIndex;
		pServerUserItem->m_dwClientAddr = UserInfoPlus.dwClientAddr;
		snprintf(pServerUserItem->m_szMachineID, sizeof(pServerUserItem->m_szMachineID), "%s", UserInfoPlus.szMachineID);

		//��������
		pServerUserItem->m_bClientReady = false;
		pServerUserItem->m_bTrusteeUser = false;
		
		m_UserItemMap[UserInfo.dwUserID] = pServerUserItem;

		//���ñ���
		*pIServerUserResult = pServerUserItem;

		return true;
	}
}