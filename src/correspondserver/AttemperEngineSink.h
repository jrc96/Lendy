#ifndef ATTEMPER_ENGINE_SINK_H
#define ATTEMPER_ENGINE_SINK_H

#include "KernelEngineHead.h"
#include "DBExports.h"
#include "GlobalInfoManager.h"

namespace Correspond
{
	using namespace Net;

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
		virtual bool OnEventTCPNetworkBind(uint32 dwClientAddr, uint32 dwSocketID);
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint32 dwClientAddr, uint32 dwSocketID);
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(TCP_Command Command, void * pData, uint16 wDataSize, uint32 dwSocketID);

		//�ӿ��¼�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize);

		//�ں��¼�
	public:
		//ʱ���¼�
		virtual bool OnEventTimer(uint32 dwTimerID) { return true; }
		
		//�����¼�
	protected:
		//ע�����
		bool OnTCPNetworkMainRegister(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//����״̬
		bool OnTCPNetworkMainServiceInfo(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//�û�����
		bool OnTCPNetworkMainUserCollect(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//Զ�̷���
		bool OnTCPNetworkMainRemoteService(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//�������
		bool OnTCPNetworkMainManagerService(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);
		//��������
		bool OnTCPNetworkMainAndroidService(uint16 wSubCmdID, void * pData, uint16 wDataSize, uint32 dwSocketID);

		//��������
	protected:
		//�����б�
		bool SendRoomListToLogon(uint32 dwSocketID);


		//״̬����
	protected:
		uint16							m_wCollectItem;						//��������
		std::vector<uint16>				m_WaitCollectItemArray;				//���ܵȴ�

	private:
		tagBindParameter *				m_pBindParameter;					//��������

		//����ӿ�
	protected:
		ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������

		//�������
	protected:
		CGlobalInfoManager				m_GlobalInfoManager;				//ȫ�ֹ���
	};
}

#endif