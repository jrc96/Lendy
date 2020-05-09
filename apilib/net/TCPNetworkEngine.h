#ifndef TCP_NETWORK_ENGINE_H
#define TCP_NETWORK_ENGINE_H

#include "netdef/AsyncAcceptor.h"
#include "../KernelEngineHead.h"
#include "AsynchronismEngine.h"
#include "TCPNetworkThread.h"
#include "TCPNetworkItem.h"

#include <map>
#include <unordered_map>

namespace Net
{
	class CTCPNetworkEngine : public ITCPNetworkEngine, public ITCPNetworkItemSink, public IAsynchronismEngineSink
	{
		//��������
	public:
		//���캯��
		CTCPNetworkEngine();

		//��������
		virtual ~CTCPNetworkEngine();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release();
		//�ӿڲ�ѯ
		virtual void * QueryInterface(GGUID uuid);

	public:
		//���õ���
		virtual bool SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx);
		//���ò���
		virtual bool SetServiceParameter(std::string strBindIP, uint16 port, uint16 threadCount);

		//�첽�ӿ�
	public:
		//�����¼�
		virtual bool OnAsynchronismEngineStart() { return true; }
		//ֹͣ�¼�
		virtual bool OnAsynchronismEngineConclude() { return true; }
		//�첽����
		virtual bool OnAsynchronismEngineData(uint16 wIdentifier, void * pData, uint16 wDataSize);

	public:
		//���¼�
		virtual bool OnEventSocketBind(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);
		//�ر��¼�
		virtual bool OnEventSocketShut(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);
		//��ȡ�¼�
		virtual bool OnEventSocketRead(TCP_Command Command, void * pData, uint16 wDataSize, std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);

	public:
		//���ͺ���
		virtual bool SendData(uint32 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0);
		//��������
		virtual bool SendDataBatch(uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize);

		//���ƽӿ�
	public:
		//�ر�����
		virtual bool CloseSocket(uint32 dwSocketID);
		//���ùر�
		virtual bool ShutDownSocket(uint32 dwSocketID);
		//����Ⱥ��
		virtual bool AllowBatchSend(uint32 dwSocketID, bool bAllowBatch);

		//����ӿ�
	public:
		//��������
		virtual bool Start(Net::IOContext* ioContext);

		//ֹͣ����
		virtual bool Stop();

		void OnSocketOpen(tcp::socket &&_socket, uint32 threadIndex);

		int GetNetworkThreadCount() const;

		uint32 SelectThreadWithMinConnections() const;

		std::pair<tcp::socket*, uint32> GetAcceptSocket();

	public:
		void Wait();

		//�������
	protected:
		//������ж���
		std::shared_ptr<CTCPNetworkItem> ActiveNetworkItem(tcp::socket && _socket);
		//��ȡ����
		CTCPNetworkItem* GetNetworkItem(uint32 dwSocket);
		//�ͷ����Ӷ���
		bool FreeNetworkItem(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);

	protected:
		virtual CTCPNetworkThread<CTCPNetworkItem>* CreateThreads();

	private:
		uint16											m_curIndex;
		std::string										m_strBindIP;
		uint16											m_port;
		AsyncAcceptor*									m_acceptor;

		uint16											m_threadCount;
		CTCPNetworkThread<CTCPNetworkItem>*				m_pThreads;

#ifdef LENDY_SOCKET_FORCE_CLOSE
		std::deque<std::shared_ptr<CTCPNetworkItem>>	m_NetworkFreeItem;
#endif

		ITCPNetworkEngineEvent*							m_pITCPNetworkEngineEvent;			//�¼��ӿ�

	private:
		CAsynchronismEngine								m_AsynchronismEngine;				//�첽����
		std::unordered_map<uint32, CTCPNetworkItem*>	m_NetItemStore;
		std::map<uint32, CTCPNetworkItem*>				m_BatchNetItemStore;

		//��������
	protected:
		std::mutex						m_mutex;
		uint8							m_cbBuffer[SOCKET_TCP_BUFFER];		//��ʱ����
	};
}

#endif