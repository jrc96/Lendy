#ifndef ATTEMPER_ENGINE_H
#define ATTEMPER_ENGINE_H

#include "../KernelEngineHead.h"
#include "AsynchronismEngine.h"
#include <mutex>

namespace Net
{
	class CAttemperEngine : public IAttemperEngine, public ITCPNetworkEngineEvent, public IAsynchronismEngineSink
	{
		//��������
public:
		//���캯��
		CAttemperEngine();
		//��������
		virtual ~CAttemperEngine();

		//����ӿ�
	public:
		//��������
		virtual bool Start(Net::IOContext* ioContext);
		//ֹͣ����
		virtual bool Stop();

		virtual void Release();
		virtual void *QueryInterface(GGUID uuid);

		//���ýӿ�
	public:
		//����ӿ�
		virtual bool SetNetworkEngine(IUnknownEx * pIUnknownEx);
		//�ص��ӿ�
		virtual bool SetAttemperEngineSink(IUnknownEx * pIUnknownEx);

		//�����¼�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize);

	public:
		//Ӧ���¼�
		virtual bool OnEventTCPNetworkBind(uint64 dwSocketID, uint64 dwClientAddr);
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint64 dwSocketID, uint64 dwClientAddr);
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(uint64 dwSocketID, Net::TCP_Command Command, void * pData, uint16 wDataSize);

		//�첽�ӿ�
	public:
		//�����¼�
		virtual bool OnAsynchronismEngineStart();
		//ֹͣ�¼�
		virtual bool OnAsynchronismEngineConclude();
		//�첽����
		virtual bool OnAsynchronismEngineData(uint16 wIdentifier, void * pData, uint16 wDataSize);

	private:
		ITCPNetworkEngine *			m_pITCPNetworkEngine;				//����ӿ�
		IAttemperEngineSink*		m_pIAttemperEngineSink;

		//�������
	protected:
		std::mutex					m_mutex;
		CAsynchronismEngine			m_AsynchronismEngine;				//�첽����

		//��������
	protected:
		BYTE						m_cbBuffer[SOCKET_TCP_BUFFER];		//��ʱ����
	};
}

#endif