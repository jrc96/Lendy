#ifndef ATTEMPER_ENGINE_SINK_H
#define ATTEMPER_ENGINE_SINK_H

#include "KernelEngineHead.h"
#include "DBExports.h"
#include "Header.h"

namespace Correspond
{
	//��������
	enum enServiceKind
	{
		ServiceKind_None,				//��Ч����
		ServiceKind_Game,				//��Ϸ����
		ServiceKind_Plaza,				//�㳡����
		ServiceKind_Chat,				//���ѷ���
	};

	//�󶨲���
	struct tagBindParameter
	{
		//�������
		uint32							dwSocketID;							//�����ʶ
		uint32							dwClientAddr;						//���ӵ�ַ
																	
		uint16							wServiceID;							//�����ʶ
		enServiceKind					ServiceKind;						//��������
	};

	class CAttemperEngineSink : public IAttemperEngineSink
	{
		friend class ServiceUnits;

	public:
		CAttemperEngineSink();
		virtual ~CAttemperEngineSink();

		virtual void Release();
		virtual void *QueryInterface(GGUID uuid);

		//�첽�ӿ�
	public:
		//�����¼�
		virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
		//ֹͣ�¼�
		virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

		//�����¼�
	public:
		//�����¼�
		virtual bool OnEventTCPSocketLink(uint16 wServiceID, int iErrorCode);
		//�ر��¼�
		virtual bool OnEventTCPSocketShut(uint16 wServiceID, uint8 cbShutReason);
		//��ȡ�¼�
		virtual bool OnEventTCPSocketRead(uint16 wServiceID, TCP_Command Command, void * pData, uint16 wDataSize);

		//�����¼�
	public:
		//Ӧ���¼�
		virtual bool OnEventTCPNetworkBind(uint64 dwClientAddr, uint64 dwSocketID);
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint64 dwClientAddr, uint64 dwSocketID);
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint64 dwSocketID);

		//�ӿ��¼�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize);
		
		//�����¼�
	protected:
		//ע�����
		bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
		//����״̬
		bool OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
		//�û�����
		bool OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
		//Զ�̷���
		bool OnTCPNetworkMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
		//�������
		bool OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
		//��������
		bool OnTCPNetworkMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	private:
		tagBindParameter *				m_pBindParameter;					//��������

		//����ӿ�
	protected:
		ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	};
}

#endif