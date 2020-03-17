#include "AttemperEngineSink.h"
#include "CMD_LogonServer.h"
#include "Implementation/LogonDatabase.h"

#define MAX_LINK_COUNT 512

namespace Logon
{
	CAttemperEngineSink::CAttemperEngineSink()
	{
		
	}

	CAttemperEngineSink::~CAttemperEngineSink()
	{
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
		return true;
	}

	bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
	{
		return false;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkBind(uint64 dwClientAddr, uint64 dwSocketID)
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

	bool CAttemperEngineSink::OnEventTCPNetworkShut(uint64 dwClientAddr, uint64 dwSocketID)
	{
		memset((m_pBindParameter + dwSocketID), 0, sizeof(tagBindParameter));
		return true;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint64 dwSocketID)
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
				return true;
			}
		}
		return false;
	}
	bool CAttemperEngineSink::OnTCPNetworkMainMBLogon(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint64 dwSocketID)
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
	bool CAttemperEngineSink::OnTCPNetworkSubMBLogonVisitor(void * pData, uint16 wDataSize, uint64 dwSocketID)
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

		////////////////////////////////////
		PreparedStatement *stmt = LogonDatabasePool.GetPreparedStatement(LOGON_VISITOR_ACCOUNT);
		PreparedQueryResult result = LogonDatabasePool.Query(stmt);

		while (result)
		{
			Field* field = result->Fetch();
		}

		return true;
	}
}