#ifndef KERNEL_ENGINE_HEAD_H
#define KERNEL_ENGINE_HEAD_H

#include <asio/ip/tcp.hpp>
#include "IOContext.h"
#include "Moudle.h"
#include "Packet.h"
//#include "DBEnvHeader.h"

#define NETWORK_ENGINE_DLL_NAME		"Net.dll"
//#define DATABASE_ENGINE_DLL_NAME	"DataBase.dll"

static GGUID IID_IServiceMoudle = { 0xee6657db, 0x739e, 0x48c4, { 0x96, 0x73, 0x25, 0xce, 0x44, 0xd0, 0xc1, 0xf } };
struct IServiceMoudle : public IUnknownEx
{
public:
	virtual bool Start(Net::IOContext*) = 0;
	virtual bool Stop() = 0;
};

///////////
namespace Net
{
	static GGUID IID_ITCPNetworkEngine = { 0xc876f8aa, 0x199f, 0x48fc, { 0x96, 0x1a, 0x6, 0x67, 0xbc, 0xf3, 0xad, 0xdf } };
	static GGUID IID_IAttemperEngine = { 0x2b972bac, 0x1c76, 0x4e3c, { 0xb9, 0x19, 0x39, 0x43, 0xf4, 0x31, 0x71, 0x83 } };
	static GGUID IID_ITCPNetworkEngineEvent = { 0x507976f6, 0xfc24, 0x4c51, { 0xac, 0xd, 0x60, 0x2f, 0x87, 0x71, 0xfc, 0x83 } };
	static GGUID IID_IAttemperEngineSink = { 0x3620c4d7, 0xfe6, 0x4eee, { 0xad, 0x4f, 0x79, 0xac, 0x23, 0x97, 0xc7, 0x62 } };
	
	//��������
	struct IAttemperEngine : public IServiceMoudle
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

	struct ITCPNetworkEngine : public IServiceMoudle
	{
	public:
		//���õ���
		virtual bool SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx) = 0;
		//���ò���
		virtual bool SetServiceParameter(std::string strBindIP, uint16 wServicePort, uint16 threadCount) = 0;

	public:
		//���ͺ���
		virtual bool SendData(uint64 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize) = 0;
	};

	//�����¼�
	struct ITCPNetworkEngineEvent : public IServiceMoudle
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

		//�����¼�
	public:
		//Ӧ���¼�
		virtual bool OnEventTCPNetworkBind(uint64 dwClientAddr, uint64 dwSocketID) = 0;
		//�ر��¼�
		virtual bool OnEventTCPNetworkShut(uint64 dwClientAddr, uint64 dwSocketID) = 0;
		//��ȡ�¼�
		virtual bool OnEventTCPNetworkRead(Net::TCP_Command Command, void * pData, uint16 wDataSize, uint64 dwSocketID) = 0;
	};

	DECLARE_MOUDLE_HELPER(AttemperEngine, NETWORK_ENGINE_DLL_NAME, "CreateAttemperEngine")
	DECLARE_MOUDLE_HELPER(TCPNetworkEngine, NETWORK_ENGINE_DLL_NAME, "CreateTCPNetworkEngine")
}

//namespace DB
//{
//	static GGUID IID_IDataBaseEngine = { 0xacfc49b6, 0xa29e, 0x47f8, { 0x9c, 0xbd, 0x80, 0x9d, 0xa0, 0xba, 0xc8, 0x4e } };
//
//	struct IDataBaseEngine : public IServiceMoudle
//	{
//
//	public:
//		virtual bool SetPreparedStatement(std::string const& dbname) = 0;
//
//		//��ȡԤ��
//		virtual PreparedStatement* GetPreparedStatement(LoginDatabaseStatements index) = 0;
//
//		//ͬ����ѯ
//		virtual PreparedQueryResult Query(PreparedStatement* stmt) = 0;
//
//		//�첽��ѯ
//		virtual QueryCallback AsyncQuery(PreparedStatement* stmt) = 0;
//	};
//
//	DECLARE_MOUDLE_HELPER(DataBaseEngine, DATABASE_ENGINE_DLL_NAME, "CreateDataBaseEngine")
//}

#endif