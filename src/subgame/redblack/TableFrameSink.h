#ifndef TABLE_FRAME_SINK_H
#define TABLE_FRAME_SINK_H

#include "CMD_RedBlack.h"
#include "GameServerManager.h"

namespace SubGame
{
	class CTableFrameSink : public ITableFrameSink, public ITableUserAction
	{
		//��������
	public:
		//���캯��
		CTableFrameSink();
		//��������
		virtual ~CTableFrameSink();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release();
		//�ӿڲ�ѯ
		virtual void * QueryInterface(GGUID Guid);

		//����ӿ�
	public:
		//��λ�ӿ�
		virtual void RepositionSink();
		//���ýӿ�
		virtual bool Initialization(IUnknownEx * pIUnknownEx);

		//�����¼�
	public:
		//�û�����
		virtual bool OnActionUserOffLine(uint16 wChairID, IRoomUserItem * pIServerUserItem);
		//�û�����
		virtual bool OnActionUserConnect(uint16 wChairID, IRoomUserItem * pIServerUserItem);
		//�û�����
		virtual bool OnActionUserSitDown(uint16 wChairID, IRoomUserItem * pIServerUserItem, bool bLookonUser);
		//�û�����
		virtual bool OnActionUserStandUp(uint16 wChairID, IRoomUserItem * pIServerUserItem, bool bLookonUser);
		//�û�ͬ��
		virtual bool OnActionUserOnReady(uint16 wChairID, IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize);

		//��Ϸ�¼�
	public:
		//��Ϸ��ʼ
		virtual bool OnEventGameStart();
		//��Ϸ����
		virtual bool OnEventGameConclude(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbReason);
		//���ͳ���
		virtual bool OnEventSendGameScene(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbGameStatus, bool bSendSecret);
		//��ע״̬
		virtual bool OnEventGetBetStatus(uint16 wChairID, IRoomUserItem * pIServerUserItem);
		//��Ϸ��¼
		virtual void OnGetGameRecord(void *GameRecord);

		//����ӿ�
	public:
		//��Ϸ��Ϣ
		virtual bool OnGameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem);
		//�����Ϣ
		virtual bool OnFrameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem);

		//�¼��ӿ�
	public:
		//ʱ���¼�
		virtual bool OnTimerMessage(uint32 dwTimerID);

		//��Ϸ�¼�
	protected:
		//�û��б�
		bool OnGetUserListGameID(uint16 wSeatUser[MAX_SEAT_COUNT]);

		//��������
	private:
		//��ȡ����
		void ReadConfigInformation();

		//�������
	protected:
		ITableFrame	*					m_pITableFrame;							//��ܽӿ�
		tagGameServiceOption*			m_pGameServiceOption;
	
		//�û���Ϣ
	protected:
		uint64							m_lUserStartScore[GAME_PLAYER];						//��ʼ����

		bool							m_bUserListWin[GAME_PLAYER][USER_LIST_COUNT];		//��ʤ����
		uint16							m_wUserPlayCount[GAME_PLAYER];						//��Ϸ����
		uint64							m_lUserListScore[GAME_PLAYER][USER_LIST_COUNT];		//��ע��Ŀ

		//��ע��
	protected:
		uint64							m_lAllBet[AREA_MAX];					//����ע��
		uint64							m_lPlayBet[GAME_PLAYER][AREA_MAX];		//�����ע
		uint32							m_nChip[MAX_CHIP_COUNT];				//��������

		//����
	protected:
		uint64							m_lBankerScore;							//ׯ�һ���
		uint64							m_lPlayScore[GAME_PLAYER][AREA_MAX];	//�����Ӯ
		uint64							m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
		uint64							m_lUserRevenue[GAME_PLAYER];			//���˰��

		//ʱ������
	protected:
		uint8							m_cbFreeTime;							//����ʱ��
		uint8							m_cbBetTime;							//��עʱ��
		uint8							m_cbEndTime;							//����ʱ��

		//�˿���Ϣ
	protected:
		uint8							m_cbCardCount[CARD_COUNT];					//�˿���Ŀ
		uint8							m_cbTableCardArray[CARD_COUNT][AREA_MAX];	//�����˿�
		uint8							m_cbOpenResult[AREA_MAX + 1];				//�������

		//״̬����
	protected:
		uint32							m_dwBetTime;							//��ʼʱ��

		//ׯ����Ϣ
	protected:
		std::vector<uint16>				m_ApplyUserArray;						//�������
		uint16							m_wCurrentBanker;						//��ǰׯ��
		uint16							m_wOfflineBanker;						//����ׯ��
		uint16							m_wBankerTime;							//��ׯ����

		uint64							m_lBankerWinScore;						//�ۼƳɼ�
		uint64							m_lBankerCurGameScore;					//��ǰ�ɼ�

		bool							m_bEnableSysBanker;						//ϵͳ��ׯ

		//ׯ������
protected:
		uint64							m_nBankerTimeLimit;						//���ׯ����
		uint64							m_nBankerTimeAdd;						//ׯ��������
		uint64							m_lExtraBankerScore;					//ׯ��Ǯ
		uint64							m_nExtraBankerTime;						//ׯ��Ǯ��ʱ,��ׯ������

		uint64							m_lPlayerBankerMAX;						//������ׯ����
		bool							m_bExchangeBanker;						//����ׯ��

		//���Ʊ���
protected:
		uint64							m_lAreaLimitScore;						//��������
		uint64							m_lUserLimitScore;						//��������
		uint64							m_lApplyBankerCondition;				//��������

		//��¼����
	protected:
		tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
		uint32							m_dwRecordFirst;						//��ʼ��¼
		uint32							m_dwRecordLast;							//����¼
		uint32							m_dwRecordCount;						//��¼��Ŀ
	};
}


#endif