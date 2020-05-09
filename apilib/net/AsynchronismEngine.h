#ifndef ASYNCHRONISM_ENGINE_H
#define ASYNCHRONISM_ENGINE_H

#include "../KernelEngineHead.h"
#include "Strand.h"
#include "DataQueue.h"
#include <thread>
#include <mutex>

namespace Net
{
	class CAsynchronismEngine : public IAsynchronismEngine
	{
		//��������
	public:
		//���캯��
		CAsynchronismEngine();
		//��������
		virtual ~CAsynchronismEngine();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release() { delete this; }
		//�ӿڲ�ѯ
		virtual void *QueryInterface(GGUID uuid);

		//����ӿ�
	public:
		//��������
		virtual bool Start(Net::IOContext*);
		//ֹͣ����
		virtual bool Stop();

		//�첽�ӿ�
	public:
		//����ģ��
		virtual bool SetAsynchronismSink(IUnknownEx * pIUnknownEx);
		//�첽����
		virtual bool PostAsynchronismData(uint16 wIdentifier, void * pData, uint16 wDataSize);

	protected:
		void Run();

	protected:
		IAsynchronismEngineSink *			m_pIAsynchronismEngineSink;			//�ص��ӿ�

		std::shared_ptr<Net::IOContext> 	m_ioContext;
		Net::Strand*						m_pStrand;
		std::thread*						m_pThread;

		//��������
	private:
		std::mutex							m_mutex;
		uint8								m_cbBuffer[SOCKET_TCP_BUFFER];	//���ջ���
		Util::DataQueue						m_dataQueue;
	};
}

#endif