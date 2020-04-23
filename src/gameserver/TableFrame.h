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
		virtual void *QueryInterface(GGUID uuid) { return nullptr; }

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

		//���ܽӿ�
	public:
		//���ͳ���
		virtual bool SendGameScene(IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize);

		//��������
	public:
		//��������
		virtual bool PerformStandUpAction(IRoomUserItem * pIServerUserItem, bool bInitiative = false);
		//���¶���
		virtual bool PerformSitDownAction(uint16 wChairID, IRoomUserItem * pIServerUserItem, const char* szPassword = nullptr);

		//ϵͳ�¼�
	public:
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
		uint16							m_wTableID;							//���Ӻ���
		uint16							m_wChairCount;						//������Ŀ
		uint16							m_wUserCount;						//�û���Ŀ

		//״̬����
	protected:
		bool							m_bGameStarted;						//��Ϸ��־
		bool							m_bDrawStarted;						//��Ϸ��־
		bool							m_bTableStarted;					//��Ϸ��־
		bool							m_bTableInitFinish;					//��ʼ��ʶ

		//�û�����
	protected:
		CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	
		//���ݽӿ�
	protected:
		Net::ITCPSocketService *		m_pITCPSocketService;				//�������
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