#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include "Define.h"
#include "Struct.h"
#include "KernelEngineHead.h"
#include "CMD_GameServer.h"
#include <vector>

//�ṹ��
namespace Game
{
	//��������
#define SUB_GAME_CREATE_NAME	"CreateGameServiceManager"			//��������

	struct IGameServiceManager;
	struct IMainServiceFrame;

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
		SCORE							lCellScore;							//��λ����
		uint16							wRevenueRatio;						//˰�ձ���
		SCORE							lServiceScore;						//�������

		//��������
		SCORE							lMinEnterScore;						//��ͻ���
		SCORE							lMaxEnterScore;						//��߻���

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
		Net::ITimerEngine *				pITimerEngine;						//ʱ������
		IGameServiceManager *			pIGameServiceManager;				//�������
		IMainServiceFrame *				pIMainServiceFrame;					//������
		tagGameServiceOption *			pGameServiceOption;
	};
}

//Ѱ�����
namespace Game
{
	using namespace Comm;
	static GGUID IID_IGameServiceManager = { 0x39876a3c, 0x7f9a, 0x4ef5, { 0xb2, 0x2a, 0x78, 0x3c, 0x4b, 0x97, 0x84, 0x33 } };
	static GGUID IID_ITableFrameSink = { 0x68b59d58, 0x9153, 0x4102, { 0x89, 0xec, 0x39, 0x79, 0x81, 0x53, 0x9b, 0xa7 } };
	static GGUID IID_IRoomUserItem = { 0x3e5dee5f, 0xcf19, 0x4f86, { 0xbb, 0x9a, 0x81, 0x93, 0x3a, 0xe3, 0x72, 0x42 } };
	static GGUID IID_IRoomUserManager = { 0x962b7164, 0xcaf8, 0x4adb, { 0xbb, 0xdb, 0x4, 0x64, 0x9f, 0xfe, 0xd0, 0xdd } };
	static GGUID IID_IRoomUserItemSink = { 0xdfc40ba8, 0x35df, 0x489b, { 0x9b, 0x9f, 0xb7, 0xcf, 0x1c, 0x67, 0xe9, 0x30 } };
	static GGUID IID_IMainServiceFrame = { 0xef49c080, 0xc4e7, 0x4584, { 0xa0, 0x81, 0x9b, 0x45, 0x27, 0x1e, 0x2c, 0x92 } };
	static GGUID IID_ITableFrame = { 0x9c1a419a, 0xf3db, 0x4d3f, { 0xb7, 0xd2, 0x2, 0x5d, 0x49, 0x64, 0x53, 0xb5 } };
	static GGUID IID_ITableUserAction = { 0x12a14343, 0xd6e2, 0x4852, { 0x81, 0xf1, 0xdb, 0x93, 0x32, 0x84, 0xb2, 0xf0 } };

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
	struct IRoomUserItem : public IUnknownEx
	{
		//������Ϣ
	public:
		//�û�����
		virtual uint16 GetBindIndex() = 0;
		//�û���ַ
		virtual uint64 GetClientAddr() = 0;
		//������ʶ
		virtual char* GetMachineID() = 0;

		//��¼��Ϣ
	public:
		//�����ʶ
		virtual uint64 GetDBQuestID() = 0;
		//��¼ʱ��
		virtual uint64 GetLogonTime() = 0;
		//��¼����
		virtual uint64 GetInoutIndex() = 0;

		//�û���Ϣ
	public:
		//�û���Ϣ
		virtual tagUserInfo * GetUserInfo() = 0;

		//������Ϣ
	public:
		//�û��Ա�
		virtual uint8 GetGender() = 0;
		//�û���ʶ
		virtual uint32 GetUserID() = 0;
		//��Ϸ��ʶ
		virtual uint32 GetGameID() = 0;
		//�û��ǳ�
		virtual char* GetNickName() = 0;

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
		//�����
		virtual bool DetachBindStatus() = 0;

		//������Ϣ
	public:
		//�û�����
		virtual SCORE GetUserScore() = 0;

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

		//Ч��ӿ�
	public:
		//�Ա�����
		virtual bool ContrastLogonPass(const char* szPassword) = 0;

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

