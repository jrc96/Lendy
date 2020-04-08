#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "Define.h"
#include "KernelEngineHead.h"
#include <vector>

//�ṹ��
namespace Game
{
	//��������
#define SUB_GAME_CREATE_NAME	"CreateGameServiceManager"			//��������

	struct IGameServiceManager;

	//��������
	struct tagGameServiceOption
	{
		//�ں�����
		uint16							wKindID;							//���ƺ���
		uint16							wServerID;							//�����ʶ
		uint16							wServerPort;						//����˿�
		uint16							wChairCount;						//������Ŀ

		//���ܱ�־
		uint8							cbDynamicJoin;						//��̬����
		uint8							cbOffLineTrustee;					//���ߴ���

		//˰������
		uint64							lCellScore;							//��λ����
		uint16							wRevenueRatio;						//˰�ձ���
		uint64							lServiceScore;						//�������

		//��������
		uint64							lMinEnterScore;						//��ͻ���
		uint64							lMaxEnterScore;						//��߻���

		//��������
		uint16							wMaxPlayer;							//�����Ŀ
		uint16							wTableCount;						//������Ŀ

		//��������
		char							strGameName[32];					//��Ϸ����
		char							strServerDLLName[32];				//��������

		//�Զ�����
		std::vector<uint8>				vCustomRule;						//��Ϸ����
	};

	//���Ӳ���
	struct tagTableFrameParameter
	{
		IGameServiceManager *			pIGameServiceManager;				//�������
		Net::ITCPSocketService *		pITCPSocketService;					//�������
		tagGameServiceOption *			pGameServiceOption;
	};
}

//Ѱ�����
namespace Game
{
	static GGUID IID_IGameServiceManager = { 0x39876a3c, 0x7f9a, 0x4ef5, { 0xb2, 0x2a, 0x78, 0x3c, 0x4b, 0x97, 0x84, 0x33 } };
	static GGUID IID_ITableFrameSink = { 0x68b59d58, 0x9153, 0x4102, { 0x89, 0xec, 0x39, 0x79, 0x81, 0x53, 0x9b, 0xa7 } };
	static GGUID IID_IServerUserItem = { 0x3e5dee5f, 0xcf19, 0x4f86, { 0xbb, 0x9a, 0x81, 0x93, 0x3a, 0xe3, 0x72, 0x42 } };

	//��Ϸ�ӿ�
	struct IGameServiceManager : public IUnknownEx
	{
		//�����ӿ�
	public:
		//��������
		virtual void * CreateTableFrameSink(GGUID Guid) = 0;
		//��������
		virtual void * CreateAndroidUserItemSink(GGUID Guid) = 0;
		//��������
		virtual void * CreateGameDataBaseEngineSink(GGUID Guid) = 0;

		//�����ӿ�
	public:
		//��������
		virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption) = 0;
	};

	//�û��ӿ�
	struct IServerUserItem : public IUnknownEx
	{
		//������Ϣ
	public:
		//�û�����
		virtual uint16 GetBindIndex() = 0;
		//�û���ַ
		virtual uint64 GetClientAddr() = 0;
		//������ʶ
		virtual wchar_t* GetMachineID() = 0;

		//��¼��Ϣ
	public:
		//�����ʶ
		virtual uint64 GetDBQuestID() = 0;
		//��¼ʱ��
		virtual uint64 GetLogonTime() = 0;
		//��¼����
		virtual uint64 GetInoutIndex() = 0;

		//������Ϣ
	public:
		//�û��Ա�
		virtual uint8 GetGender() = 0;
		//�û���ʶ
		virtual uint64 GetUserID() = 0;
		//��Ϸ��ʶ
		virtual uint64 GetGameID() = 0;
		//�û��ǳ�
		virtual wchar_t* GetNickName() = 0;

		//״̬�ӿ�
	public:
		//���Ӻ���
		virtual uint16 GetTableID() = 0;
		//���Ӻ���
		virtual uint16 GetLastTableID() = 0;
		//���Ӻ���
		virtual uint16 GetChairID() = 0;
		//�û�״̬
		virtual uint8 GetUserStatus() = 0;

		//������Ϣ
	public:
		//�û�����
		virtual uint64 GetUserScore() = 0;

		//������Ϣ
	public:
		//�û�ʤ��
		virtual uint16 GetUserWinRate() = 0;
		//�û�����
		virtual uint16 GetUserLostRate() = 0;
		//�û�����
		virtual uint16 GetUserDrawRate() = 0;
		//�û�����
		virtual uint16 GetUserFleeRate() = 0;
		//��Ϸ����
		virtual uint16 GetUserPlayCount() = 0;

		//�й�״̬
	public:
		//�ж�״̬
		virtual bool IsTrusteeUser() = 0;
		//����״̬
		virtual void SetTrusteeUser(bool bTrusteeUser) = 0;

		//��Ϸ״̬
	public:
		//����״̬
		virtual bool IsClientReady() = 0;
		//��������
		virtual void SetClientReady(bool bClientReady) = 0;

		//����ӿ�
	public:
		//����״̬
		virtual bool SetUserStatus(uint8 cbUserStatus, uint16 wTableID, uint16 wChairID) = 0;
	};

	//�ص��ӿ�
	struct ITableFrameSink : public IUnknownEx
	{
		//����ӿ�
	public:
		//��λ�ӿ�
		virtual void RepositionSink() = 0;
		//���ýӿ�
		virtual bool Initialization(IUnknownEx * pIUnknownEx) = 0;

		//��Ϸ�¼�
	public:
		//��Ϸ��ʼ
		virtual bool OnEventGameStart() = 0;
		//��Ϸ����
		virtual bool OnEventGameConclude(uint16 wChairID, IServerUserItem * pIServerUserItem, uint8 cbReason) = 0;
		//���ͳ���
		virtual bool OnEventSendGameScene(uint16 wChairID, IServerUserItem * pIServerUserItem, uint8 cbGameStatus, bool bSendSecret) = 0;

		//����ӿ�
	public:
		//��Ϸ��Ϣ
		virtual bool OnGameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IServerUserItem * pIServerUserItem) = 0;
		//�����Ϣ
		virtual bool OnFrameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IServerUserItem * pIServerUserItem) = 0;
	};

	DECLARE_MOUDLE_DYNAMIC(GameServiceManager)
}

#endif