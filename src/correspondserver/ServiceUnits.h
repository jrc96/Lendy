#ifndef SERVICE_UNITS_H
#define SERVICE_UNITS_H


#include "KernelEngineHead.h"
#include "IOContext.h"
#include "Header.h"
#include "AttemperEngineSink.h"

namespace Correspond
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
		//����״̬
		bool SetServiceStatus(enServiceStatus ServiceStatus);

	private:
		//״̬����
		enServiceStatus						m_ServiceStatus;

		//�������
	public:
		CAttemperEngineSink					m_AttemperEngineSink;				//���ȹ���

	private:
		CAttemperEngineHelper				m_AttemperEngine;					//��������
		CTCPNetworkEngineHelper				m_TCPNetworkEngine;
	};
}

#define SrvUnitsMgr Correspond::ServiceUnits::GetInstance()

#endif