		//�߼��ӿ�
	public:
		//���ò���
		virtual bool SetUserParameter(uint32 dwClientAddr, uint16 wBindIndex, const char szMachineID[LEN_MACHINE_ID], bool bClientReady) = 0;

		//д�����
		virtual bool WriteUserScore(SCORE & lScore) = 0;
	};

	//���ӽӿ�
	struct ITableFrame : public IUnknownEx
	{
		//���Խӿ�
	public:
		//���Ӻ���
		virtual uint16 GetTableID() = 0;
		//��Ϸ����
		virtual uint16 GetChairCount() = 0;
		//��λ����Ŀ
		virtual uint16 GetNullChairCount() = 0;

		//�û��ӿ�
	public:
		//Ѱ���û�
		virtual IRoomUserItem * SearchUserItem(uint32 dwUserID) = 0;
		//��Ϸ�û�
		virtual IRoomUserItem * GetTableUserItem(uint32 wChairID) = 0;
		//�����û�
		virtual IRoomUserItem * SearchUserItemGameID(uint32 dwGameID) = 0;


		//��Ϣ�ӿ�
	public:
		//��Ϸ״̬
		virtual bool IsGameStarted() = 0;
		//��Ϸ״̬
		virtual bool IsDrawStarted() = 0;
		//��Ϸ״̬
		virtual bool IsTableStarted() = 0;

		virtual void SetGameStarted(bool cbGameStatus) = 0;

		//���ƽӿ�
	public:
		//��ʼ��Ϸ
		virtual bool StartGame() = 0;
		//��ɢ��Ϸ
		virtual bool DismissGame() = 0;
		//������Ϸ
		virtual bool ConcludeGame(uint8 cbGameStatus) = 0;

		//д�ֽӿ�
	public:
		//д�����
		virtual bool WriteUserScore(uint8 wChairID, SCORE & lScore) = 0;
		//д�����
		virtual bool WriteTableScore(SCORE ScoreArray[], uint16 wScoreCount) = 0;

		//���ܽӿ�
	public:
		//���ͳ���
		virtual bool SendGameScene(IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize) = 0;

		//��Ϸ�û�
	public:
		//��������
		virtual bool SendTableData(uint16 wChairID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0, uint16 wMainCmdID = MDM_GF_GAME) = 0;

		//��������
	public:
		//��������
		virtual bool PerformStandUpAction(IRoomUserItem * pIServerUserItem, bool bInitiative = false) = 0;
		//���¶���
		virtual bool PerformSitDownAction(uint16 wChairID, IRoomUserItem * pIServerUserItem, const char* szPassword = nullptr) = 0;

		//ʱ��ӿ�
	public:
		//����ʱ��
		virtual bool SetGameTimer(uint32 dwTimerID, uint32 dwElapse, uint32 dwRepeat) = 0;
		//ɾ��ʱ��
		virtual bool KillGameTimer(uint32 dwTimerID) = 0;

		//״̬�ӿ�
	public:
		//��ȡ״̬
		virtual uint8 GetGameStatus() = 0;
		//����״̬
		virtual void SetGameStatus(uint8 bGameStatus) = 0;


		//���ýӿ�
	public:
		//��ʼģʽ
		virtual uint8 GetStartMode() = 0;
		//��ʼģʽ
		virtual void SetStartMode(uint8 cbStartMode) = 0;

		//״̬�ӿ�
	public:
		//��ȡ����
		virtual tagGameServiceOption* GetGameServiceOption() = 0;
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
		virtual bool OnEventGameConclude(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbReason) = 0;
		//���ͳ���
		virtual bool OnEventSendGameScene(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbGameStatus, bool bSendSecret) = 0;
		//��ע״̬
		virtual bool OnEventGetBetStatus(uint16 wChairID, IRoomUserItem * pIServerUserItem) = 0;
		//��Ϸ��¼
		virtual void OnGetGameRecord(void *GameRecord) = 0;

		//�¼��ӿ�
	public:
		//ʱ���¼�
		virtual bool OnTimerMessage(uint32 dwTimerID) = 0;

