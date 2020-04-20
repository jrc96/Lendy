#include "AttemperEngineSink.h"
#include "Timer.h"
#include "ServiceUnits.h"
#include "CMD_LogonServer.h"
#include "CMD_Correspond.h"
#include "Implementation/LogonDatabase.h"
#include "Log.h"
#include "INIReader.h"
#include "StringUtility.h"


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
		QUERY_INTERFACE(IAttemperEngineSink, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(IAttemperEngineSink, uuid);
		return nullptr;
	}

	bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
	{
		m_pBindParameter = new tagBindParameter[MAX_LINK_COUNT];
		
		InitTableFrameArray();
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
			RegisterRoom.dwOnLineCount		= 0;// m_ServerUserManager.GetUserItemCount();
			RegisterRoom.dwFullCount		= m_pGameServiceOption->wMaxPlayer;
			RegisterRoom.wTableCount		= m_pGameServiceOption->wTableCount;
			RegisterRoom.dwServerRule		= 0;//vCustomRule

			sprintf_s(RegisterRoom.szServerName, "%s", m_pGameServiceOption->strGameName);
			sprintf_s(RegisterRoom.szServerAddr, "%s", m_pGameAddressOption->szIP);

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
		memset((m_pBindParameter + dwSocketID), 0, sizeof(tagBindParameter));
		return true;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		switch (Command.wMainCmdID)
		{
			case MDM_MB_LOGON:			//��¼����
			{
				return OnTCPNetworkMainMBLogon(Command.wSubCmdID, pData, wDataSize, dwSocketID);
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
		switch (dwTimerID)
		{
			case IDI_CONNECT_CORRESPOND:
			{
				std::string strCorrespondAddress = sConfigMgr->Get("CorrespondNet", "BindIP", "127.0.0.1");
				uint16 wCorrespondPort = sConfigMgr->GetInt32("CorrespondNet", "Port", 8600);
				m_pITCPSocketService->Connect(strCorrespondAddress, wCorrespondPort);
				LOG_INFO("server.game", "Connecting to the correspond server [ %s:%d ]", strCorrespondAddress, wCorrespondPort);
				return true;
			}
		}
		return false;
	}

	bool CAttemperEngineSink::InitTableFrameArray()
	{
		tagTableFrameParameter TableFrameParameter;
		memset(&TableFrameParameter, 0, sizeof(TableFrameParameter));

		//�������
		TableFrameParameter.pIGameServiceManager = m_pIGameServiceManager;
		TableFrameParameter.pITCPSocketService = m_pITCPSocketService;

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

	bool CAttemperEngineSink::OnTCPNetworkMainMBLogon(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		switch (wSubCmdID)
		{
			case SUB_MB_LOGON_VISITOR:      //�ο͵�¼
			{
				return OnTCPNetworkSubMBLogonVisitor(pData, wDataSize, dwSocketID);
			}
		}
		return false;
	}
	bool CAttemperEngineSink::OnTCPNetworkSubMBLogonVisitor(void * pData, uint16 wDataSize, uint32 dwSocketID)
	{
		//Ч�����
		assert(wDataSize >= sizeof(CMD_MB_LogonVisitor));
		if (wDataSize < sizeof(CMD_MB_LogonVisitor))return false;

		//��������
		tagBindParameter * pBindParameter = (m_pBindParameter + dwSocketID);

		//������Ϣ
		CMD_MB_LogonVisitor * pLogonVisitor = (CMD_MB_LogonVisitor *)pData;

		//��������
		pBindParameter->cbClientKind = LinkType::LT_MOBILE;

		LogonErrorCode eLogonErrorCode = LEC_NONE;
		BYTE * pClientAddr = (BYTE *)&pBindParameter->dwClientAddr;
		std::string strClientIP = StringFormat("%d.%d.%d.%d", pClientAddr[3], pClientAddr[2], pClientAddr[1], pClientAddr[0]);

		////////////////////////////////////
		PreparedStatement *stmt = LogonDatabasePool.GetPreparedStatement(LOGON_SEL_LIMIT_ADDRESS);
		stmt->SetString(0, strClientIP);
		stmt->SetString(1, pLogonVisitor->szMachineID);
		PreparedQueryResult result = LogonDatabasePool.Query(stmt);

		if (result)
		{
			Field* field = result->Fetch();
			while (field[3].GetInt8() == OPEN_SWITCH)
			{
				if (field[2].GetUInt32() < time(0))
				{
					//���½�ֹ��Ϣ
					stmt = LogonDatabasePool.GetPreparedStatement(LOGON_UPD_LIMIT_ADDRESS);
					stmt->SetInt8(0, 0);
					stmt->SetInt8(1, 0);
					stmt->SetInt8(2, 0);
					stmt->SetString(3, strClientIP);
					stmt->SetString(4, pLogonVisitor->szMachineID);
					LogonDatabasePool.DirectExecute(stmt);
					break;
				}

				if (field[0].GetInt8() == OPEN_SWITCH)
				{
					eLogonErrorCode = LEC_LIMIT_IP;
					break;
				}

				if (field[1].GetInt8() == OPEN_SWITCH)
				{
					eLogonErrorCode = LEC_LIMIT_MAC;
					break;
				}
				
				LOG_ERROR("server.game", "��ֹ��¼�߼����� IP: %s  MAC: %s", strClientIP.c_str(), pLogonVisitor->szMachineID);
				break;
			}
		}

		//�Ƿ��ֹ��½
		LOGON_FAILURE(dwSocketID, eLogonErrorCode)

		//��ѯ�û���Ϣ
		stmt = LogonDatabasePool.GetPreparedStatement(LOGON_SEL_VISITOR_ACCOUNT);
		stmt->SetString(0, pLogonVisitor->szMachineID);
		result = LogonDatabasePool.Query(stmt);
		if (!result)
		{
			int game_id = 0;
			stmt = LogonDatabasePool.GetPreparedStatement(LOGON_SEL_GAME_ID);
			PreparedQueryResult result_id = LogonDatabasePool.Query(stmt);
			if (result_id)
			{
				Field* field = result_id->Fetch();
				game_id = field[0].GetInt32();

				//���±�ʶ
				stmt = LogonDatabasePool.GetPreparedStatement(LOGON_UPD_GAME_ID);
				LogonDatabasePool.DirectExecute(stmt);
			}
			else 
			{
				LOG_ERROR("server.game", "�����ο�ID���� IP: %s  MAC: %s", strClientIP.c_str(), pLogonVisitor->szMachineID);
			}


			//�����ο��û�
			std::string strVisitor = StringFormat("�ο�%d", game_id);
			stmt = LogonDatabasePool.GetPreparedStatement(LOGON_INS_VISITOR_ACCOUNT);
			
			std::string strUTF8Visitor;
			Util::StringUtility::ConsoleToUtf8(strVisitor, strUTF8Visitor);
			stmt->SetString(0, strUTF8Visitor);
			stmt->SetString(1, strUTF8Visitor);
			stmt->SetString(2, "");
			stmt->SetString(3, "1");
			stmt->SetInt8(4, pBindParameter->cbClientKind);
			stmt->SetString(5, strClientIP);
			stmt->SetString(6, pLogonVisitor->szMachineID);
			LogonDatabasePool.DirectExecute(stmt);

			//���²�ѯ�ο�
			stmt = LogonDatabasePool.GetPreparedStatement(LOGON_SEL_VISITOR_ACCOUNT);
			stmt->SetString(0, pLogonVisitor->szMachineID);
			result = LogonDatabasePool.Query(stmt);
			if (!result) 
			{
				LOG_ERROR("server.game", "�����ο�ID���� IP: %s  MAC: %s", strClientIP.c_str(), pLogonVisitor->szMachineID);
				return false;
			}
		}

		//��ȡ��Ϸ��Ϣ
		Field* field = result->Fetch();
		int id = field[0].GetInt32();
		std::string account = field[1].GetString();
		std::string username = field[2].GetString();
		std::string strAnsiVisitor;
		Util::StringUtility::Utf8ToConsole(account, strAnsiVisitor);
		std::string sha_pass_hash = field[3].GetString();
		std::string face_url = field[4].GetString();
		int limit = field[5].GetInt8();

		//�˺Ŷ���״̬
		if ((limit & LEC_LIMIT_FREEZE) > 0)
		{
			eLogonErrorCode = LEC_LIMIT_FREEZE;
		}
		LOGON_FAILURE(dwSocketID, eLogonErrorCode)

		//���µ�½��Ϣ
		stmt = LogonDatabasePool.GetPreparedStatement(LOGON_UPD_VISITOR_ACCOUNT);
		stmt->SetString(0, strClientIP);
		stmt->SetString(1, pLogonVisitor->szMachineID);
		LogonDatabasePool.DirectExecute(stmt);

		return true;
	}
	
	bool CAttemperEngineSink::OnLogonFailure(uint32 dwSocketID, LogonErrorCode & lec)
	{
		if (lec == LEC_NONE)
		{
			return false;
		}

		CMD_MB_LogonFailure LogonFailure;
		memset(&LogonFailure, 0, sizeof(LogonFailure));

		LogonFailure.lResultCode = lec;
		std::wstring wstrLogonError = Util::StringUtility::StringToWString(LogonError[lec]);
		swprintf_s(LogonFailure.szDescribe, L"%s", wstrLogonError.c_str());

		return m_pITCPNetworkEngine->SendData(dwSocketID, MDM_MB_LOGON, SUB_MB_LOGON_FAILURE, &LogonFailure, sizeof(LogonFailure));
	}
}