#ifndef SERVICE_UNITS_H
#define SERVICE_UNITS_H


#include "KernelEngineHead.h"
#include "DBExports.h"
#include "IOContext.h"
#include "AttemperEngineSink.h"
#include "Header.h"

namespace Logon
{
	class ServiceUnits
	{
		//����״̬
		enum enServiceStatus
		{
			ServiceStatus_Stop,				//ֹͣ״̬
			ServiceStatus_Config,			//����״̬
			ServiceStatus_Run,				//����״̬
		};

	public:
		ServiceUnits();

	public:
		static ServiceUnits* GetInstance();

	public:
		//��������
		bool Start(Net::IOContext* ioContext);

		//ֹͣ����
		bool Conclude();

		//�ڲ�����
	protected:
		//�������
		bool InitializeService();
		//�����ں�
		bool StartKernelService(Net::IOContext*);

		//�ڲ�����
	private:
		//���Ϳ���
		bool SendControlPacket(uint16 wControlID, void * pData, uint16 wDataSize);

	private:
		//״̬����
		enServiceStatus				m_ServiceStatus;

		//�������
	public:
		CAttemperEngineSink			m_AttemperEngineSink;				//���ȹ���

	private:
		CAttemperEngineHelper		m_AttemperEngine;					//��������
		CTCPNetworkEngineHelper		m_TCPNetworkEngine;
		CTCPSocketServiceHelper		m_TCPSocketService;					//����ͨѶ
	};
}

#define SrvUnitsMgr Logon::ServiceUnits::GetInstance()

#endif