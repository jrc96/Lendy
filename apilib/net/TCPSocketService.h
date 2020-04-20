#ifndef TCP_SOCKET_SERVICE_H
#define TCP_SOCKET_SERVICE_H

#include "../KernelEngineHead.h"
#include "Strand.h"
#include "DataQueue.h"
#include <thread>
#include <mutex>

namespace Net
{
	class CTCPSocketServiceThread
	{
		//��������
	public:
		//���캯��
		CTCPSocketServiceThread();
		//��������
		virtual ~CTCPSocketServiceThread();

		//����ӿ�
	public:
		//��������
		virtual bool Start();
		//ֹͣ����
		virtual bool Stop();

		bool IsStart() { return m_pThread != nullptr; }

		//������
	public:
		//Ͷ������
		bool PostThreadRequest(uint16 wIdentifier, void * const pBuffer, uint16 wDataSize);

		//������
	private:
		//������Ϣ
		bool OnServiceRequest(uint16 wIdentifier, void * const pBuffer, uint16 wDataSize);

		//���ƺ���
	private:
		//���ӷ�����
		uint64 PerformConnect(uint64 dwServerIP, uint32 wPort);

		//���ͺ���
		uint64 PerformSendData(uint16 wMainCmdID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0);

		//����socket
		bool OnEventSocketCreate(int af, int type, int protocol);

		//��������
	private:
		//�����ȡ
		bool OnSocketNotifyRead();
		//���緢��
		bool OnSocketNotifyWrite();

		//��������
	private:
		//�ر�����
		void PerformCloseSocket(uint8 cbShutReason);
		//��������
		uint64 SendBuffer(void * pBuffer, uint16 wSendSize);
		//��������
		uint16 CrevasseBuffer(uint8 cbDataBuffer[], uint16 wDataSize);
		//��������
		uint16 EncryptBuffer(uint8 cbDataBuffer[], uint16 wDataSize, uint16 wBufferSize);

		//�ڲ�����
	protected:
		void Run();

		void Loop();

		//�ں˱���
	protected:
		SOCKET								m_hSocket;							//���Ӿ��
		timeval								m_tTimeOut;

		//�ڲ��Q��
	private:
		std::mutex							m_mutex;
		Util::DataQueue						m_dataQueue;

		//���ձ���
	protected:
		uint16								m_wRecvSize;						//���ճ���
		uint8								m_cbRecvBuf[SOCKET_TCP_BUFFER * 10];//���ջ���

	protected:
		Net::IOContext 						m_ioContext;
		Net::Strand*						m_pStrand;
		std::thread*						m_pThread;
		std::thread*						m_pThreadSelect;
	};

	//�������
	class CTCPSocketService : public ITCPSocketService
	{
		//��Ԫ˵��
		friend class CTCPSocketServiceThread;

	protected:
		uint16							m_wServiceID;						//�����ʶ
		ITCPSocketEvent*				m_pITCPSocketEvent;					//�¼��ӿ�

		//��������
	protected:
		std::mutex							m_mutex;
		uint8								m_cbBuffer[SOCKET_TCP_BUFFER];		//��ʱ����

		//�������
	protected:
		CTCPSocketServiceThread			m_TCPSocketServiceThread;			//�����߳�

		//��������
	public:
		//���캯��
		CTCPSocketService();
		//��������
		virtual ~CTCPSocketService();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release() { delete this; }
		//�ӿڲ�ѯ
		virtual void *QueryInterface(GGUID uuid);

		//����ӿ�
	public:
		//��������
		virtual bool Start(Net::IOContext* ioContext);

		//ֹͣ����
		virtual bool Stop();

		//���ýӿ�
	public:
		//���ú���
		virtual bool SetServiceID(uint16 wServiceID);
		//���ýӿ�
		virtual bool SetTCPSocketEvent(IUnknownEx * pIUnknownEx);

		//���ܽӿ�
	public:
		//�ر�����
		virtual bool CloseSocket();
		//���ӵ�ַ
		virtual bool Connect(uint64 dwServerIP, uint16 wPort);
		//���ӵ�ַ
		virtual bool Connect(std::string strServerIP, uint16 wPort);
		//���ͺ���
		virtual bool SendData(uint16 wMainCmdID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0);

		//��������
	protected:
		//������Ϣ
		bool OnSocketLink(int nErrorCode);
		//�ر���Ϣ
		bool OnSocketShut(uint8 cbShutReason);
		//������Ϣ
		bool OnSocketRead(TCP_Command Command, void * pData, uint16 wDataSize);
	};
}

#endif