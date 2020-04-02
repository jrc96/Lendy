#include "AttemperEngineSink.h"
#include "ServiceUnits.h"
#include "CMD_Correspond.h"
#include "Log.h"
#include "StringUtility.h"

#define MAX_LINK_COUNT 512
#define OPEN_SWITCH		1
#define CLIENT_SWITCH	0

namespace Correspond
{
	using namespace LogComm;

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
		m_pITCPNetworkEngine = nullptr;

		return false;
	}

	bool CAttemperEngineSink::OnEventTCPSocketLink(uint16 wServiceID, int iErrorCode)
	{
		return false;
	}

	bool CAttemperEngineSink::OnEventTCPSocketShut(uint16 wServiceID, uint8 cbShutReason)
	{
		return false;
	}

	bool CAttemperEngineSink::OnEventTCPSocketRead(uint16 wServiceID, TCP_Command Command, void * pData, uint16 wDataSize)
	{
		return true;
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
		return true;
	}

	bool CAttemperEngineSink::OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint64 dwSocketID)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//����ע��
		{
			return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
		}
		return false;
	}
	bool CAttemperEngineSink::OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize)
	{
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
	{
		switch (wSubCmdID)
		{
		case SUB_CS_C_REGISTER_PLAZA:	//ע��㳡
		{
			//Ч������
			assert(wDataSize == sizeof(CMD_CS_C_RegisterPlaza));
			if (wDataSize != sizeof(CMD_CS_C_RegisterPlaza)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterPlaza * pRegisterPlaza = (CMD_CS_C_RegisterPlaza *)pData;

			////��Ч�ж�
			//if ((pRegisterPlaza->szServerName[0] == 0) || (pRegisterPlaza->szServerAddr[0] == 0))
			//{
			//	//��������
			//	CMD_CS_S_RegisterFailure RegisterFailure;
			//	memset(&RegisterFailure, 0, sizeof(RegisterFailure));

			//	//���ñ���
			//	RegisterFailure.lErrorCode = 0L;
			//	sprintf_s(RegisterFailure.szDescribeString, "������ע��ʧ�ܣ��������ַ���롰�������������Ϸ���");

			//	//������Ϣ
			//	uint16 wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			//	uint16 wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			//	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize);

			//	//�ж�����
			//	//m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			//	return true;
			//}

			////���ð�
			//WORD wBindIndex = LOWORD(dwSocketID);
			//(m_pBindParameter + wBindIndex)->wServiceID = wBindIndex;
			//(m_pBindParameter + wBindIndex)->ServiceKind = ServiceKind_Plaza;

			////��������
			//tagGamePlaza GamePlaza;
			//ZeroMemory(&GamePlaza, sizeof(GamePlaza));

			////��������
			//GamePlaza.wPlazaID = wBindIndex;
			//lstrcpyn(GamePlaza.szServerName, pRegisterPlaza->szServerName, CountArray(GamePlaza.szServerName));
			//lstrcpyn(GamePlaza.szServerAddr, pRegisterPlaza->szServerAddr, CountArray(GamePlaza.szServerAddr));

			////ע�᷿��
			//m_GlobalInfoManager.ActivePlazaItem(wBindIndex, GamePlaza);

			////�����б�
			//SendServerListItem(dwSocketID);

			//SendMatchListItem(dwSocketID);

			////Ⱥ������
			//m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

			return true;
		}
		}
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
	{
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
	{
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
	{
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
	{
		return false;
	}

	bool CAttemperEngineSink::OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
	{
		return false;
	}
}