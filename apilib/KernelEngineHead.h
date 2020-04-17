#ifndef KERNEL_ENGINE_HEAD_H
#define KERNEL_ENGINE_HEAD_H

#include <asio/ip/tcp.hpp>
#include "IOContext.h"
#include "Moudle.h"
#include "Packet.h"
//#include "DBEnvHeader.h"

#define NETWORK_ENGINE_DLL_NAME		"Net.dll"
//#define DATABASE_ENGINE_DLL_NAME	"DataBase.dll"

static GGUID IID_IServiceModule = { 0xee6657db, 0x739e, 0x48c4, { 0x96, 0x73, 0x25, 0xce, 0x44, 0xd0, 0xc1, 0xf } };
struct IServiceModule : public IUnknownEx
{
public:
	virtual bool Start(Net::IOContext*) = 0;
	virtual bool Stop() = 0;
};

///////////
namespace Net
{
	static GGUID IID_IAsynchronismEngine = { 0x50312e19, 0x7245, 0x4ea7, { 0xac, 0xb0, 0x2c, 0xe8, 0xe4, 0xfd, 0xcd, 0x9c } };
	static GGUID IID_IAsynchronismEngineSink = { 0x22131182, 0xfdb1, 0x4cfe, { 0xa2, 0xa1, 0x14, 0x19, 0x29, 0x87, 0xca, 0x4b } };
	static GGUID IID_ITCPNetworkEngine = { 0xc876f8aa, 0x199f, 0x48fc, { 0x96, 0x1a, 0x6, 0x67, 0xbc, 0xf3, 0xad, 0xdf } };
	static GGUID IID_IAttemperEngine = { 0x2b972bac, 0x1c76, 0x4e3c, { 0xb9, 0x19, 0x39, 0x43, 0xf4, 0x31, 0x71, 0x83 } };
	static GGUID IID_ITCPNetworkEngineEvent = { 0x507976f6, 0xfc24, 0x4c51, { 0xac, 0xd, 0x60, 0x2f, 0x87, 0x71, 0xfc, 0x83 } };
	static GGUID IID_IAttemperEngineSink = { 0x3620c4d7, 0xfe6, 0x4eee, { 0xad, 0x4f, 0x79, 0xac, 0x23, 0x97, 0xc7, 0x62 } };
	static GGUID IID_ITCPSocketService = { 0x3fbd00dc, 0x72ba, 0x4686, { 0xa4, 0xa1, 0x1e, 0xd2, 0x10, 0x14, 0xbb, 0xd0 } };
	static GGUID IID_ITCPSocketEvent = { 0x82c3bfaa, 0xc4c8, 0x40a8, { 0x9e, 0xd7, 0x15, 0xfb, 0x6d, 0xf2, 0x40, 0xbf } };
	static GGUID IID_ITimerEngine = { 0xc3e77af5, 0x297d, 0x4708, { 0xa2, 0x7b, 0x8f, 0xa8, 0x71, 0xcb, 0x16, 0x59 } };
	static GGUID IID_ITimerEngineEvent = { 0x5c0ff1b6, 0x659d, 0x44fe, { 0x9c, 0xcc, 0x50, 0xc3, 0xf7, 0x86, 0x4a, 0x4d } };

	//////////////////////////////service/////////////////////////////
	//����ӿ�
	struct ITCPSocketService : public IServiceModule
	{
		//���ýӿ�
	public:
		//���ú���
		virtual bool SetServiceID(uint16 wServiceID) = 0;
		//���ýӿ�
		virtual bool SetTCPSocketEvent(IUnknownEx * pIUnknownEx) = 0;

		//���ܽӿ�
	public:
		//�ر�����
		virtual bool CloseSocket() = 0;
		//���ӵ�ַ
		virtual bool Connect(uint64 dwServerIP, uint16 wPort) = 0;
		//���ӵ�ַ
		virtual bool Connect(std::string strServerIP, uint16 wPort) = 0;
		//���ͺ���
		virtual bool SendData(uint16 wMainCmdID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0) = 0;
	};

	//�����¼�
	struct ITCPSocketEvent : public IUnknownEx
	{
		//�����¼�
		virtual bool OnEventTCPSocketLink(uint16 wServiceID, int nErrorCode) = 0;
		//�ر��¼�
		virtual bool OnEventTCPSocketShut(uint16 wServiceID, uint8 cbShutReason) = 0;
		//��ȡ�¼�
		virtual bool OnEventTCPSocketRead(uint16 wServiceID, TCP_Command Command, void * pData, uint16 wDataSize) = 0;
	};

	//////////////////////////////client/////////////////////////////
	//�첽����
	struct IAsynchronismEngine : public IServiceModule
	{
		//���ýӿ�
	public:
		//����ģ��
		virtual bool SetAsynchronismSink(IUnknownEx * pIUnknownEx) = 0;

