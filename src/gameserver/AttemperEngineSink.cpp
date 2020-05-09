#include "AttemperEngineSink.h"
#include "Timer.h"
#include "ServiceUnits.h"
#include "CMD_Correspond.h"
#include "CMD_LogonServer.h"
#include "CMD_GameServer.h"
#include "Implementation/LogonDatabase.h"
#include "Log.h"
#include "INIReader.h"
#include "StringUtility.h"
#include "BigNumber.h"

#define MAX_LINK_COUNT 512
#define OPEN_SWITCH		1
#define CLIENT_SWITCH	0

namespace Game
{
	using namespace LogComm;

#define IDI_CONNECT_CORRESPOND		(IDI_MAIN_MODULE_START+3)			//����ʱ��

#define LOGON_FAILURE(linkid, errorcode) \
	if (OnLogonFailure(linkid, errorcode)) { \
		return true;	\
	}

	CAttemperEngineSink::CAttemperEngineSink():
		m_bNeekCorrespond(true),
		m_pBindParameter(nullptr),
		m_pGameAddressOption(nullptr),
		m_pGameServiceOption(nullptr)
	{
		
	}

	CAttemperEngineSink::~CAttemperEngineSink()
	{
		for (size_t i = 0; i < m_TableFrameArray.size(); ++i)
		{
			PDELETE(m_TableFrameArray[i]);
		}
		m_TableFrameArray.clear();
	}

	void CAttemperEngineSink::Release()
	{
	}

	void * CAttemperEngineSink::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(IMainServiceFrame, uuid);
		QUERY_INTERFACE(IAttemperEngineSink, uuid);
		QUERY_INTERFACE(IRoomUserItemSink, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(IAttemperEngineSink, uuid);
		return nullptr;
	}

	bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
	{
		m_pBindParameter = new tagBindParameter[MAX_LINK_COUNT];
		memset(m_pBindParameter, 0, sizeof(tagBindParameter)*MAX_LINK_COUNT);

		InitTableFrameArray();

		if (!m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IRoomUserItemSink)))
		{
			assert(nullptr);
			return false;
		}
		return true;
	}

	bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
	{
		m_pITCPSocketService = nullptr;
		return false;
	}

	bool CAttemperEngineSink::OnEventTCPSocketLink(uint16 wServiceID, int iErrorCode)
	{
		//Э������
		if (wServiceID == NETWORK_CORRESPOND)
		{
			//�����ж�
			if (iErrorCode != 0)
			{
				int iConnectTime = sConfigMgr->GetInt32("LocalNet", "ConnectTime", 5);
				LOG_INFO("server.game", "Correspond server connection failed [ %d ], will reconnect in %d seconds", iErrorCode, iConnectTime);
				m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, iConnectTime * 1000, 1);
				return false;
			}

			//��ʾ��Ϣ
			LOG_INFO("server.game", "����ע����Ϸ��¼������...");

			//��������
			CMD_CS_C_RegisterRoom RegisterRoom;
			memset(&RegisterRoom, 0, sizeof(RegisterRoom));

			//���ñ���
			RegisterRoom.wKindID			= m_pGameServiceOption->wKindID;
			RegisterRoom.wServerID			= m_pGameServiceOption->wServerID;
			RegisterRoom.wServerPort		= m_pGameServiceOption->wServerPort;
			RegisterRoom.lCellScore			= m_pGameServiceOption->lCellScore;
			RegisterRoom.lEnterScore		= m_pGameServiceOption->lMinEnterScore;
			RegisterRoom.dwOnLineCount		= m_ServerUserManager.GetUserItemCount();
			RegisterRoom.dwFullCount		= m_pGameServiceOption->wMaxPlayer;
			RegisterRoom.wTableCount		= m_pGameServiceOption->wTableCount;
			RegisterRoom.dwServerRule		= 0;//vCustomRule

			snprintf(RegisterRoom.szServerName, sizeof(RegisterRoom.szServerName), "%s", m_pGameServiceOption->strGameName);
			snprintf(RegisterRoom.szServerAddr, sizeof(RegisterRoom.szServerAddr), "%s", m_pGameAddressOption->szIP);

			//��������
			m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_ROOM, &RegisterRoom, sizeof(RegisterRoom));

			return true;
		}

		return true;
	}

	bool CAttemperEngineSink::OnEventTCPSocketShut(uint16 wServiceID, uint8 cbShutReason)
	{
		//Э������
		if (wServiceID == NETWORK_CORRESPOND)
		{
			//�����ж�
			if (m_bNeekCorrespond)
			{
				//������ʾ
				int iConnectTime = sConfigMgr->GetInt32("LocalNet", "ConnectTime", 5);
				LOG_INFO("server.game", "The connection to the correspond server is closed and will reconnect in %d seconds", iConnectTime);

				//����ʱ��
				assert(m_pITimerEngine != nullptr);
				m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, iConnectTime * 1000, 1);
			}
			return true;
		}

		return false;
	}

	bool CAttemperEngineSink::OnEventTCPSocketRead(uint16 wServiceID, TCP_Command Command, void * pData, uint16 wDataSize)
	{
		//Э������
		if (wServiceID == NETWORK_CORRESPOND)
		{
			switch (Command.wMainCmdID)
			{
				case MDM_CS_REGISTER:		//ע�����
				{
					return OnTCPSocketMainRegister(Command.wSubCmdID, pData, wDataSize);
				}
				case MDM_CS_ROOM_INFO:	//������Ϣ
				{
					return OnTCPSocketMainServiceInfo(Command.wSubCmdID, pData, wDataSize);
				}
			}
		}
		return false;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkBind(uint32 dwClientAddr, uint32 dwSocketID)
	{
		//��ȡ����
		assert(dwSocketID < MAX_LINK_COUNT);
		if (dwSocketID >= MAX_LINK_COUNT) return false;

		//��������
		tagBindParameter * pBindParameter = (m_pBindParameter + dwSocketID);

		//���ñ���
		pBindParameter->dwSocketID = dwSocketID;
		pBindParameter->dwClientAddr = dwClientAddr;
		return true;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkShut(uint32 dwClientAddr, uint32 dwSocketID)
	{
		//��������
		uint16 wBindIndex = LOWORD(dwSocketID);
		tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
		if (pBindParameter == nullptr) return false;

		//��ȡ�û�
		IRoomUserItem * pIServerUserItem = pBindParameter->pIServerUserItem;
		uint16 wTableID = INVALID_WORD;

		try
		{
			//�û�����
			if (pIServerUserItem != nullptr)
			{
				//��������
				wTableID = pIServerUserItem->GetTableID();

				//���ߴ���
				if (wTableID != INVALID_TABLE)
				{
					//�����
					pIServerUserItem->DetachBindStatus();
					if (wTableID < m_pGameServiceOption->wTableCount)
					{
						//����֪ͨ
						assert(wTableID < m_pGameServiceOption->wTableCount);
						//m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
					}
					else //�Ȳ�������ʲô����
					{

					}
				}
				else
				{
					pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
				}
			}
		}
		catch (...)
		{
			LOG_INFO("server.game", "�ر������쳣: wTableID=%d", wTableID);
		}

		//�����Ϣ
		memset((m_pBindParameter + dwSocketID), 0, sizeof(tagBindParameter));
		return false;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		switch (Command.wMainCmdID)
		{
			case MDM_GR_LOGON:			//��¼����
			{
				return OnTCPNetworkMainLogon(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			}
			case MDM_GR_USER:
			{
				return OnTCPNetworkMainUser(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			}
			case MDM_GF_FRAME:
			{
				return OnTCPNetworkMainFrame(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			}
		}
		return false;
	}

	bool CAttemperEngineSink::OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize)
	{
		switch (wControlID)
		{
			case SUC_LOAD_DB_GAME_LIST:
			{
				//�¼�֪ͨ
				ControlResult ControlResult;
				ControlResult.cbSuccess = 1;
				SrvUnitsMgr->PostControlRequest(UDC_LOAD_DB_LIST_RESULT, &ControlResult, sizeof(ControlResult));
				return true;
			}
			case SUC_CONNECT_CORRESPOND:
			{
				//��������
				m_pITCPSocketService->Connect(sConfigMgr->Get("CorrespondNet", "BindIP", "127.0.0.1"), sConfigMgr->GetInt32("CorrespondNet", "Port", 8600));
				return true;
			}
		}
		return false;
	}

	bool CAttemperEngineSink::OnEventTimer(uint32 dwTimerID)
	{
		//ʱ�䴦��
		try 
		{
			//����ʱ��
			if ((dwTimerID >= IDI_TABLE_MODULE_START) && (dwTimerID <= IDI_TABLE_MODULE_FINISH))
			{
				//���Ӻ���
				uint32 dwTableTimerID = dwTimerID - IDI_TABLE_MODULE_START;
				uint16 wTableID = (uint16)(dwTableTimerID / IDI_TABLE_MODULE_RANGE);

				//ʱ��Ч��
				if (wTableID >= (uint16)m_TableFrameArray.size())
				{
					assert(nullptr);
					return false;
				}

				//ʱ��֪ͨ
				CTableFrame * pTableFrame = m_TableFrameArray[wTableID];
				return pTableFrame->OnEventTimer(dwTableTimerID%IDI_TABLE_MODULE_RANGE);
			}

			if (IDI_CONNECT_CORRESPOND)
			{
				std::string strCorrespondAddress = sConfigMgr->Get("CorrespondNet", "BindIP", "127.0.0.1");
				uint16 wCorrespondPort = sConfigMgr->GetInt32("CorrespondNet", "Port", 8600);
				m_pITCPSocketService->Connect(strCorrespondAddress, wCorrespondPort);
				LOG_INFO("server.game", "Connecting to the correspond server [ %s:%d ]", strCorrespondAddress, wCorrespondPort);
				return true;
			}
		}
		catch (...)
		{

		}
		return false;
	}

	bool CAttemperEngineSink::SendRoomMessage(char * lpszMessage, uint16 wType)
	{
		return false;
	}

	bool CAttemperEngineSink::SendGameMessage(char * lpszMessage, uint16 wType)
	{
		return false;
	}

	bool CAttemperEngineSink::SendRoomMessage(IRoomUserItem * pIServerUserItem, const char * lpszMessage, uint16 wType)
	{
		//Ч�����
		assert(pIServerUserItem != NULL);
		if (pIServerUserItem == NULL) return false;

		//��������
		if (pIServerUserItem->GetBindIndex() != INVALID_WORD)
		{
			//��������
			CMD_CM_SystemMessage SystemMessage;
			memset(&SystemMessage, 0, sizeof(SystemMessage));

			//��������
			SystemMessage.wType = wType;
			SystemMessage.wLength = strlen(lpszMessage) + 1;

			std::wstring wstrMessage = Util::StringUtility::StringToWString(lpszMessage);
			swprintf(SystemMessage.szString, sizeof(SystemMessage.szString), L"%s", wstrMessage.c_str());

			//��������
			uint32 dwUserIndex = pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter = GetBindParameter(dwUserIndex);

			//��������
			uint16 wHeadSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString);
			uint16 wSendSize = wHeadSize + SystemMessage.wLength;

			//�����û�
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);
			return true;
		}

		return false;
	}

	bool CAttemperEngineSink::SendGameMessage(IRoomUserItem * pIServerUserItem, const char * lpszMessage, uint16 wType)
	{
		return false;
	}

	bool CAttemperEngineSink::SendRoomMessage(uint32 dwSocketID, const char * lpszMessage, uint16 wType, bool bAndroid)
	{
		return false;
	}

	bool CAttemperEngineSink::SendData(uint32 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		//�����û�
		m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
		return true;
	}

	bool CAttemperEngineSink::SendData(IRoomUserItem * pIServerUserItem, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		//Ч�����
		assert(pIServerUserItem != nullptr);
		if (pIServerUserItem == nullptr) return false;

		//�����û�
		uint16 wBindIndex = pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
		m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
		return true;
	}

	bool CAttemperEngineSink::SendDataBatch(uint16 wCmdTable, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		CRoomUserManager::CRoomUserItemMap ruim = m_ServerUserManager.TraverseRoomUserList();
		for (CRoomUserManager::RUIM_IT it = ruim.begin(); it != ruim.end(); ++it)
		{
			IRoomUserItem *pITargetUserItem = it->second;
			uint16 wTagerTableID = pITargetUserItem->GetTableID();

			//״̬����
			if ((pITargetUserItem->GetUserStatus() >= US_SIT) && pITargetUserItem->IsClientReady())
			{
				if (wCmdTable != INVALID_WORD && wTagerTableID != wCmdTable)continue;
			}

			//������Ϣ
			SendData(pITargetUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

		return false;
	}

	void CAttemperEngineSink::UnLockScoreLockUser(uint32 dwUserID, uint32 dwInoutIndex, uint32 dwLeaveReason)
	{
		PerformUnlockScore(dwUserID, dwInoutIndex, dwLeaveReason);
	}

	bool CAttemperEngineSink::OnEventUserItemStatus(IRoomUserItem * pIServerUserItem, uint16 wOldTableID, uint16 wOldChairID)
	{
		//Ч�����
		assert(pIServerUserItem != nullptr);
		if (pIServerUserItem == nullptr) return false;
		
		//��������
		CMD_GR_UserStatus UserStatus;
		memset(&UserStatus, 0, sizeof(UserStatus));

		//��������
		UserStatus.dwUserID = pIServerUserItem->GetUserID();
		UserStatus.UserStatus.wTableID = pIServerUserItem->GetTableID();
		UserStatus.UserStatus.wChairID = pIServerUserItem->GetChairID();
		UserStatus.UserStatus.cbUserStatus = pIServerUserItem->GetUserStatus();

		//�û���Ϣ
		uint16 wTableID = pIServerUserItem->GetTableID();
		uint8 cbUserStatus = pIServerUserItem->GetUserStatus();

	
		if (cbUserStatus >= US_SIT)
		{
			if (wOldTableID == INVALID_TABLE && cbUserStatus == US_SIT)
			{
			}
			else if (m_pGameServiceOption->wChairCount >= MAX_CHAIR)
			{
				SendDataBatch(wOldTableID, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
			}
			SendDataBatch(wTableID, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}
		else
		{
			SendDataBatch(wOldTableID, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}

		//�������״̬
		CMD_CS_C_UserStatus  Status;
		Status.dwUserID = UserStatus.dwUserID;
		Status.cbUserStatus = UserStatus.UserStatus.cbUserStatus;
		Status.wKindID = m_pGameServiceOption->wKindID;
		Status.wServerID = m_pGameServiceOption->wServerID;
		Status.wTableID = UserStatus.UserStatus.wTableID;
		Status.wChairID = UserStatus.UserStatus.wChairID;
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_STATUS, &Status, sizeof(Status));

		//�뿪�ж�
		if (pIServerUserItem->GetUserStatus() == US_NULL)
		{
			//��ȡ��
			uint16 wBindIndex = pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

			//�������
			if (pBindParameter != nullptr)
			{
				//�󶨴���
				if (pBindParameter->pIServerUserItem == pIServerUserItem)
				{
					pBindParameter->pIServerUserItem = nullptr;
				}

				//�ж�����
				if (pBindParameter->dwSocketID != 0)
				{
					m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
				}
			}

			//�뿪����
			OnEventUserLogout(pIServerUserItem, 0);
		}
		return true;
	}

	bool CAttemperEngineSink::SendUserInfoPacket(IRoomUserItem * pIServerUserItem, uint32 dwSocketID)
	{
		//Ч�����
		assert(pIServerUserItem != nullptr);
		if (pIServerUserItem == nullptr) return false;

		//��������
		uint8 cbBuffer[SOCKET_TCP_PACKET] = {};
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
		
		CMD_GR_MobileUserInfoHead *pUserInfoHead = (CMD_GR_MobileUserInfoHead *)cbBuffer;

		//�û�����
		pUserInfoHead->wFaceID = pUserInfo->wFaceID;
		pUserInfoHead->dwGameID = pUserInfo->dwGameID;
		pUserInfoHead->dwUserID = pUserInfo->dwUserID;

		//�û�����	
		pUserInfoHead->cbGender = pUserInfo->cbGender;

		//�û�״̬
		pUserInfoHead->wTableID = pUserInfo->wTableID;
		pUserInfoHead->wChairID = pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus = pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->dwWinCount = pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount = pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount = pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount = pUserInfo->dwFleeCount;

		//�û��ɼ�
		pUserInfoHead->lScore = pUserInfo->lScore;

		std::wstring wstrNickName = Util::StringUtility::StringToWString(pUserInfo->szNickName);
		swprintf(pUserInfoHead->szNickName, sizeof(pUserInfoHead->szNickName), L"%s", wstrNickName.c_str());

		if (dwSocketID == INVALID_DWORD)
		{
			SendUserInfoPacketBatch(pIServerUserItem, INVALID_DWORD);
		}
		else
		{
			uint16 wHeadSize = sizeof(CMD_GR_MobileUserInfoHead);
			SendData(dwSocketID, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wHeadSize);
		}
		return true;
	}

	bool CAttemperEngineSink::SendUserInfoPacketBatch(IRoomUserItem * pIServerUserItem, uint32 dwSocketID)
	{
		//Ч�����
		assert(pIServerUserItem != NULL);
		if (pIServerUserItem == NULL) return false;

		//��������
		uint8 cbBuffer[SOCKET_TCP_PACKET] = {};
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
		CMD_GR_MobileUserInfoHead * pUserInfoHead = (CMD_GR_MobileUserInfoHead *)cbBuffer;

		//�û�����
		pUserInfoHead->wFaceID = pUserInfo->wFaceID;
		pUserInfoHead->dwGameID = pUserInfo->dwGameID;
		pUserInfoHead->dwUserID = pUserInfo->dwUserID;

		//�û�����
		pUserInfoHead->cbGender = pUserInfo->cbGender;

		//�û�״̬
		pUserInfoHead->wTableID = pUserInfo->wTableID;
		pUserInfoHead->wChairID = pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus = pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->dwWinCount = pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount = pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount = pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount = pUserInfo->dwFleeCount;

		//�û��ɼ�
		pUserInfoHead->lScore = pUserInfo->lScore;

		std::wstring wstrNickName = Util::StringUtility::StringToWString(pUserInfo->szNickName);
		swprintf(pUserInfoHead->szNickName, sizeof(pUserInfoHead->szNickName), L"%s", wstrNickName.c_str());
	
		//��������
		uint16 wHeadSize = sizeof(CMD_GR_MobileUserInfoHead);
		if (dwSocketID == INVALID_DWORD)
		{
			SendDataBatch(pUserInfo->wTableID, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wHeadSize);
		}
		else
		{
			SendData(dwSocketID, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wHeadSize);
		}

		return true;
	}

	bool CAttemperEngineSink::InitTableFrameArray()
	{
		tagTableFrameParameter TableFrameParameter;
		memset(&TableFrameParameter, 0, sizeof(TableFrameParameter));

		//�������
		TableFrameParameter.pIMainServiceFrame = this;
		TableFrameParameter.pITimerEngine = m_pITimerEngine;
		TableFrameParameter.pIGameServiceManager = m_pIGameServiceManager;

		//���ò���
		TableFrameParameter.pGameServiceOption = m_pGameServiceOption;

		//��������
		m_TableFrameArray.resize(m_pGameServiceOption->wTableCount);

		//��������
		for (uint16 i = 0; i < m_pGameServiceOption->wTableCount; ++i)
		{
			//��������
			m_TableFrameArray[i] = new CTableFrame;

			//��������
			if (!m_TableFrameArray[i]->InitializationFrame(i, TableFrameParameter))
			{
				return false;
			}
		}
		return true;
	}

	bool CAttemperEngineSink::OnTCPSocketMainRegister(uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		switch (wSubCmdID)
		{
			case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
			{
				//��������
				CMD_CS_S_RegisterFailure * pRegisterFailure = (CMD_CS_S_RegisterFailure *)pData;

				//Ч�����
				assert(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
				if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

				//�رմ���
				m_bNeekCorrespond = false;
				m_pITCPSocketService->CloseSocket();

				//��ʾ��Ϣ
				LOG_INFO("server.game", "%s", pRegisterFailure->szDescribeString);

				//�¼�֪ͨ
				ControlResult ControlResult;
				ControlResult.cbSuccess = 0;
				SrvUnitsMgr->PostControlRequest(UDC_CORRESPOND_RESULT, &ControlResult, sizeof(ControlResult));
				return true;
			}
		}

		return true;
	}

	bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		switch (wSubCmdID)
		{
			case SUB_CS_S_ROOM_INFO:		//������Ϣ
			{
				return true;
			}
			case SUB_CS_S_ROOM_ONLINE:	//��������
			{
				//Ч�����
				assert(wDataSize == sizeof(CMD_CS_S_RoomOnLine));
				if (wDataSize != sizeof(CMD_CS_S_RoomOnLine)) return false;

				//��������
				CMD_CS_S_RoomOnLine * pServerOnLine = (CMD_CS_S_RoomOnLine *)pData;
				return true;
			}
			case SUB_CS_S_ROOM_INSERT:	//�������
			{
				return true;
			}
			case SUB_CS_S_ROOM_FINISH:	//�������
			{
				//�¼�����
				ControlResult ControlResult;
				ControlResult.cbSuccess = 1;
				SrvUnitsMgr->PostControlRequest(UDC_CORRESPOND_RESULT, &ControlResult, sizeof(ControlResult));
				return true;
			}
		}
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainUser(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		switch (wSubCmdID)
		{
			case SUB_GR_USER_SITDOWN:		//�û�����
			{
				return OnTCPNetworkSubUserSitDown(pData, wDataSize, dwSocketID);
			}
			case SUB_GR_USER_STANDUP:		//�û�����
			{
				return OnTCPNetworkSubUserStandUp(pData, wDataSize, dwSocketID);
			}
		}
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainLogon(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		switch (wSubCmdID)
		{
			case SUB_GR_LOGON_MOBILE:      //�ο͵�¼
			{
				return OnTCPNetworkSubMBLogonVisitor(pData, wDataSize, dwSocketID);
			}
		}
		return false;
	}
	bool CAttemperEngineSink::OnTCPNetworkMainFrame(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		//��ȡ��Ϣ
		bool bResult = false;
		uint16 wBindIndex = LOWORD(dwSocketID);
		IRoomUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);

		//�û�Ч��
		assert(pIServerUserItem != nullptr);
		if (pIServerUserItem == nullptr) return false;

		//�������
		uint16 wTableID = pIServerUserItem->GetTableID();
		uint16 wChairID = pIServerUserItem->GetChairID();

		if (wSubCmdID == SUB_GF_BJL_GAME_STATUS)
		{
			bResult = QuerryGameRoomRecordTime(pIServerUserItem);
		}
		else
		{
			if (wTableID != INVALID_TABLE)
			{
				//��Ϣ���� 
				CTableFrame * pTableFrame = m_TableFrameArray[wTableID];
				bResult = pTableFrame->OnEventSocketFrame(wSubCmdID, pData, wDataSize, pIServerUserItem);
			}
		}

		if (!bResult)
		{
			LOG_INFO("server.game", "MDM_GF_FRAME �������� false");
		}
		return bResult;
	}
	bool CAttemperEngineSink::OnTCPNetworkSubMBLogonVisitor(void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		//Ч�����
		assert(wDataSize >= sizeof(CMD_GR_LogonMobile));
		if (wDataSize < sizeof(CMD_GR_LogonMobile))return false;

		//��������
		uint16 wBindIndex = LOWORD(dwSocketID);
		IRoomUserItem	 * pIBindUserItem = GetBindUserItem(wBindIndex);
		tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
		
		//�ظ��ж�
		if (pBindParameter == nullptr || pIBindUserItem)
		{
			assert(nullptr);
			return false;
		}

		//������Ϣ
		CMD_GR_LogonMobile * pLogonMobile = (CMD_GR_LogonMobile *)pData;
		if (pBindParameter->dwClientAddr != 0 && pLogonMobile->szPassword[0] == L'0')
		{
			//����ʧ��
			SendLogonFailure(LogonError[LEC_PW_EMPTY].c_str(), LEC_PW_EMPTY, dwSocketID);
			return true;
		}

		//��������
		pBindParameter->cbClientKind = LinkType::LT_MOBILE;

		//��������
		std::string strLogonPass = Util::StringUtility::WStringToString(pLogonMobile->szPassword);
		std::string strMachineID = Util::StringUtility::WStringToString(pLogonMobile->szMachineID);

		IRoomUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogonMobile->dwUserID);
		if ((pIServerUserItem != NULL) && (pIServerUserItem->ContrastLogonPass(strLogonPass.c_str()) == true))
		{
			SwitchUserItemConnect(pIServerUserItem, strMachineID.c_str(), wBindIndex);
			return true;
		}

		///////////////////////////////DB��ѯ//////////////////////////////////
		//�Ż�TODO...																			
		//1�����ڸĳɶ࿪��¼�����ӣ�������Ϣ�ӵ�¼�շ��������Ͳ����ٴβ�ѯDB���ε�¼��			
		//2������������ʵ�ܶ಻��Ҫ��Ϣ���Լ�¼redis��Ҹ����񲻷�æʱ��up�����ݿ⡣	
		///////////////////////////////////////////////////////////////////////
		
		//��ѯ�û���Ϣ
		PreparedStatement *stmt = LogonDatabasePool.GetPreparedStatement(LOGON_SEL_VISITOR_ACCOUNT);
		stmt->SetString(0, "");
		PreparedQueryResult result = LogonDatabasePool.Query(stmt);
		
		if (!result)
		{
			//SendLogonFailure();
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return true;
		}

		//��ȡ��Ϸ��Ϣ
		Field* field = result->Fetch();
		int id = field[0].GetInt32();
		std::string account = field[1].GetString();
		std::string username = field[2].GetString();
		std::string sha_pass_hash = field[3].GetString();
		std::string face_url = field[4].GetString();
		int limit = field[5].GetInt8();
		uint64 score = field[6].GetUInt64();

		////�����ж�
		//if ((pBindParameter->pIServerUserItem != nullptr) || (pBindParameter->dwSocketID != dwSocketID))
		//{
		//	PerformUnlockScore(pLogonMobile->dwUserID, 0/*pLogonMobile->dwInoutIndex*/, LER_NORMAL);
		//	return true;
		//}

		//��ͷ���
		if ((m_pGameServiceOption->lMinEnterScore != 0) && (score < m_pGameServiceOption->lMinEnterScore))
		{
			SendLogonFailure(LogonError[LEC_ROOM_ENTER_SCORE_LESS].c_str(), LEC_ROOM_ENTER_SCORE_LESS, pBindParameter->dwSocketID);
			PerformUnlockScore(pLogonMobile->dwUserID, 0/* pDBOLogonSuccess->dwInoutIndex*/, LER_SERVER_CONDITIONS);
			return true;
		}

		//�����ж�
		uint16 wMaxPlayer = m_pGameServiceOption->wMaxPlayer;
		uint32 dwOnlineCount = m_ServerUserManager.GetUserItemCount();
		if (dwOnlineCount > (uint32)(wMaxPlayer))
		{
			SendLogonFailure(LogonError[LEC_ROOM_FULL].c_str(), LEC_ROOM_FULL, pBindParameter->dwSocketID);
			PerformUnlockScore(pLogonMobile->dwUserID, 0/* pDBOLogonSuccess->dwInoutIndex*/, LER_SERVER_FULL);
			return true;
		}

		//////////////////////////////////�û�����
		std::string strAnsiNickName;
		Util::StringUtility::Utf8ToConsole(username, strAnsiNickName);

		std::string strAnsiPW;
		Util::StringUtility::Utf8ToConsole(sha_pass_hash, strAnsiPW);

		tagUserInfo UserInfo;
		tagUserInfoPlus UserInfoPlus;
		memset(&UserInfo, 0, sizeof(UserInfo));
		memset(&UserInfoPlus, 0, sizeof(UserInfoPlus));

		//��������
		UserInfo.wFaceID = 0;
		UserInfo.dwUserID = pLogonMobile->dwUserID;
		UserInfo.dwGameID = pLogonMobile->dwUserID;
		snprintf(UserInfo.szNickName, sizeof(UserInfo.szNickName), "%s", strAnsiNickName.c_str());

		//�û�����
		UserInfo.cbGender = 0;

		//״̬����
		UserInfo.cbUserStatus = US_FREE;
		UserInfo.wTableID = INVALID_TABLE;
		UserInfo.wChairID = INVALID_CHAIR;

		//������Ϣ
		UserInfo.lScore = 100;

		//��¼��Ϣ
		UserInfoPlus.dwLogonTime = (uint32)time(nullptr);
		UserInfoPlus.dwInoutIndex = 0;

		//�û�Ȩ��
		UserInfoPlus.dwUserRight = 0;

		//��������
		UserInfoPlus.lLimitScore = 10;
		snprintf(UserInfoPlus.szPassword, sizeof(UserInfoPlus.szPassword), "%s", sha_pass_hash.c_str());

		//������Ϣ
		UserInfoPlus.wBindIndex = wBindIndex;
		UserInfoPlus.dwClientAddr = pBindParameter->dwClientAddr;
		snprintf(UserInfoPlus.szMachineID, sizeof(UserInfoPlus.szMachineID), "%s", strMachineID.c_str());

		//�����û�
		m_ServerUserManager.InsertUserItem(&pIServerUserItem, UserInfo, UserInfoPlus);

		//�����ж�
		if (pIServerUserItem == nullptr)
		{
			assert(nullptr);
			PerformUnlockScore(pLogonMobile->dwUserID, 0/*pDBOLogonSuccess->dwInoutIndex*/, LER_SERVER_FULL);
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return true;
		}

		//�����û�
		pBindParameter->pIServerUserItem = pIServerUserItem;

		//��¼�¼�
		OnEventUserLogon(pIServerUserItem, false);

		//�����û�
		//if (m_bCollectUser == true)
		{
			//��������
			CMD_CS_C_UserEnter UserEnter;
			memset(&UserEnter, 0, sizeof(UserEnter));

			//���ñ���
			UserEnter.dwUserID = pIServerUserItem->GetUserID();
			UserEnter.dwGameID = pIServerUserItem->GetGameID();
			snprintf(UserEnter.szNickName, sizeof(UserEnter.szNickName), "%s", pIServerUserItem->GetNickName());

			//�û���ϸ��Ϣ
			tagUserInfo* pUserInfo = pIServerUserItem->GetUserInfo();
			if (pUserInfo) memcpy(&UserEnter.userInfo, pUserInfo, sizeof(tagUserInfo));

			//������Ϣ
			assert(m_pITCPSocketService);
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ENTER, &UserEnter, sizeof(UserEnter));
		}
		return true;
	}

	bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		//Ч�����
		assert(wDataSize == sizeof(CMD_GR_UserSitDown));
		if (wDataSize != sizeof(CMD_GR_UserSitDown)) return false;

		//Ч������
		CMD_GR_UserSitDown * pUserSitDown = (CMD_GR_UserSitDown *)pData;

		//��ȡ�û�
		uint16 wBindIndex = LOWORD(dwSocketID);
		IRoomUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);

		//�û�Ч��
		assert(pIServerUserItem != nullptr);
		if (pIServerUserItem == nullptr) return false;

		//��Ϣ����
		uint16 wTableID = pIServerUserItem->GetTableID();
		uint16 wChairID = pIServerUserItem->GetChairID();
		uint8 cbUserStatus = pIServerUserItem->GetUserStatus();

		//�ظ��ж�
		if ((pUserSitDown->wTableID < m_pGameServiceOption->wTableCount) && (pUserSitDown->wChairID < m_pGameServiceOption->wChairCount))
		{
			CTableFrame * pTableFrame = m_TableFrameArray[pUserSitDown->wTableID];
			if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID) == pIServerUserItem) return true;
		}

		//�û��ж�
		if (cbUserStatus == US_PLAYING)
		{
			SendUserFailure(pIServerUserItem, LogonError[LEC_USER_PLAYING].c_str());
			return true;
		}

		//�뿪����
		if (wTableID != INVALID_TABLE)
		{
			CTableFrame * pTableFrame = m_TableFrameArray[wTableID];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem, true) == false) return true;
		}

		//�������
		uint16 wRequestTableID = pUserSitDown->wTableID;
		uint16 wRequestChairID = pUserSitDown->wChairID;
		uint16 wTailChairID = INVALID_CHAIR;

		//Ѱ��λ��
		for (uint16 i = 0; i < m_TableFrameArray.size(); ++i)
		{
			//��ȡ��λ
			uint16 wNullChairID = m_TableFrameArray[i]->GetNullChairID();

			//�������
			if (wNullChairID != INVALID_CHAIR)
			{
				//���ñ���
				wRequestTableID = i;
				wRequestChairID = wNullChairID;
				break;
			}
		}

		//����ж�
		if ((wRequestTableID == INVALID_CHAIR) || (wRequestChairID == INVALID_CHAIR))
		{
			SendUserFailure(pIServerUserItem, LogonError[LEC_USER_ROOM_FULL].c_str());
			return true;
		}
		

		//���ӵ���
		if (wRequestChairID >= m_pGameServiceOption->wChairCount)
		{
			//Ч�����
			assert(wRequestTableID < m_TableFrameArray.size());
			if (wRequestTableID >= m_TableFrameArray.size()) return false;

			//���ҿ�λ
			wRequestChairID = m_TableFrameArray[wRequestTableID]->GetNullChairID();

			//����ж�
			if (wRequestChairID == INVALID_CHAIR)
			{
				SendUserFailure(pIServerUserItem, LogonError[LEC_USER_TABLE_NOT_CHAIR].c_str());
				return true;
			}
		}

		//���´���
		std::string strPWD = Util::StringUtility::WStringToString(pUserSitDown->szPassword);
		CTableFrame * pTableFrame = m_TableFrameArray[wRequestTableID];
		pTableFrame->PerformSitDownAction(wRequestChairID, pIServerUserItem, strPWD.c_str());
		return true;
	}

	bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		//Ч�����
		CMD_GR_UserStandUp * pUserStandUp = (CMD_GR_UserStandUp *)pData;
		if (nullptr == pUserStandUp || wDataSize != sizeof(CMD_GR_UserStandUp))
		{
			assert(false);
			return false;
		}

		//��ȡ�û�
		uint16 wBindIndex = LOWORD(dwSocketID);
		IRoomUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);

		//�û�Ч��
		if (pIServerUserItem == NULL)
		{
			assert(false);
			return false;
		}
	
		//Ч������
		if (pUserStandUp->wChairID >= m_pGameServiceOption->wChairCount) return false;
		if (pUserStandUp->wTableID >= (uint16)m_TableFrameArray.size()) return false;

		//��Ϣ����
		uint16 wTableID = pIServerUserItem->GetTableID();
		uint16 wChairID = pIServerUserItem->GetChairID();
		if ((wTableID != pUserStandUp->wTableID) || (wChairID != pUserStandUp->wChairID)) return true;

		//�û��ж�
		if ((pUserStandUp->cbForceLeave == 0) && (pIServerUserItem->GetUserStatus() == US_PLAYING))
		{
			SendRoomMessage(pIServerUserItem, "��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��", SMT_CHAT | SMT_EJECT | SMT_GLOBAL);
			return true;
		}

		//�뿪����
		if (wTableID != INVALID_TABLE)
		{
			auto pTableFrame = m_TableFrameArray[wTableID];
			if (!pTableFrame->PerformStandUpAction(pIServerUserItem)) return true;
		}
		return true;
	}

	bool CAttemperEngineSink::SwitchUserItemConnect(IRoomUserItem * pIServerUserItem, const char szMachineID[LEN_MACHINE_ID], uint16 wTargetIndex)
	{
		//Ч�����
		assert((pIServerUserItem != nullptr) && (wTargetIndex != INVALID_WORD));
		if ((pIServerUserItem == nullptr) || (wTargetIndex == INVALID_WORD)) return false;

		//�Ͽ��û�
		if (pIServerUserItem->GetBindIndex() != INVALID_WORD)
		{
			//����֪ͨ
			SendRoomMessage(pIServerUserItem, "��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�������뿪��", SMT_CHAT | SMT_EJECT | SMT_GLOBAL | SMT_CLOSE_ROOM);

			//�󶨲���
			uint16 wSourceIndex = pIServerUserItem->GetBindIndex();
			tagBindParameter * pSourceParameter = GetBindParameter(wSourceIndex);

			//�����
			assert((pSourceParameter != nullptr) && (pSourceParameter->pIServerUserItem == pIServerUserItem));
			if ((pSourceParameter != nullptr) && (pSourceParameter->pIServerUserItem == pIServerUserItem)) pSourceParameter->pIServerUserItem = nullptr;

			//�Ͽ��û�
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}

		//�����ж�
		char* tszMachineID = pIServerUserItem->GetMachineID();
		bool bSameMachineID = strcmp(szMachineID, tszMachineID) == 0;

		//�����û�
		tagBindParameter * pTargetParameter = GetBindParameter(wTargetIndex);
		pTargetParameter->pIServerUserItem = pIServerUserItem;
		pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr, wTargetIndex, szMachineID, false);

		//״̬�л�
		bool bIsOffLine = false;
		if (pIServerUserItem->GetUserStatus() == US_OFFLINE)
		{
			//��������
			uint16 wTableID = pIServerUserItem->GetTableID();
			uint16 wChairID = pIServerUserItem->GetChairID();

			//����״̬
			bIsOffLine = true;
			pIServerUserItem->SetUserStatus(US_PLAYING, wTableID, wChairID);
		}

		//��¼�¼�
		OnEventUserLogon(pIServerUserItem, true);

		//��ȫ��ʾ
		if (!bIsOffLine && !bSameMachineID)
		{
			SendRoomMessage(pIServerUserItem, "��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�Է������뿪��", SMT_EJECT | SMT_CHAT | SMT_GLOBAL);
		}
		return true;
	}

	void CAttemperEngineSink::OnEventUserLogon(IRoomUserItem * pIServerUserItem, bool bOnLine)
	{
		//��ȡ����
		uint16 wBindIndex = pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

		//��������
		CMD_GR_LogonSuccess LogonSuccess;
		memset(&LogonSuccess, 0, sizeof(LogonSuccess));

		//��¼�ɹ�
		LogonSuccess.dwUserRight = 0;//pIServerUserItem->GetUserRight();
		LogonSuccess.dwMasterRight = 0;//pIServerUserItem->GetMasterRight();
		SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_SUCCESS, &LogonSuccess, sizeof(LogonSuccess));

		//��������
		CMD_GR_ConfigServer ConfigServer;
		memset(&ConfigServer, 0, sizeof(ConfigServer));

		//��������
		ConfigServer.wTableCount = m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount = m_pGameServiceOption->wChairCount;
		ConfigServer.wServerType = 0;// m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule = 0;// m_pGameServiceOption->dwServerRule;
		SendData(pBindParameter->dwSocketID, MDM_GR_CONFIG, SUB_GR_CONFIG_SERVER, &ConfigServer, sizeof(ConfigServer));

		//�������
		SendData(pBindParameter->dwSocketID, MDM_GR_CONFIG, SUB_GR_CONFIG_FINISH, nullptr, 0);

		//������Ϣ
		SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

		//��������	
		CRoomUserManager::CRoomUserItemMap ruim = m_ServerUserManager.TraverseRoomUserList();
		for (CRoomUserManager::RUIM_IT it = ruim.begin(); it != ruim.end(); ++it)
		{
			if (it->second == pIServerUserItem) continue;
			SendUserInfoPacket(it->second, pBindParameter->dwSocketID);
		}

		//������Ϣ
		SendUserInfoPacketBatch(pIServerUserItem, INVALID_DWORD);

		//��¼���
		SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_FINISH, nullptr, 0);

		//����״̬
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount = (uint16)m_TableFrameArray.size();
		assert(TableInfo.wTableCount < ARR_LEN(TableInfo.TableStatusArray));
		for (uint16 i = 0; i < TableInfo.wTableCount; i++)
		{
			CTableFrame * pTableFrame = m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock = 0;
			TableInfo.TableStatusArray[i].cbPlayStatus = 0;
			TableInfo.TableStatusArray[i].lCellScore = 1;
		}

		//����״̬
		uint16 wHeadSize = sizeof(TableInfo) - sizeof(TableInfo.TableStatusArray);
		uint16 wSendSize = wHeadSize + TableInfo.wTableCount * sizeof(TableInfo.TableStatusArray[0]);
		SendData(pBindParameter->dwSocketID, MDM_GR_STATUS, SUB_GR_TABLE_INFO, &TableInfo, wSendSize);

		//��������
		m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID, true);
		return;
	}

	void CAttemperEngineSink::OnEventUserLogout(IRoomUserItem * pIServerUserItem, uint32 dwLeaveReason)
	{
		//TODO...DB

		//�����û�
		{
			//��������
			CMD_CS_C_UserLeave UserLeave;
			memset(&UserLeave, 0, sizeof(UserLeave));

			//���ñ���
			UserLeave.dwUserID = pIServerUserItem->GetUserID();

			//������Ϣ
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_LEAVE, &UserLeave, sizeof(UserLeave));
		}

		//ɾ���û�
		m_ServerUserManager.DeleteUserItem(pIServerUserItem);
	}

	IRoomUserItem * CAttemperEngineSink::GetBindUserItem(uint16 wBindIndex)
	{
		//��ȡ����
		tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

		//��ȡ�û�
		if (pBindParameter != nullptr)
		{
			return pBindParameter->pIServerUserItem;
		}

		//�������
		assert(nullptr);
		return nullptr;
	}

	tagBindParameter * CAttemperEngineSink::GetBindParameter(uint16 wBindIndex)
	{
		//��Ч����
		if (wBindIndex == INVALID_WORD) return nullptr;

		//��������
		if (wBindIndex < m_pGameServiceOption->wMaxPlayer)
		{
			return m_pBindParameter + wBindIndex;
		}

		//�������
		assert(nullptr);
		return nullptr;
	}

	bool CAttemperEngineSink::SendLogonFailure(const char * pszString, uint32 lErrorCode, uint32 dwSocketID)
	{
		CMD_GR_LogonFailure LogonFailure;
		memset(&LogonFailure, 0, sizeof(LogonFailure));

		LogonFailure.lResultCode = lErrorCode;
		
		std::wstring wstrLogonError = Util::StringUtility::StringToWString(pszString);
		swprintf(LogonFailure.szDescribeString, sizeof(LogonFailure.szDescribeString), L"%s", wstrLogonError.c_str());

		return m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_FAILURE, &LogonFailure, sizeof(LogonFailure));
	}

	bool CAttemperEngineSink::SendUserFailure(IRoomUserItem * pIServerUserItem, const char * pszDescribe, uint32 lErrorCode)
	{
		//��������
		CMD_GR_UserRequestFailure UserRequestFailure;
		memset(&UserRequestFailure, 0, sizeof(UserRequestFailure));

		//���ñ���
		UserRequestFailure.lErrorCode = lErrorCode;
		
		std::wstring wstrDescribe = Util::StringUtility::StringToWString(pszDescribe);
		swprintf(UserRequestFailure.szDescribeString, sizeof(UserRequestFailure.szDescribeString), L"%s", wstrDescribe.c_str());

		//��������
		SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_REQUEST_FAILURE, &UserRequestFailure, sizeof(UserRequestFailure));
		return true;
	}

	bool CAttemperEngineSink::QuerryGameRoomRecordTime(IRoomUserItem * pIServerUserItem)
	{
		//���
		if (m_pGameServiceOption->wKindID == 104)
		{
			for (size_t i = 0; i < m_TableFrameArray.size(); ++i)
			{
				CTableFrame * pTableFrame = m_TableFrameArray[i];

				CMD_GF_RBRoomStatus RoomStatus;
				memset(&RoomStatus, 0, sizeof(RoomStatus));
				pTableFrame->OnGetGameRecord(&RoomStatus);
				SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_BJL_GAME_STATUS, &RoomStatus, sizeof(RoomStatus));
			}
			SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_BAIREN_STATUS_END, nullptr, 0);
		}
		return true;
	}

	bool CAttemperEngineSink::PerformUnlockScore(uint32 dwUserID, uint32 dwInoutIndex, uint32 dwLeaveReason)
	{
		return false;
	}
}