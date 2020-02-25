#ifndef ATTEMPER_ENGINE_SINK_H
#define ATTEMPER_ENGINE_SINK_H

#include "KernelEngineHead.h"
#include "DBExports.h"
#include "Header.h"

namespace Logon
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

		//�ֻ��¼�
	protected:
		//��¼����
		bool OnTCPNetworkMainMBLogon(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint64 dwSocketID);

	protected:
		//�ο͵�¼
		bool OnTCPNetworkSubMBLogonVisitor(void * pData, uint16 wDataSize, uint64 dwSocketID);

	private:
		tagBindParameter *				m_pBindParameter;					//��������

		//����ӿ�
	protected:
		ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	};
}

#endif