		//�첽����
		virtual bool PostAsynchronismData(uint16 wIdentifier, void * pData, uint16 wDataSize) = 0;
	};

	//�첽����
	struct IAsynchronismEngineSink : public IUnknownEx
	{
		//�����¼�
		virtual bool OnAsynchronismEngineStart() = 0;
		//ֹͣ�¼�
		virtual bool OnAsynchronismEngineConclude() = 0;
		//�첽����
		virtual bool OnAsynchronismEngineData(uint16 wIdentifier, void * pData, uint16 wDataSize) = 0;
	};

	//��������
	struct IAttemperEngine : public IServiceModule
	{
		//���ýӿ�
	public:
		//����ӿ�
		virtual bool SetNetworkEngine(IUnknownEx * pIUnknownEx) = 0;
		//�ص��ӿ�
		virtual bool SetAttemperEngineSink(IUnknownEx * pIUnknownEx) = 0;

		//�����¼�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wControlID, void * pData, uint16 wDataSize) = 0;
	};

	struct ITCPNetworkEngine : public IServiceModule
	{
	public:
		//���õ���
		virtual bool SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx) = 0;
		//���ò���
		virtual bool SetServiceParameter(std::string strBindIP, uint16 wServicePort, uint16 threadCount) = 0;

	public:
		//���ͺ���
		virtual bool SendData(uint64 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0) = 0;
	
		//���ƽӿ�
	public:
		//�ر�����
		virtual bool CloseSocket(uint64 dwSocketID) = 0;
	};

	//�����¼�
	struct ITCPNetworkEngineEvent : public IServiceModule
	{
		//�ӿڶ���
	public:
		//Ӧ���¼�
		virtual bool OnEventTCPNetworkBind(uint64 dwSocketID, uint64 dwClientAddr) = 0;
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint64 dwSocketID, uint64 dwClientAddr) = 0;
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(uint64 dwSocketID, Net::TCP_Command Command, void * pData, uint16 wDataSize) = 0;
	};

	//���ȹ���
	struct IAttemperEngineSink : public IUnknownEx
	{
		//�첽�ӿ�
	public:
		//�����¼�
		virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx) = 0;
		//ֹͣ�¼�
		virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx) = 0;

		//�¼��ӿ�
	public:
		//�����¼�
		virtual bool OnEventControl(uint16 wIdentifier, void * pData, uint16 wDataSize) = 0;

		//�ں��¼�
	public:
		//ʱ���¼�
		virtual bool OnEventTimer(uint32 dwTimerID) = 0;

		//�����¼�
	public:
		//�����¼�
		virtual bool OnEventTCPSocketLink(uint16 wServiceID, int iErrorCode) = 0;
		//�ر��¼�
		virtual bool OnEventTCPSocketShut(uint16 wServiceID, uint8 cbShutReason) = 0;
		//��ȡ�¼�
		virtual bool OnEventTCPSocketRead(uint16 wServiceID, TCP_Command Command, void * pData, uint16 wDataSize) = 0;

		//�����¼�
	public:
		//Ӧ���¼�
		virtual bool OnEventTCPNetworkBind(uint64 dwClientAddr, uint64 dwSocketID) = 0;
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint64 dwClientAddr, uint64 dwSocketID) = 0;
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint64 dwSocketID) = 0;
	};

	//��ʱ������
	struct ITimerEngine : public IServiceModule
	{
		//���ýӿ�
	public:
		//���ýӿ�
		virtual bool SetTimerEngineEvent(IUnknownEx * pIUnknownEx) = 0;

		//���ܽӿ�
	public:
		//���ö�ʱ��
		virtual bool SetTimer(uint32 dwTimerID, uint32 dwElapse, uint32 dwRepeat) = 0;
		//ɾ����ʱ��
		virtual bool KillTimer(uint32 dwTimerID) = 0;
		//ɾ����ʱ��
		virtual bool KillAllTimer() = 0;
	};

	//��ʱ���¼�
	struct ITimerEngineEvent : public IUnknownEx
	{
		//�ӿڶ���
	public:
		//ʱ���¼�
		virtual bool OnEventTimer(uint32 dwTimerID) = 0;
	};

	DECLARE_MOUDLE_HELPER(TimerEngine, NETWORK_ENGINE_DLL_NAME, "CreateTimerEngine")
	DECLARE_MOUDLE_HELPER(AttemperEngine, NETWORK_ENGINE_DLL_NAME, "CreateAttemperEngine")
	DECLARE_MOUDLE_HELPER(TCPNetworkEngine, NETWORK_ENGINE_DLL_NAME, "CreateTCPNetworkEngine")
	DECLARE_MOUDLE_HELPER(TCPSocketService, NETWORK_ENGINE_DLL_NAME, "CreateTCPSocketService")
}

#endif