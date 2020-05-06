#include "TCPNetworkEngine.h"
#include <map>

namespace Net
{
#define ASYNCHRONISM_SEND_DATA		1									//���ͱ�ʶ
#define ASYNCHRONISM_SEND_BATCH		2									//Ⱥ�巢��
#define ASYNCHRONISM_SHUT_DOWN		3									//��ȫ�ر�
#define ASYNCHRONISM_ALLOW_BATCH	4									//����Ⱥ��
#define ASYNCHRONISM_CLOSE_SOCKET	5									//�ر�����

	//�ر�����
	struct tagCloseSocket
	{
		uint32							dwSocketID;							//��������
	};

	//��ȫ�ر�
	struct tagShutDownSocket
	{
		uint32							dwSocketID;							//��������
	};

	//����Ⱥ��
	struct tagAllowBatchSend
	{
		uint32							dwSocketID;							//��������
		uint8							cbAllowBatch;						//�����־
	};

	//Ⱥ������
	struct tagBatchSendRequest
	{
		uint16							wMainCmdID;							//��������
		uint16							wSubCmdID;							//��������
		uint16							wDataSize;							//���ݴ�С
		uint8							cbSendBuffer[SOCKET_TCP_PACKET];	//���ͻ���
	};

	//��������
	struct tagSendData
	{
		uint32							dwSocketID;							//��������
		uint16							wMainCmdID;							//��������
		uint16							wSubCmdID;							//��������
		uint16							wDataSize;							//���ݴ�С
		uint8							cbSendBuffer[SOCKET_TCP_PACKET];	//���ͻ���
	};

	CTCPNetworkEngine::CTCPNetworkEngine() :
		m_curIndex(0),
		m_strBindIP(""),
		m_port(0),
		m_acceptor(nullptr),
		m_threadCount(0),
		m_pThreads(nullptr),
		m_pITCPNetworkEngineEvent(nullptr)
	{}

	CTCPNetworkEngine::~CTCPNetworkEngine()
	{
	}

	void CTCPNetworkEngine::Release()
	{
		delete this;
	}

	void * CTCPNetworkEngine::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(IServiceModule, uuid);
		QUERY_INTERFACE(ITCPNetworkEngine, uuid);
		QUERY_INTERFACE(IAsynchronismEngineSink, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(ITCPNetworkEngine, uuid);
		return nullptr;
	}

	bool CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
	{
		assert(m_threadCount == 0);
		if (m_threadCount > 0) return false;

		//��ѯ�ӿ�
		m_pITCPNetworkEngineEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPNetworkEngineEvent);

