#include "AttemperEngine.h"

namespace Net
{
	CAttemperEngine::CAttemperEngine():
		m_pITCPNetworkEngine(nullptr),
		m_pIAttemperEngineSink(nullptr)
	{
		memset(m_cbBuffer, 0, sizeof(m_cbBuffer));
	}

	CAttemperEngine::~CAttemperEngine()
	{
	}

	bool CAttemperEngine::Start(Net::IOContext* ioContext)
	{
		//Ч�����
		assert(m_pIAttemperEngineSink != nullptr);
		if (m_pIAttemperEngineSink == nullptr) return false;

		//ע�����
		IUnknownEx * pIAsynchronismEngineSink = QUERY_ME_INTERFACE(IUnknownEx);
		if (m_AsynchronismEngine.SetAsynchronismSink(pIAsynchronismEngineSink) == false)
		{
			assert(nullptr);
			return false;
		}

		//�첽����
		if (!m_AsynchronismEngine.Start(ioContext))
		{
			assert(nullptr);
			return false;
		}
		////����֪ͨ
		//if (!m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx)))
		//{
		//	assert(nullptr);
		//	return false;
		//}
		return true;
	}

	bool CAttemperEngine::Stop()
	{
		m_AsynchronismEngine.Stop();

		////Ч�����
		//assert(m_pIAttemperEngineSink != nullptr);
		//if (m_pIAttemperEngineSink == nullptr) return false;

		//����֪ͨ
		//if (!m_pIAttemperEngineSink->OnAttemperEngineConclude(QUERY_ME_INTERFACE(IUnknownEx)))
		//{
		//	assert(nullptr);
		//	return false;
		//}
		return true;
	}

	void CAttemperEngine::Release()
	{
		delete this;
	}

	void * CAttemperEngine::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(IAttemperEngine, uuid);
		QUERY_INTERFACE(ITCPNetworkEngineEvent, uuid);
		QUERY_INTERFACE(IAsynchronismEngineSink, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(IAttemperEngine, uuid);
		return nullptr;
	}

	bool CAttemperEngine::SetNetworkEngine(IUnknownEx * pIUnknownEx)
	{
		//���ýӿ�
		if (pIUnknownEx != nullptr)
		{
			//��ѯ�ӿ�
			assert(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPNetworkEngine) != nullptr);
			m_pITCPNetworkEngine = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPNetworkEngine);

