#ifndef ATTEMPER_ENGINE_SINK_H
#define ATTEMPER_ENGINE_SINK_H

#include "DBExports.h"
#include "Header.h"
#include "TableFrame.h"
#include "GameComponent.h"

namespace Game
{
	//�󶨲���
	struct tagBindParameter
	{
		//�������
		uint64							dwSocketID;							//�����ʶ
		uint64							dwClientAddr;						//���ӵ�ַ
		uint8							cbClientKind;						//��������
	};

	enum LinkType
	{
		LT_FLASH	= 1,				//��ҳ����
		LT_MOBILE	= 2,				//�ֻ�����
		LT_COMPUTER = 3,				//��������
	};

	typedef std::vector<CTableFrame *> CTableFrameArray;

	class CAttemperEngineSink : public IAttemperEngineSink
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
		virtual bool OnEventTCPNetworkBind(uint64 dwClientAddr, uint64 dwSocketID);
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint64 dwClientAddr, uint64 dwSocketID);
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint64 dwSocketID);

		//�ӿ��¼�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize);
		
		//�ں��¼�
	public:
		virtual bool OnEventTimer(uint32 dwTimerID);

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

		//�ֻ��¼�
	protected:
		//��¼����
		bool OnTCPNetworkMainMBLogon(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint64 dwSocketID);

	protected:
		//�ο͵�¼
		bool OnTCPNetworkSubMBLogonVisitor(void * pData, uint16 wDataSize, uint64 dwSocketID);

	protected:
		//��½ʧ��
		bool OnLogonFailure(uint64 dwSocketID, LogonErrorCode &lec);

	private:
		tagBindParameter *				m_pBindParameter;					//��������
		tagGameServiceOption *			m_pGameServiceOption;				//��������

		//����ӿ�
	protected:
		ITimerEngine *					m_pITimerEngine;					//ʱ������
		ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
		ITCPSocketService *				m_pITCPSocketService;
		IGameServiceManager *			m_pIGameServiceManager;				//�������

		//�������
	protected:
		CTableFrameArray				m_TableFrameArray;					//��������
	};
}

#endif