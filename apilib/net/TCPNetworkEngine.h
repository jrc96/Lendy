#ifndef TCP_NETWORK_ENGINE_H
#define TCP_NETWORK_ENGINE_H

#include "netdef/AsyncAcceptor.h"
#include "../KernelEngineHead.h"
#include "TCPNetworkThread.h"
#include "TCPNetworkItem.h"

namespace Net
{
	class CTCPNetworkEngine : public ITCPNetworkEngine, public ITCPNetworkItemSink
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

	public:
		//���¼�
		virtual bool OnEventSocketBind(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);
		//�ر��¼�
		virtual bool OnEventSocketShut(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);
		//��ȡ�¼�
		virtual bool OnEventSocketRead(TCP_Command Command, void * pData, uint16 wDataSize, std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);

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
		std::shared_ptr<CTCPNetworkItem> GetNetworkItem(uint16 wIndex);
		//�ͷ����Ӷ���
		bool FreeNetworkItem(std::shared_ptr<CTCPNetworkItem> pTCPNetworkItem);

	protected:
		virtual CTCPNetworkThread<CTCPNetworkItem>* CreateThreads();

	private:
		uint16								m_curIndex;
		std::string							m_strBindIP;
		uint16								m_port;
		AsyncAcceptor*						m_acceptor;

		uint16								m_threadCount;
		CTCPNetworkThread<CTCPNetworkItem>*	m_pThreads;
		std::deque<std::shared_ptr<CTCPNetworkItem>> m_NetworkFreeItem;

		ITCPNetworkEngineEvent*				m_pITCPNetworkEngineEvent;			//�¼��ӿ�
	};
}

#endif