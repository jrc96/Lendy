#ifndef ATTEMPER_ENGINE_SINK_H
#define ATTEMPER_ENGINE_SINK_H

#include "DBExports.h"
#include "Header.h"
#include "TableFrame.h"
#include "GameComponent.h"
#include "RoomUserManager.h"

namespace Game
{
	//�󶨲���
	struct tagBindParameter
	{
		//�������
		uint32							dwSocketID;							//�����ʶ
		uint32							dwClientAddr;						//���ӵ�ַ
		uint8							cbClientKind;						//��������
		IRoomUserItem *				pIServerUserItem;
	};

	enum LinkType
	{
		LT_FLASH	= 1,				//��ҳ����
		LT_MOBILE	= 2,				//�ֻ�����
		LT_COMPUTER = 3,				//��������
	};

	typedef std::vector<CTableFrame *> CTableFrameArray;

	class CAttemperEngineSink : public IAttemperEngineSink , public IMainServiceFrame, public IRoomUserItemSink
	{
		friend class ServiceUnits;

	public:
		CAttemperEngineSink();
		virtual ~CAttemperEngineSink();

		virtual void Release();
		virtual void *QueryInterface(GGUID uuid);

		//�첽�ӿ�
	public:
		//�����¼�
		virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
		//ֹͣ�¼�
		virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

		//�����¼�
	public:
		//�����¼�
		virtual bool OnEventTCPSocketLink(uint16 wServiceID, int iErrorCode);
		//�ر��¼�
		virtual bool OnEventTCPSocketShut(uint16 wServiceID, uint8 cbShutReason);
		//��ȡ�¼�
		virtual bool OnEventTCPSocketRead(uint16 wServiceID, TCP_Command Command, void * pData, uint16 wDataSize);

		//�����¼�
	public:
		//Ӧ���¼�
		virtual bool OnEventTCPNetworkBind(uint32 dwClientAddr, uint32 dwSocketID);
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint32 dwClientAddr, uint32 dwSocketID);
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint32 dwSocketID);

		//�ӿ��¼�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize);
		
		//�ں��¼�
	public:
		virtual bool OnEventTimer(uint32 dwTimerID);

		//��Ϣ�ӿ�
	public:
		//������Ϣ
		virtual bool SendRoomMessage(char* lpszMessage, uint16 wType);
		//��Ϸ��Ϣ
		virtual bool SendGameMessage(char* lpszMessage, uint16 wType);
		//������Ϣ
		virtual bool SendRoomMessage(IRoomUserItem * pIServerUserItem, const char* lpszMessage, uint16 wType);
		//��Ϸ��Ϣ
		virtual bool SendGameMessage(IRoomUserItem * pIServerUserItem, const char* lpszMessage, uint16 wType);
		//������Ϣ
		virtual bool SendRoomMessage(uint32 dwSocketID, const char* lpszMessage, uint16 wType, bool bAndroid);

		//��������
		virtual bool SendData(uint32 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize);
		//��������
		virtual bool SendData(IRoomUserItem * pIServerUserItem, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize);
		//Ⱥ������
		virtual bool SendDataBatch(uint16 wCmdTable, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize);
		//��������
		virtual void UnLockScoreLockUser(uint32 dwUserID, uint32 dwInoutIndex, uint32 dwLeaveReason);

		//�û��ӿ�
	public:
		//�û�״̬
		virtual bool OnEventUserItemStatus(IRoomUserItem * pIServerUserItem, uint16 wOldTableID = INVALID_TABLE, uint16 wOldChairID = INVALID_CHAIR);

		//���ͺ���
	protected:
		//�û���Ϣ
		bool SendUserInfoPacket(IRoomUserItem * pIServerUserItem, uint32 dwSocketID);
		//Ⱥ���û���Ϣ
		bool SendUserInfoPacketBatch(IRoomUserItem * pIServerUserItem, uint32 dwSocketID);

		//��������
	protected:
		//��������
		bool InitTableFrameArray();

		//���Ӵ���
	protected:
		//ע���¼�
		bool OnTCPSocketMainRegister(uint16 wSubCmdID, void * pData, uint16 wDataSize);
		//�б��¼�
		bool OnTCPSocketMainServiceInfo(uint16 wSubCmdID, void * pData, uint16 wDataSize);
		
		//�����¼�
	protected:
		//�û�����
		bool OnTCPNetworkMainUser(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//��¼����
		bool OnTCPNetworkMainLogon(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//��ܴ���
		bool OnTCPNetworkMainFrame(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);

	protected:
		//�ο͵�¼
		bool OnTCPNetworkSubMBLogonVisitor(void * pData, uint16 wDataSize, uint32 dwSocketID);

		//�û�����
	protected:
		//�û�����
		bool OnTCPNetworkSubUserSitDown(void * pData, uint16 wDataSize, uint32 dwSocketID);

		//ִ�й���
	protected:
		//�л�����
		bool SwitchUserItemConnect(IRoomUserItem * pIServerUserItem, const char szMachineID[LEN_MACHINE_ID], uint16 wTargetIndex);

		//�ڲ��¼�
	protected:
		//�û���¼
		void OnEventUserLogon(IRoomUserItem * pIServerUserItem, bool bOnLine);
		//�û��ǳ�
		void OnEventUserLogout(IRoomUserItem * pIServerUserItem, uint32 dwLeaveReason);

		//��������
	public:
		//���û�
		IRoomUserItem  * GetBindUserItem(uint16 wBindIndex);
		//�󶨲���
		tagBindParameter * GetBindParameter(uint16 wBindIndex);

		//��������
	protected:
		//��¼ʧ��
		bool SendLogonFailure(const char* pszString, uint32 lErrorCode, uint32 dwSocketID);
		//�û�ʧ��
		bool SendUserFailure(IRoomUserItem * pIServerUserItem, const char* pszDescribe, uint32 lErrorCode = 0);

		//��������
protected:
		//·��ʱ��
		bool QuerryGameRoomRecordTime(IRoomUserItem * pIServerUserItem);

		//ִ�й���
protected:
		//������Ϸ
		bool PerformUnlockScore(uint32 dwUserID, uint32 dwInoutIndex, uint32 dwLeaveReason);

		//״̬����
	protected:
		bool							m_bNeekCorrespond;					//Э����־

	private:
		tagBindParameter *				m_pBindParameter;					//��������
		tagGameAddressOption *			m_pGameAddressOption;				//�����ַ
		tagGameServiceOption *			m_pGameServiceOption;				//��������

		//����ӿ�
	protected:
		ITimerEngine *					m_pITimerEngine;					//ʱ������
		ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
		ITCPSocketService *				m_pITCPSocketService;
		IGameServiceManager *			m_pIGameServiceManager;				//�������

		//�������
	protected:
		CRoomUserManager				m_ServerUserManager;				//�û�����
		CTableFrameArray				m_TableFrameArray;					//��������
	};
}

#endif