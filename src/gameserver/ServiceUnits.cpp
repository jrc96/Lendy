#include "ServiceUnits.h"
#include "Log.h"
#include "INIReader.h"

namespace Game
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
		m_ServiceStatus(ServiceStatus_Stop),
		m_ioContext(1),
		m_pStrand(new Net::Strand(m_ioContext)),
		m_pThread(nullptr)
	{
	}

	bool ServiceUnits::Start(Net::IOContext* ioContext)
	{
		assert(m_ServiceStatus == ServiceStatus_Stop);
		if (m_ServiceStatus != ServiceStatus_Stop) return false;

		m_ServiceStatus = ServiceStatus_Config;

		if (m_pThread = new std::thread(&ServiceUnits::Run, this), m_pThread == nullptr)
		{
			assert(nullptr);
			return false;
		}

		UpdateConfig();

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

		//���ú���
		m_funcStartNetService = [&]()
		{
			if (!m_TCPNetworkEngine->Start(ioContext))
			{
				return false;
			}
			return true;
		};

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

		if (m_TCPSocketService.GetDLLInterface())
		{
			m_TCPSocketService->Stop();
		}
		return true;
	}

	void ServiceUnits::Run()
	{
		//�¼�֪ͨ
		asio::io_context::work work(m_ioContext);
		m_ioContext.run();
	}

	bool ServiceUnits::UpdateConfig()
	{
		//Ч��״̬
		////assert(m_ServiceStatus == ServiceStatus_Stop);
		//if (m_ServiceStatus != ServiceStatus_Stop) return false;

		//����ģ��
		///m_GameServiceOption = GameServiceOption;
		m_GameServiceManager.SetMoudleDLLCreate("RedBlack.dll", SUB_GAME_CREATE_NAME);
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

		if ((m_TCPSocketService.GetDLLInterface() == nullptr) && (!m_TCPSocketService.CreateInstance()))
		{
			return false;
		}

		//��Ϸģ��
		if ((m_GameServiceManager.GetDLLInterface() == nullptr) && (!m_GameServiceManager.CreateInstance()))
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

		if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND) == false) return false;
		if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine) == false) return false;

		m_AttemperEngineSink.m_pITCPNetworkEngine = m_TCPNetworkEngine.GetDLLInterface();
		m_AttemperEngineSink.m_pITCPSocketService = m_TCPSocketService.GetDLLInterface();
		m_AttemperEngineSink.m_pIGameServiceManager = m_GameServiceManager.GetDLLInterface();

		if (!m_TCPNetworkEngine->SetServiceParameter(
			sConfigMgr->Get("Net", "BindIP", "127.0.0.1"),
			sConfigMgr->GetInt32("Net", "Port", 8600),
			sConfigMgr->GetInt32("Net", "Threads", 4)))
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

		if (!m_TCPSocketService->Start(ioContext))
		{
			return false;
		}

		//��ȡDB��DB�����������ᶨ��ܶ������ֱ���ûص�������
		LogonDatabasePool.Start(LogonDatabasePool);

		return true;
	}

	bool ServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
	{
		if (m_ServiceStatus != ServiceStatus)
		{
			//����֪ͨ
			if ((m_ServiceStatus != ServiceStatus_Run) && (ServiceStatus == ServiceStatus_Stop))
			{
				LOG_INFO("server.logon", "��������ʧ��");
			}

			//���ñ���
			m_ServiceStatus = ServiceStatus;

			switch (m_ServiceStatus)
			{
				case ServiceStatus_Stop:	//ֹͣ״̬
				{
					LOG_INFO("server.logon", "����ֹͣ�ɹ�");
					break;
				}
				case ServiceStatus_Config:	//����״̬
				{
					LOG_INFO("server.logon", "���ڳ�ʼ�����...");
					break;
				}
				case ServiceStatus_Run:	//����״̬
				{
					LOG_INFO("server.logon", "���������ɹ�");
					break;
				}
			}
		}
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

	bool ServiceUnits::PostControlRequest(uint16 wIdentifier, void * pData, uint16 wDataSize)
	{
		{
			std::lock_guard<std::mutex>	 _lock(m_mutex);
			m_dataQueue.InsertData(wIdentifier, pData, wDataSize);
		}

		Net::post(m_ioContext, Net::bind_executor(*m_pStrand, [this, wIdentifier, pData, wDataSize]() {
			OnUIControlRequest();
		}));
		return true;
	}

	bool ServiceUnits::OnUIControlRequest()
	{
		tagDataHead DataHead;
		uint8 cbBuffer[SOCKET_TCP_BUFFER] = {};

		//��ȡ����
		std::lock_guard<std::mutex>	 _lock(m_mutex);
		if (m_dataQueue.DistillData(DataHead, cbBuffer, sizeof(cbBuffer)) == false)
		{
			assert(nullptr);
			return false;
		}

		//���ݴ���
		switch (DataHead.wIdentifier)
		{
			case UDC_LOAD_DB_LIST_RESULT:	//�б���
			{
				//Ч����Ϣ
				assert(DataHead.wDataSize == sizeof(ControlResult));
				if (DataHead.wDataSize != sizeof(ControlResult)) return 0;

				//��������
				ControlResult * pControlResult = (ControlResult *)cbBuffer;

				//ʧ�ܴ���
				if ((m_ServiceStatus != ServiceStatus_Run) && (pControlResult->cbSuccess == 0))
				{
					Conclude();
					return 0;
				}

				//�ɹ�����
				if ((m_ServiceStatus != ServiceStatus_Run) && (pControlResult->cbSuccess == 1))
				{
					//����Э��
					SendControlPacket(SUC_CONNECT_CORRESPOND, NULL, 0);
				}
				return 0;
			}
			case UDC_CORRESPOND_RESULT:
			{
				//Ч����Ϣ
				assert(DataHead.wDataSize == sizeof(ControlResult));
				if (DataHead.wDataSize != sizeof(ControlResult)) return 0;

				//��������
				ControlResult * pControlResult = (ControlResult *)cbBuffer;

				//ʧ�ܴ���
				if ((m_ServiceStatus != ServiceStatus_Run) && (pControlResult->cbSuccess == 0))
				{
					Conclude();
					return 0;
				}

				//�ɹ�����
				if ((m_ServiceStatus != ServiceStatus_Run) && (pControlResult->cbSuccess == 1))
				{
					//��������
					if (!m_funcStartNetService())
					{
						Conclude();
						return 0;
					}

					//����״̬
					SetServiceStatus(ServiceStatus_Run);
				}

				return 0;
			}
		}

		return true;
	}
}