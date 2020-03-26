#include "TCPSocketService.h"
#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
#include <WinSock2.h>

#define EINPROGRESS WSAEINPROGRESS
#define SHUT_RDWR SD_BOTH
#endif

namespace Net
{
#define IS_SS_RUN \
	bool bRun = m_TCPSocketServiceThread.IsStart(); \
	assert(bRun);	\
	if (!bRun) return false; 

#define IS_SS_STOP \
	bool bRun = m_TCPSocketServiceThread.IsStart(); \
	assert(!bRun);	\
	if (bRun) return false; 

#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                  (PCHAR)(address) - \
                                                  (ULONG_PTR)(&((type *)0)->field)))

#define REQUEST_CONNECT				1									//��������
#define REQUEST_SEND_DATA			2									//������
#define REQUEST_CLOSE_SOCKET		4									//����ر�

//���Ӵ���
#define CONNECT_SUCCESS				0									//���ӳɹ�
#define CONNECT_FAILURE				1									//����ʧ��
#define CONNECT_EXCEPTION			2									//�����쳣

	//��������
	struct tagConnectRequest
	{
		uint16							wPort;								//���Ӷ˿�
		uint64							dwServerIP;							//���ӵ�ַ
	};

	//��������
	struct tagSendDataRequest
	{
		uint16							wMainCmdID;							//��������
		uint16							wSubCmdID;							//��������
		uint16							wDataSize;							//���ݴ�С
		uint8							cbSendBuffer[SOCKET_TCP_PACKET];	//���ͻ���
	};

	CTCPSocketServiceThread::CTCPSocketServiceThread():
		m_ioContext(2),
		m_pStrand(new Net::Strand(m_ioContext)),
		m_pThread(nullptr),
		m_pThreadSelect(nullptr)
	{
	}

	CTCPSocketServiceThread::~CTCPSocketServiceThread()
	{
		Stop();
	}

	bool CTCPSocketServiceThread::Start()
	{
		if (m_pThread = new std::thread(&CTCPSocketServiceThread::Run, this), m_pThread == nullptr)
		{
			assert(nullptr);
			return false;
		}
		return true;
	}

	bool CTCPSocketServiceThread::Stop()
	{
		m_ioContext.stop();
		if (m_pThread)
		{
			m_pThread->join();
			PDELETE(m_pThread);
		}
		if (m_pThreadSelect)
		{
			m_pThreadSelect->join();
			PDELETE(m_pThreadSelect);
		}
		PDELETE(m_pStrand);
		return true;
	}

	bool CTCPSocketServiceThread::PostThreadRequest(uint16 wIdentifier, void * const pBuffer, uint16 wDataSize)
	{
		Net::post(m_ioContext, Net::bind_executor(*m_pStrand, [this, wIdentifier, pBuffer, wDataSize]() { 
			OnServiceRequest(wIdentifier, pBuffer, wDataSize);
		}));
		return true;
	}

	bool Net::CTCPSocketServiceThread::OnServiceRequest(uint16 wIdentifier, void * const pBuffer, uint16 wDataSize)
	{
		//���ݴ���
		switch (wIdentifier)
		{
			case REQUEST_CONNECT:		//��������
			{
				//Ч������
				assert(wDataSize == sizeof(tagConnectRequest));
				tagConnectRequest * pConnectRequest = (tagConnectRequest *)pBuffer;

				//�¼�֪ͨ
				uint64 uConnect = PerformConnect(pConnectRequest->dwServerIP, pConnectRequest->wPort);
//				int iErrorCode;
//#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
//				iErrorCode = WSAGetLastError();
//#else
//				iErrorCode = errno;
//#endif
//				CTCPSocketService * pTCPSocketStatusService = CONTAINING_RECORD(this, CTCPSocketService, m_TCPSocketServiceThread);
//				pTCPSocketStatusService->OnSocketLink(uConnect == CONNECT_SUCCESS ? 0 : iErrorCode);
				return true;
			}
			case REQUEST_SEND_DATA:
			{
				//Ч������
				assert(wDataSize == sizeof(tagSendDataRequest));
				tagSendDataRequest * pSendDataRequest = (tagSendDataRequest *)pBuffer;

				//���ݴ���
				PerformSendData(pSendDataRequest->wMainCmdID, pSendDataRequest->wSubCmdID);
				return true;
			}
			case REQUEST_CLOSE_SOCKET:
			{
				return true;
			}
		}
		return false;
	}

	uint64 CTCPSocketServiceThread::PerformConnect(uint64 dwServerIP, uint32 wPort)
	{
		try
		{
			if (!OnEventSocketCreate(AF_INET, SOCK_STREAM, IPPROTO_TCP))
			{
				throw CONNECT_EXCEPTION;
			}

			if (m_pThreadSelect = new std::thread(&CTCPSocketServiceThread::Loop, this), m_pThreadSelect == nullptr)
			{
				throw CONNECT_EXCEPTION;
			}

			struct sockaddr_in socketAddr;
			memset(&socketAddr, 0, sizeof(socketAddr));

			//���ñ���
			socketAddr.sin_family = AF_INET;
			socketAddr.sin_port = htons(wPort);
			socketAddr.sin_addr.S_un.S_addr = dwServerIP;
			//socketAddr.sin_addr.s_addr = dwServerIP;

			int nErrorCode = connect(m_hSocket, (const struct sockaddr *)&socketAddr, sizeof(socketAddr));
			if (nErrorCode == SOCKET_ERROR)
			{
#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
				if (WSAGetLastError() != WSAEWOULDBLOCK) throw CONNECT_EXCEPTION;
#else
				throw CONNECT_EXCEPTION;
#endif
			}
			return CONNECT_SUCCESS;
		}
		catch (...)
		{

		}
		
		return CONNECT_EXCEPTION;
	}

	uint64 Net::CTCPSocketServiceThread::PerformSendData(uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		//Ч��״̬
		if (m_hSocket == INVALID_SOCKET) return 0L;
		//if (m_TCPSocketStatus != SOCKET_STATUS_CONNECT) return 0L;

		//Ч���С
		assert(wDataSize <= SOCKET_TCP_PACKET);
		if (wDataSize > SOCKET_TCP_PACKET) return 0L;

		//��������
		uint8 cbDataBuffer[SOCKET_TCP_BUFFER];
		TCP_Head * pHead = (TCP_Head *)cbDataBuffer;

		//���ñ���
		pHead->CommandInfo.wSubCmdID = wSubCmdID;
		pHead->CommandInfo.wMainCmdID = wMainCmdID;

		//��������
		if (wDataSize > 0)
		{
			assert(pData != NULL);
			memcpy(pHead + 1, pData, wDataSize);
		}

		//��������
		uint16 wSendSize = EncryptBuffer(cbDataBuffer, sizeof(TCP_Head) + wDataSize, sizeof(cbDataBuffer));

		//��������
		return SendBuffer(cbDataBuffer, wSendSize);
	}

	bool Net::CTCPSocketServiceThread::OnEventSocketCreate(int af, int type, int protocol)
	{
		m_hSocket = ::socket(af, type, protocol);
		if (m_hSocket == INVALID_SOCKET) return false;

		int iResult = 0;
		int iShutdwonResult = 0;
#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
		unsigned long off;
		iResult = ioctlsocket(m_hSocket, FIONBIO, &off);
#else
		int flags = fcntl(m_hSocket, F_GETFL);
		iResult = fcntl(m_hSocket, F_SETFL, flags | O_NONBLOCK);
#endif
		if (iResult == SOCKET_ERROR)
		{
			if (m_hSocket != INVALID_SOCKET)
			{
				iShutdwonResult = shutdown(m_hSocket, SHUT_RDWR);
			}
			return false;
		}

		m_hMinSocket = m_hMaxSocket = m_hSocket;

		return true;
	}

	uint64 Net::CTCPSocketServiceThread::SendBuffer(void * pBuffer, uint16 wSendSize)
	{
		return uint64();
	}

	uint16 Net::CTCPSocketServiceThread::CrevasseBuffer(uint8 cbDataBuffer[], uint16 wDataSize)
	{
		return uint16();
	}

	uint16 Net::CTCPSocketServiceThread::EncryptBuffer(uint8 cbDataBuffer[], uint16 wDataSize, uint16 wBufferSize)
	{
		int i = 0;
		//Ч�����
		assert(wDataSize >= sizeof(TCP_Head));
		assert(wBufferSize >= (wDataSize + 2 * sizeof(DWORD)));
		assert(wDataSize <= (sizeof(TCP_Head) + SOCKET_TCP_BUFFER));

		//��д��Ϣͷ
		TCP_Head * pHead = (TCP_Head *)cbDataBuffer;
		pHead->TCPInfo.wPacketSize = wDataSize;
		pHead->TCPInfo.cbDataKind = DK_MAPPED;


		BYTE checkCode = 0;

		for (uint16 i = sizeof(TCP_Info); i < wDataSize; i++)
		{
			checkCode += cbDataBuffer[i];
			cbDataBuffer[i] = g_SendByteMap[cbDataBuffer[i]];
		}
		pHead->TCPInfo.cbCheckCode = ~checkCode + 1;

		return wDataSize;
	}

	void CTCPSocketServiceThread::Run()
	{
		//�¼�֪ͨ
		asio::io_context::work work(m_ioContext);
		m_ioContext.run();
	}

	void Net::CTCPSocketServiceThread::Loop()
	{
		try
		{
			fd_set readset, o_readset;
			fd_set writeset, o_writeset;
			fd_set exceptset, o_exceptset;
			
			m_tTimeOut.tv_sec = 0;
			m_tTimeOut.tv_usec = 0;

			FD_ZERO(&readset);
			FD_ZERO(&writeset);
			FD_ZERO(&exceptset);
			FD_SET(m_hSocket, &readset);
			FD_SET(m_hSocket, &writeset);
			FD_SET(m_hSocket, &exceptset);

			o_readset = readset;
			o_writeset = writeset;
			o_exceptset = exceptset;

			while (true)
			{
#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
				Sleep(100);
#else
				usleep(100 * 1000);
#endif
				int result = select(m_hMaxSocket + 1, &readset, &writeset, &exceptset, &m_tTimeOut);
				if (result == SOCKET_ERROR)
				{
					break;
				}

				if (result <= 0) continue;

				//�ɶ�
				if (FD_ISSET(m_hSocket, &readset))
				{

				}
				else if (FD_ISSET(m_hSocket, &writeset))
				{
					printf("11111111\n");
					int error = 0;
					socklen_t len = sizeof(error);
					if (getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)
					{
						break;
					}
				}

				readset = o_readset;
				writeset = o_writeset;
				exceptset = o_exceptset;
			}
		}
		catch (...)
		{

		}
	}

	CTCPSocketService::CTCPSocketService() :
		m_wServiceID(0),
		m_pITCPSocketEvent(nullptr)
	{
		memset(m_cbBuffer, 0, sizeof(m_cbBuffer));
	}

	CTCPSocketService::~CTCPSocketService()
	{
		Stop();
	}

	void * CTCPSocketService::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(IServiceModule, uuid);
		QUERY_INTERFACE(ITCPSocketService, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(ITCPSocketService, uuid);
		return nullptr;
	}

	bool CTCPSocketService::Start(Net::IOContext * ioContext)
	{
		IS_SS_STOP
		if (!m_TCPSocketServiceThread.Start())
		{
			return false;
		}
		return true;
	}

	bool CTCPSocketService::Stop()
	{
		IS_SS_RUN
		m_TCPSocketServiceThread.Stop();
		return true;
	}

	bool CTCPSocketService::SetServiceID(uint16 wServiceID)
	{
		IS_SS_RUN
		m_wServiceID = wServiceID;
		return true;
	}

	bool CTCPSocketService::SetTCPSocketEvent(IUnknownEx * pIUnknownEx)
	{
		IS_SS_RUN
		m_pITCPSocketEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPSocketEvent);

		//�����ж�
		if (m_pITCPSocketEvent == nullptr)
		{
			assert(nullptr);
			return false;
		}
		return true;
	}

	bool CTCPSocketService::CloseSocket()
	{
		IS_SS_RUN
		std::lock_guard<std::mutex> _lock(m_mutex);
		return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CLOSE_SOCKET, nullptr, 0);
	}

	bool CTCPSocketService::Connect(uint64 dwServerIP, uint16 wPort)
	{
		IS_SS_RUN
		std::lock_guard<std::mutex> _lock(m_mutex);

		tagConnectRequest *pConnectRequest = (tagConnectRequest *)m_cbBuffer;
		pConnectRequest->wPort = wPort;
		pConnectRequest->dwServerIP = htonl(dwServerIP);
		return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CONNECT, &m_cbBuffer, sizeof(tagConnectRequest));
	}

	bool CTCPSocketService::Connect(std::string strServerIP, uint16 wPort)
	{
		IS_SS_RUN
		std::lock_guard<std::mutex> _lock(m_mutex);

		tagConnectRequest *pConnectRequest = (tagConnectRequest *)m_cbBuffer;
		pConnectRequest->wPort = wPort;
		pConnectRequest->dwServerIP = inet_addr(strServerIP.c_str());
		return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CONNECT, &m_cbBuffer, sizeof(tagConnectRequest));
	}

	bool CTCPSocketService::SendData(uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize)
	{
		//״̬Ч��
		IS_SS_RUN
		std::lock_guard<std::mutex> _lock(m_mutex);

		//��������
		tagSendDataRequest SendRequest;
		memset(&SendRequest, 0, sizeof(SendRequest));

		//���ñ���
		SendRequest.wDataSize = wDataSize;
		SendRequest.wSubCmdID = wSubCmdID;
		SendRequest.wMainCmdID = wMainCmdID;

		//��������
		if (wDataSize > 0)
		{
			assert(pData != NULL);
			memcpy(SendRequest.cbSendBuffer, pData, wDataSize);
		}

		//Ͷ������
		uint16 wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuffer) + wDataSize;
		return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_SEND_DATA, &SendRequest, wSendSize);
	}

	bool CTCPSocketService::OnSocketLink(int nErrorCode)
	{
		//Ͷ���¼�
		assert(m_pITCPSocketEvent != NULL);
		return m_pITCPSocketEvent->OnEventTCPSocketLink(m_wServiceID, nErrorCode);
	}

	//�����������
	DECLARE_CREATE_MODULE(TCPSocketService);
}