		//����ӿ�
	public:
		//��Ϸ��Ϣ
		virtual bool OnGameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem) = 0;
		//�����Ϣ
		virtual bool OnFrameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem) = 0;
	};

	//�û�����
	struct IRoomUserManager : public IUnknownEx
	{
		//���ýӿ�
	public:
		//���ýӿ�
		virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx) = 0;

		//���ҽӿ�
	public:
		//�����û�
		virtual IRoomUserItem * SearchUserItem(uint32 dwUserID) = 0;
		//�����û�
		virtual IRoomUserItem * SearchUserItem(char* pszNickName) = 0;

		//ͳ�ƽӿ�
	public:
		//��������
		virtual uint32 GetAndroidCount() = 0;
		//��������
		virtual uint32 GetUserItemCount() = 0;

		//����ӿ�
	public:
		//ɾ���û�
		virtual bool DeleteUserItem() = 0;
		//ɾ���û�
		virtual bool DeleteUserItem(IRoomUserItem * pIServerUserItem) = 0;
		//�����û�
		virtual bool InsertUserItem(IRoomUserItem * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus &UserInfoPlus) = 0;
	};

	//״̬�ӿ�
	struct IRoomUserItemSink : public IUnknownEx
	{
		//�û�����
		virtual bool OnEventUserItemScore(IRoomUserItem * pIServerUserItem, uint8 cbReason) = 0;
		//�û�״̬
		virtual bool OnEventUserItemStatus(IRoomUserItem * pIServerUserItem, uint16 wOldTableID = INVALID_TABLE, uint16 wOldChairID = INVALID_CHAIR) = 0;
	};

	//������
	struct IMainServiceFrame : public IUnknownEx
	{
		//��Ϣ�ӿ�
	public:
		//������Ϣ
		virtual bool SendRoomMessage(char* lpszMessage, uint16 wType) = 0;
		//��Ϸ��Ϣ
		virtual bool SendGameMessage(char* lpszMessage, uint16 wType) = 0;
		//������Ϣ
		virtual bool SendRoomMessage(IRoomUserItem * pIServerUserItem, const char* lpszMessage, uint16 wType) = 0;
		//��Ϸ��Ϣ
		virtual bool SendGameMessage(IRoomUserItem * pIServerUserItem, const char* lpszMessage, uint16 wType) = 0;
		//������Ϣ
		virtual bool SendRoomMessage(uint32 dwSocketID, const char* lpszMessage, uint16 wType, bool bAndroid) = 0;

		//��������
		virtual bool SendData(uint32 dwSocketID, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize) = 0;
		//��������
		virtual bool SendData(IRoomUserItem * pIServerUserItem, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize) = 0;
		//Ⱥ������
		virtual bool SendDataBatch(uint16 wCmdTable, uint16 wMainCmdID, uint16 wSubCmdID, void * pData, uint16 wDataSize) = 0;
		//��������
		virtual void UnLockScoreLockUser(uint32 dwUserID, uint32 dwInoutIndex, uint32 dwLeaveReason) = 0;
	};

	//�û�����
	struct ITableUserAction : public IUnknownEx
	{
		//�û�����
		virtual bool OnActionUserOffLine(uint16 wChairID, IRoomUserItem * pIServerUserItem) = 0;
		//�û�����
		virtual bool OnActionUserConnect(uint16 wChairID, IRoomUserItem * pIServerUserItem) = 0;
		//�û�����
		virtual bool OnActionUserSitDown(uint16 wChairID, IRoomUserItem * pIServerUserItem, bool bLookonUser) = 0;
		//�û�����
		virtual bool OnActionUserStandUp(uint16 wChairID, IRoomUserItem * pIServerUserItem, bool bLookonUser) = 0;
		//�û�ͬ��
		virtual bool OnActionUserOnReady(uint16 wChairID, IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize) = 0;
		//�û���ע
		virtual bool OnActionUserBet(uint16 wChairID, IRoomUserItem * pIServerUserItem) { return true; }
	};


	DECLARE_MOUDLE_DYNAMIC(GameServiceManager)
}

#endif