#ifndef TABLE_FRAME_H
#define TABLE_FRAME_H

#include "GameComponent.h"

namespace Game
{
	class CTableFrame : public ITableFrame
	{
		//���鶨��
		typedef IRoomUserItem * CTableUserItemArray[MAX_CHAIR];				//��Ϸ����

		//��������
	public:
		//���캯��
		CTableFrame();
		//��������
		virtual ~CTableFrame();

		virtual void Release() { delete this; }
		virtual void *QueryInterface(GGUID uuid);

		//���Խӿ�
	public:
		//���Ӻ���
		virtual uint16 GetTableID();
		//��Ϸ����
		virtual uint16 GetChairCount();
		//��λ����Ŀ
		virtual uint16 GetNullChairCount();

		//�û��ӿ�
	public:
		//Ѱ���û�
		virtual IRoomUserItem * SearchUserItem(uint32 dwUserID);
		//��Ϸ�û�
		virtual IRoomUserItem * GetTableUserItem(uint32 wChairID);
		//�����û�
		virtual IRoomUserItem * SearchUserItemGameID(uint32 dwGameID);

		//��Ϣ�ӿ�
	public:
		//��Ϸ״̬
		virtual bool IsGameStarted();
		//��Ϸ״̬
		virtual bool IsDrawStarted();
		//��Ϸ״̬
		virtual bool IsTableStarted();

		virtual void SetGameStarted(bool cbGameStatus);

		//���ƽӿ�
	public:
		//��ʼ��Ϸ
		virtual bool StartGame();
		//��ɢ��Ϸ
		virtual bool DismissGame();
		//������Ϸ
		virtual bool ConcludeGame(uint8 cbGameStatus) ;
		//��������
		virtual bool ConcludeTable();

		//д�ֽӿ�
	public:
		//д�����
		virtual bool WriteUserScore(uint8 wChairID, SCORE & lScore);
		//д�����
		virtual bool WriteTableScore(SCORE ScoreArray[], uint16 wScoreCount);

		//���ܽӿ�
	public:
		//���ͳ���
		virtual bool SendGameScene(IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize);

		//ʱ��ӿ�
	public:
		//����ʱ��
		virtual bool SetGameTimer(uint32 dwTimerID, uint32 dwElapse, uint32 dwRepeat);
		//ɾ��ʱ��
		virtual bool KillGameTimer(uint32 dwTimerID);

		//״̬�ӿ�
	public:
		//��ȡ״̬
		virtual uint8 GetGameStatus();
		//����״̬
		virtual void SetGameStatus(uint8 bGameStatus);

		//��Ϸ�û�
	public:
		//��������
		virtual bool SendTableData(uint16 wChairID, uint16 wSubCmdID, void * pData = nullptr, uint16 wDataSize = 0, uint16 wMainCmdID = MDM_GF_GAME);

		//��������
	public:
		//��������
		virtual bool PerformStandUpAction(IRoomUserItem * pIServerUserItem, bool bInitiative = false);
		//���¶���
		virtual bool PerformSitDownAction(uint16 wChairID, IRoomUserItem * pIServerUserItem, const char* szPassword = nullptr);

		//���ýӿ�
	public:
		//��ʼģʽ
		virtual uint8 GetStartMode() { return m_cbStartMode; }
		//��ʼģʽ
		virtual void SetStartMode(uint8 cbStartMode) { m_cbStartMode = cbStartMode; }

		//״̬�ӿ�
	public:
		//��ȡ����
		virtual tagGameServiceOption* GetGameServiceOption();

		//�û��¼�
	public:
		//�����¼�
		bool OnEventUserOffLine(IRoomUserItem * pIServerUserItem);

		//ϵͳ�¼�
	public:
		//ʱ���¼�
		bool OnEventTimer(uint32 dwTimerID);
		//��Ϸ�¼�
		bool OnEventSocketGame(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem);
		//����¼�
		bool OnEventSocketFrame(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem);

		//Ч�麯��
	public:
		//��ʼЧ��
		bool EfficacyStartGame(uint16 wReadyChairID);

	public:
		//��������
		bool InitializationFrame(uint16 wTableID, tagTableFrameParameter & TableFrameParameter);

		//���ܺ���
	public:
		//��Ϸ��¼
		void OnGetGameRecord(void *GameRecord);

		//���ܺ���
	public:
		//��ȡ��λ
		uint16 GetNullChairID();
		//�û���Ŀ
		uint16 GetSitUserCount();

		//��������
	public:
		//����״̬
		bool SendTableStatus();
	
		//��Ϸ����
	protected:
		uint8							m_cbStartMode;
		uint8							m_cbGameStatus;
		uint16							m_wTableID;							//���Ӻ���
		uint16							m_wChairCount;						//������Ŀ
		uint16							m_wUserCount;						//�û���Ŀ
		uint16							m_wPlayCount;						//��Ϸ����

		//״̬����
	protected:
		bool							m_bGameStarted;						//��Ϸ��־
		bool							m_bDrawStarted;						//��Ϸ��־
		bool							m_bTableStarted;					//��Ϸ��־
		bool							m_bTableInitFinish;					//��ʼ��ʶ

		uint32							m_dwDrawStartTime;

		//�û�����
	protected:
		CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	
		//���ݽӿ�
	protected:
		Net::ITimerEngine *				m_pITimerEngine;					//ʱ������
		ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�
		IMainServiceFrame *				m_pIMainServiceFrame;				//����ӿ�

		//��չ�ӿ�
	protected:
		ITableUserAction *				m_pITableUserAction;				//�����ӿ�

	//������Ϣ
	protected:
		tagGameServiceOption *			m_pGameServiceOption;					//���ò���
	};
}

#endif