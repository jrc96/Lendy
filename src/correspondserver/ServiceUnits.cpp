#include "ServiceUnits.h"
#include "Log.h"
#include "INIReader.h"

namespace Correspond
{
	using namespace LogComm;
	using namespace Util;

	ServiceUnits * ServiceUnits::GetInstance()
	{
		static ServiceUnits* _instance = nullptr;
		if (_instance == nullptr)
		{
			_instance = new ServiceUnits();
		}
		return _instance;
	}

	ServiceUnits::ServiceUnits() :
		m_ServiceStatus(ServiceStatus_Stop)
	{
	}

	bool ServiceUnits::Start(Net::IOContext* ioContext)
	{
		assert(m_ServiceStatus == ServiceStatus_Stop);
		if (m_ServiceStatus != ServiceStatus_Stop) return false;

		m_ServiceStatus = ServiceStatus_Config;

		if (!InitializeService())
		{
			Conclude();
			return false;
		}

		//�����ں�
		if (!StartKernelService(ioContext))
		{
			Conclude();
			return false;
		}

		//����״̬
		SetServiceStatus(ServiceStatus_Run);
		return true;
	}
	bool ServiceUnits::Conclude()
	{
		m_ServiceStatus = ServiceStatus_Stop;

		if (m_AttemperEngine.GetDLLInterface())
		{
			m_AttemperEngine->Stop();
		}

		if (m_TCPNetworkEngine.GetDLLInterface())
		{
			m_TCPNetworkEngine->Stop();
		}
		return true;
	}

	bool ServiceUnits::InitializeService()
	{
		if ((m_AttemperEngine.GetDLLInterface() == nullptr) && (!m_AttemperEngine.CreateInstance()))
		{
			return false;
		}

		if ((m_TCPNetworkEngine.GetDLLInterface() == nullptr) && (!m_TCPNetworkEngine.CreateInstance()))
		{
			return false;
		}
		
		//����ӿ�
		IUnknownEx * pIAttemperEngine = m_AttemperEngine.GetDLLInterface();
		IUnknownEx * pITCPNetworkEngine = m_TCPNetworkEngine.GetDLLInterface();
		IUnknownEx * pIAttemperEngineSink = QUERY_OBJECT_INTERFACE(m_AttemperEngineSink, IUnknownEx);

		//�ں����
		if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine) == false) return false;
		if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine) == false) return false;
		if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

		m_AttemperEngineSink.m_pITCPNetworkEngine = m_TCPNetworkEngine.GetDLLInterface();

		if (!m_TCPNetworkEngine->SetServiceParameter(
			sConfigMgr->Get("LocalNet", "BindIP", "127.0.0.1"), 
			sConfigMgr->GetInt32("LocalNet", "Port", 8610),
			sConfigMgr->GetInt32("LocalNet", "Threads", 4)))
		{
			return false;
		}

		return true;
	}
	bool ServiceUnits::StartKernelService(Net::IOContext* ioContext)
	{
		if (!m_AttemperEngine->Start(ioContext))
		{
			assert(nullptr);
			return false;
		}

		//��������
		if (!m_TCPNetworkEngine->Start(ioContext))
		{
			assert(nullptr);
			return false;
		}

		return true;
	}

	bool ServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
	{
		if (m_ServiceStatus != ServiceStatus)
		{
			//����֪ͨ
			if ((m_ServiceStatus != ServiceStatus_Run) && (ServiceStatus == ServiceStatus_Stop))
			{
				LOG_INFO("server.correspond", "Service failed to start.");
			}

			//���ñ���
			m_ServiceStatus = ServiceStatus;

			switch (m_ServiceStatus)
			{
				case ServiceStatus_Stop:	//ֹͣ״̬
				{
					LOG_INFO("server.correspond", "Service stopped successfully");
					break;
				}
				case ServiceStatus_Config:	//����״̬
				{
					LOG_INFO("server.correspond", "Initializing component...");
					break;
				}
				case ServiceStatus_Run:	//����״̬
				{
					LOG_INFO("server.correspond", "Service started successfully");
					break;
				}
			}
		}
		return true;
	}
}