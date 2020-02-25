#include "ServiceUnits.h"

namespace Logon
{
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

		SendControlPacket(SUC_LOAD_DB_GAME_LIST, NULL, 0);
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

		if (!m_TCPNetworkEngine->SetServiceParameter("192.168.1.217", 8600, 4))
		{
			return false;
		}

		return true;
	}
	bool ServiceUnits::StartKernelService(Net::IOContext* ioContext)
	{
		if (!m_AttemperEngine->Start(ioContext))
		{
			return false;
		}
		if (!m_TCPNetworkEngine->Start(ioContext))
		{
			return false;
		}

		LogonDatabasePool.Start(LogonDatabasePool, "Logon");

		//��ȡDB��DB�����������ᶨ��ܶ������ֱ���ûص�������
		return true;
	}
	bool ServiceUnits::SendControlPacket(uint16 wControlID, void * pData, uint16 wDataSize)
	{
		//״̬Ч��
		assert(m_AttemperEngine.GetDLLInterface() != nullptr);
		if (m_AttemperEngine.GetDLLInterface() == nullptr) return false;

		//���Ϳ���
		return m_AttemperEngine->OnEventControl(wControlID, pData, wDataSize);
	}
}