			//�ɹ��ж�
			if (m_pITCPNetworkEngine == nullptr) return false;
		}
		else m_pITCPNetworkEngine = nullptr;

		return true;
	}

	bool CAttemperEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
	{
		//��ѯ�ӿ�
		assert(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAttemperEngineSink) != NULL);
		m_pIAttemperEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAttemperEngineSink);

		//����ж�
		if (m_pIAttemperEngineSink == nullptr)
		{
			assert(FALSE);
			return false;
		}
		return true;
	}

	bool CAttemperEngine::OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize)
	{
		//if (m_ioContext)
		//{
		//	Net::post(*m_ioContext, Net::bind_executor(*m_pStrand, [this, wControlID, pData, wDataSize]() {m_pIAttemperEngineSink->OnEventControl(wControlID, pData, wDataSize); }));
		//}
		return true;
	}

	bool CAttemperEngine::OnEventTCPNetworkBind(uint64 dwSocketID, uint64 dwClientAddr)
	{
		//��������
		std::lock_guard<std::mutex> _lock(m_mutex);
		AS_TCPNetworkAcceptEvent * pAcceptEvent = (AS_TCPNetworkAcceptEvent *)m_cbBuffer;

		//��������
		pAcceptEvent->dwSocketID = dwSocketID;
		pAcceptEvent->dwClientAddr = dwClientAddr;

		//Ͷ������
		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_CLIENT_ACCEPT, m_cbBuffer, sizeof(AS_TCPNetworkAcceptEvent));
	}

	bool CAttemperEngine::OnEventTCPNetworkShut(uint64 dwSocketID, uint64 dwClientAddr)
	{
		//��������
		std::lock_guard<std::mutex> _lock(m_mutex);
		AS_TCPNetworkShutEvent * pCloseEvent = (AS_TCPNetworkShutEvent *)m_cbBuffer;

		//��������
		pCloseEvent->dwSocketID = dwSocketID;
		pCloseEvent->dwClientAddr = dwClientAddr;

		//Ͷ������
		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_CLIENT_SHUT, m_cbBuffer, sizeof(AS_TCPNetworkShutEvent));
	}
	bool CAttemperEngine::OnEventTCPNetworkRead(uint64 dwSocketID, Net::TCP_Command Command, void * pData, uint16 wDataSize)
	{
		assert((wDataSize + sizeof(AS_TCPNetworkReadEvent)) <= SOCKET_TCP_BUFFER);
		if ((wDataSize + sizeof(AS_TCPNetworkReadEvent)) > SOCKET_TCP_BUFFER) return false;

		std::lock_guard<std::mutex> _lock(m_mutex);
		AS_TCPNetworkReadEvent* pReadEvent = (AS_TCPNetworkReadEvent *)m_cbBuffer;

		//��������
		pReadEvent->Command = Command;
		pReadEvent->wDataSize = wDataSize;
		pReadEvent->dwSocketID = dwSocketID;

		//��������
		if (wDataSize > 0)
		{
			assert(pData != NULL);
			memcpy(m_cbBuffer + sizeof(AS_TCPNetworkReadEvent), pData, wDataSize);
		}

		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_CLIENT_READ, m_cbBuffer, sizeof(AS_TCPNetworkReadEvent) + wDataSize); 
	}

	bool CAttemperEngine::OnAsynchronismEngineStart()
	{
		//Ч�����
		assert(m_pIAttemperEngineSink != nullptr);
		if (m_pIAttemperEngineSink == nullptr) return false;

		//����֪ͨ
		if (!m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx)))
		{
			assert(nullptr);
			return false;
		}
		return true;
	}

	bool CAttemperEngine::OnAsynchronismEngineConclude()
	{
		//Ч�����
		assert(m_pIAttemperEngineSink != nullptr);
		if (m_pIAttemperEngineSink == nullptr) return false;

		//ֹ֪ͣͨ
		if (!m_pIAttemperEngineSink->OnAttemperEngineConclude(QUERY_ME_INTERFACE(IUnknownEx)))
		{
			assert(nullptr);
			return false;
		}
		return true;
	}

	bool CAttemperEngine::OnAsynchronismEngineData(uint16 wIdentifier, void * pData, uint16 wDataSize)
	{
		//Ч�����
		assert(m_pITCPNetworkEngine != NULL);
		assert(m_pIAttemperEngineSink != NULL);

		//�ں��¼�
		switch (wIdentifier)
		{
			case EVENT_TCP_CLIENT_ACCEPT:
			{
				//��С����
				assert(wDataSize == sizeof(AS_TCPNetworkAcceptEvent));
				if (wDataSize != sizeof(AS_TCPNetworkAcceptEvent)) return false;

				//��������
				bool bSuccess = false;
				AS_TCPNetworkAcceptEvent * pAcceptEvent = (AS_TCPNetworkAcceptEvent *)pData;

				//������Ϣ
				try
				{
					bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkBind(pAcceptEvent->dwClientAddr, pAcceptEvent->dwSocketID);
				}
				catch (...) {}

				//ʧ�ܴ���
				if (bSuccess == false) m_pITCPNetworkEngine->CloseSocket(pAcceptEvent->dwSocketID);

				return true;
			}
			case EVENT_TCP_CLIENT_READ:		//��ȡ�¼�
			{
				//Ч���С
				AS_TCPNetworkReadEvent * pReadEvent = (AS_TCPNetworkReadEvent *)pData;

				//��С����
				assert(wDataSize >= sizeof(AS_TCPNetworkReadEvent));
				assert(wDataSize == (sizeof(AS_TCPNetworkReadEvent) + pReadEvent->wDataSize));

				//��СЧ��
				if (wDataSize < sizeof(AS_TCPNetworkReadEvent))
				{
					m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
					return false;
				}

				//��СЧ��
				if (wDataSize != (sizeof(AS_TCPNetworkReadEvent) + pReadEvent->wDataSize))
				{
					m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
					return false;
				}

				//������Ϣ
				bool bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkRead(pReadEvent->Command, pReadEvent + 1, pReadEvent->wDataSize, pReadEvent->dwSocketID);
				
				//ʧ�ܴ���
				if (!bSuccess) m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				
				return true;
			}
		}

		return false;
	}

	DECLARE_CREATE_MODULE(AttemperEngine);
}
	 