		//�����ж�
		if (m_pITCPNetworkEngineEvent == nullptr)
		{
			assert(FALSE);
			return false;
		}
		return true;
	}

	bool CTCPNetworkEngine::SetServiceParameter(std::string strBindIP, uint16 port, uint16 threadCount)
	{
		assert(threadCount > 0);
		if (threadCount == 0) return false;

		m_strBindIP = std::move(strBindIP);
		m_port = port;
		m_threadCount = threadCount;
		return true;
	}

	bool CTCPNetworkEngine::OnAsynchronismEngineData(uint16 wIdentifier, void * pData, uint16 wDataSize)
	{
		switch (wIdentifier)
		{
			case ASYNCHRONISM_SEND_DATA:		//��������
			{
				//Ч������
				tagSendData * pSendDataRequest = (tagSendData *)pData;
				assert(wDataSize >= (sizeof(tagSendData) - sizeof(pSendDataRequest->cbSendBuffer)));
				assert(wDataSize == (pSendDataRequest->wDataSize + sizeof(tagSendData) - sizeof(pSendDataRequest->cbSendBuffer)));

				//��ȡ����
				CTCPNetworkItem* pTCPNetworkItem = GetNetworkItem(pSendDataRequest->dwSocketID);
				if (pTCPNetworkItem == NULL) return false;

				//��������
				std::lock_guard<std::mutex> _lock(pTCPNetworkItem->GetMutex());
				pTCPNetworkItem->SendData(pSendDataRequest->wMainCmdID, pSendDataRequest->wSubCmdID, pSendDataRequest->cbSendBuffer, pSendDataRequest->wDataSize);
				return true;
			}
			case ASYNCHRONISM_SEND_BATCH:
			{
				//Ч������
				tagBatchSendRequest * pBatchSendRequest = (tagBatchSendRequest *)pData;
				assert(wDataSize >= (sizeof(tagBatchSendRequest) - sizeof(pBatchSendRequest->cbSendBuffer)));
				assert(wDataSize == (pBatchSendRequest->wDataSize + sizeof(tagBatchSendRequest) - sizeof(pBatchSendRequest->cbSendBuffer)));

				//Ⱥ������
				CTCPNetworkItem * pTCPNetworkItem = nullptr;
				for (std::map<uint32, CTCPNetworkItem*>::iterator it = m_BatchNetItemStore.begin(); it != m_BatchNetItemStore.end(); ++it)
				{
					//��ȡ����
					pTCPNetworkItem = it->second;
				
					//��������
					std::lock_guard<std::mutex> _lock(pTCPNetworkItem->GetMutex());
					pTCPNetworkItem->SendData(pBatchSendRequest->wMainCmdID, pBatchSendRequest->wSubCmdID, pBatchSendRequest->cbSendBuffer, pBatchSendRequest->wDataSize);
				}
				return true;
			}
			case ASYNCHRONISM_CLOSE_SOCKET:
			{
				//Ч������
				assert(wDataSize == sizeof(tagCloseSocket));
				tagCloseSocket * pCloseSocket = (tagCloseSocket *)pData;

				//��ȡ����
				CTCPNetworkItem * pTCPNetworkItem = GetNetworkItem(pCloseSocket->dwSocketID);
				if (pTCPNetworkItem == nullptr) return false;

				//�ر�����
				std::lock_guard<std::mutex> _lock(pTCPNetworkItem->GetMutex());
				pTCPNetworkItem->CloseSocket();
			}
			case ASYNCHRONISM_SHUT_DOWN:
			{
				//Ч������
				assert(wDataSize == sizeof(tagShutDownSocket));
				tagShutDownSocket * pShutDownSocket = (tagShutDownSocket *)pData;

				//��ȡ����
				CTCPNetworkItem * pTCPNetworkItem = GetNetworkItem(pShutDownSocket->dwSocketID);
				if (pTCPNetworkItem == NULL) return false;

				//��ȫ�ر�
				std::lock_guard<std::mutex> _lock(pTCPNetworkItem->GetMutex());
				pTCPNetworkItem->DelayedCloseSocket();

				return true;
			}
			case ASYNCHRONISM_ALLOW_BATCH:
			{
				//Ч������
				assert(wDataSize == sizeof(tagAllowBatchSend));
				tagAllowBatchSend * pAllowBatchSend = (tagAllowBatchSend *)pData;

				//��ȡ����
				CTCPNetworkItem * pTCPNetworkItem = GetNetworkItem(pAllowBatchSend->dwSocketID);
				if (pTCPNetworkItem == nullptr) return false;

				//����Ⱥ��
				std::lock_guard<std::mutex> _lock(pTCPNetworkItem->GetMutex());
				pTCPNetworkItem->AllowBatchSend(pAllowBatchSend->cbAllowBatch);
				m_BatchNetItemStore[pTCPNetworkItem->GetIndex()] = pTCPNetworkItem;
			}
		}
		return false;
	}

	bool CTCPNetworkEngine::OnEventSocketBind(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem)
	{
		//Ч������
		assert(pTCPNetworkItem != nullptr);
		assert(m_pITCPNetworkEngineEvent != nullptr);

		m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(pTCPNetworkItem->GetIndex(), pTCPNetworkItem->GetClientIP());
		return true;
	}

	bool CTCPNetworkEngine::OnEventSocketShut(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem)
	{
		//Ч������
		assert(pTCPNetworkItem != nullptr);
		assert(m_pITCPNetworkEngineEvent != nullptr);

		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(pTCPNetworkItem->GetIndex(), pTCPNetworkItem->GetClientIP());
		
		FreeNetworkItem(pTCPNetworkItem);
		return true;
	}

	bool CTCPNetworkEngine::OnEventSocketRead(TCP_Command Command, void * pData, uint16 wDataSize, std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem)
	{
		//Ч������
		assert(pTCPNetworkItem != nullptr);
		assert(m_pITCPNetworkEngineEvent != nullptr);

		m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(pTCPNetworkItem->GetIndex(), Command, pData, wDataSize);
		return true;
	}

	bool CTCPNetworkEngine::SendData(uint32 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		//��������
		std::lock_guard<std::mutex> _lock(m_mutex);
		tagSendData * pSendDataRequest = (tagSendData *)m_cbBuffer;

		//��������
		pSendDataRequest->wDataSize = wDataSize;
		pSendDataRequest->wSubCmdID = wSubCmdID;
		pSendDataRequest->wMainCmdID = wMainCmdID;
		pSendDataRequest->dwSocketID = dwSocketID;
		if (wDataSize > 0)
		{
			assert(pData != NULL);
			memcpy(pSendDataRequest->cbSendBuffer, pData, wDataSize);
		}

		//��������
		uint16 wSendSize = sizeof(tagSendData) - sizeof(pSendDataRequest->cbSendBuffer) + wDataSize;
		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA, m_cbBuffer, wSendSize);
	}

	bool CTCPNetworkEngine::SendDataBatch(uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		//Ч������
		assert((wDataSize + sizeof(TCP_Head)) <= SOCKET_TCP_PACKET);
		if ((wDataSize + sizeof(TCP_Head)) > SOCKET_TCP_PACKET) return false;

		//��������
		std::lock_guard<std::mutex> _lock(m_mutex);
		tagBatchSendRequest * pBatchSendRequest = (tagBatchSendRequest *)m_cbBuffer;

		//��������
		pBatchSendRequest->wMainCmdID = wMainCmdID;
		pBatchSendRequest->wSubCmdID = wSubCmdID;
		pBatchSendRequest->wDataSize = wDataSize;

		if (wDataSize > 0)
		{
			assert(pData != nullptr);
			memcpy(pBatchSendRequest->cbSendBuffer, pData, wDataSize);
		}

		//��������
		uint16 wSendSize = sizeof(tagBatchSendRequest) - sizeof(pBatchSendRequest->cbSendBuffer) + wDataSize;
		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_BATCH, m_cbBuffer, wSendSize);
	}

	bool CTCPNetworkEngine::CloseSocket(uint32 dwSocketID)
	{
		std::lock_guard<std::mutex> _lock(m_mutex);
		tagCloseSocket *pCloseSocket = (tagCloseSocket*)m_cbBuffer;
		pCloseSocket->dwSocketID = dwSocketID;
		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_CLOSE_SOCKET, m_cbBuffer, sizeof(tagCloseSocket));
	}

	bool CTCPNetworkEngine::ShutDownSocket(uint32 dwSocketID)
	{
		std::lock_guard<std::mutex> _lock(m_mutex);
		tagShutDownSocket *pCloseSocket = (tagShutDownSocket*)m_cbBuffer;
		pCloseSocket->dwSocketID = dwSocketID;
		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SHUT_DOWN, m_cbBuffer, sizeof(tagShutDownSocket));
	}

	bool CTCPNetworkEngine::AllowBatchSend(uint32 dwSocketID, bool bAllowBatch)
	{
		std::lock_guard<std::mutex> _lock(m_mutex);
		tagAllowBatchSend *pAllowBatch = (tagAllowBatchSend*)m_cbBuffer;
		pAllowBatch->dwSocketID = dwSocketID;
		pAllowBatch->cbAllowBatch = bAllowBatch;
		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_ALLOW_BATCH, m_cbBuffer, sizeof(tagAllowBatchSend));
	}

	bool CTCPNetworkEngine::Start(Net::IOContext* ioContext)
	{
		//�첽����
		IUnknownEx * pIUnknownEx = QUERY_ME_INTERFACE(IUnknownEx);
		if (m_AsynchronismEngine.SetAsynchronismSink(pIUnknownEx) == false)
		{
			assert(nullptr);
			return false;
		}

		//��������
		if (m_AsynchronismEngine.Start(ioContext) == false)
		{
			assert(nullptr);
			return false;
		}

		assert(m_threadCount > 0);
		if (m_threadCount == 0) return false;

		AsyncAcceptor *acceptor = nullptr;
		try
		{
			acceptor = new AsyncAcceptor(*ioContext, m_strBindIP, m_port);
		}
		catch (asio::error_code const&)
		{
			assert(nullptr);
			return false;
		}

		if (!acceptor->Bind())
		{
			assert(nullptr);
			delete acceptor;
			return false;
		}

		m_acceptor = acceptor;
		m_pThreads = CreateThreads();

		for (int i = 0; i < 1; ++i)
		{
			m_pThreads[i].Start();
		}

		m_acceptor->AsyncAcceptWithCallBack(std::bind(&CTCPNetworkEngine::OnSocketOpen, this, std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	bool CTCPNetworkEngine::Stop()
	{
		m_acceptor->Close();

		for (int i = 0; i < m_threadCount; ++i)
		{
			m_pThreads[i].Stop();
		}

		Wait();

		PDELETE(m_acceptor);
		ADELETE(m_pThreads);
		m_threadCount = 0;

		return true;
	}

	void CTCPNetworkEngine::OnSocketOpen(tcp::socket && _socket, uint32 threadIndex)
	{
		try
		{
			std::shared_ptr<CTCPNetworkItem> newSocket = ActiveNetworkItem(std::move(_socket));
			m_pThreads[threadIndex].AddSocket(newSocket);
			newSocket->Start();
		}
		catch (asio::error_code const&)
		{
			assert(nullptr);
		}
	}

	int CTCPNetworkEngine::GetNetworkThreadCount() const
	{
		return m_threadCount;
	}

	uint32 CTCPNetworkEngine::SelectThreadWithMinConnections() const
	{
		uint32 min = 0;
		for (int i = 1; i < m_threadCount; ++i)
		{
			if (m_pThreads[i].GetConnectionCount() < m_pThreads[0].GetConnectionCount())
			{
				min = i;
			}
		}
		return min;
	}

	std::pair<tcp::socket*, uint32> CTCPNetworkEngine::GetAcceptSocket()
	{
		uint32 threadIndex = SelectThreadWithMinConnections();
		return std::make_pair(m_pThreads[threadIndex].GetAcceptSocket(), threadIndex);
	}

	void CTCPNetworkEngine::Wait()
	{
		for (int i = 0; i < m_threadCount; ++i)
		{
			m_pThreads[i].Wait();
		}
	}

	std::shared_ptr<CTCPNetworkItem> CTCPNetworkEngine::ActiveNetworkItem(tcp::socket && _socket)
	{
		//TODO...�ݲ��жϵ�ǰ������

		//��ȡ����
		std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem = nullptr;
#ifdef FORCE_CLOSE
		if (!m_NetworkFreeItem.empty())
		{
			pTCPNetworkItem = m_NetworkFreeItem.front();
			m_NetworkFreeItem.pop_front();
			pTCPNetworkItem->Attach(std::move(_socket));
		}
#endif
		//��������
		if (pTCPNetworkItem == nullptr)
		{
			pTCPNetworkItem = std::make_shared<CTCPNetworkItem>(m_curIndex, std::move(_socket), this);
			//pTCPNetworkItem = std::make_shared<CTCPNetworkItem>(m_curIndex++, std::move(_socket), this
			//		std::bind(&CTCPNetworkEngine::OnEventSocketBind, this, std::placeholders::_1),
			//		std::bind(&CTCPNetworkEngine::OnEventSocketShut, this, std::placeholders::_1),
			//		std::bind(&CTCPNetworkEngine::OnEventSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
			++m_curIndex;
		}

		m_NetItemStore.insert(std::make_pair(pTCPNetworkItem->GetIndex(), pTCPNetworkItem.get()));
		return pTCPNetworkItem;
	}

	CTCPNetworkItem* CTCPNetworkEngine::GetNetworkItem(uint32 dwSocket)
	{
		auto k = m_NetItemStore.find(dwSocket);
		if (k != m_NetItemStore.end())
		{
			return m_NetItemStore[dwSocket];
		}
		return nullptr;
	}

	bool CTCPNetworkEngine::FreeNetworkItem(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem)
	{
		std::unordered_map<uint32, CTCPNetworkItem*>::iterator it = m_NetItemStore.find(pTCPNetworkItem->GetIndex());
		if (it == m_NetItemStore.end())
		{
			assert(nullptr);
			return false;
		}
		std::map<uint32, CTCPNetworkItem*>::iterator itBatch = m_BatchNetItemStore.find(pTCPNetworkItem->GetIndex());
		if (itBatch != m_BatchNetItemStore.end())
		{
			m_BatchNetItemStore.erase(itBatch);
		}
		m_NetItemStore.erase(it);
		
#ifdef FORCE_CLOSE
		m_NetworkFreeItem.push_back(pTCPNetworkItem);
#endif
		return true;
	}

	CTCPNetworkThread<CTCPNetworkItem>* CTCPNetworkEngine::CreateThreads()
	{
		return new CTCPNetworkThread<CTCPNetworkItem>[1];
	}

	DECLARE_CREATE_MODULE(TCPNetworkEngine);
}