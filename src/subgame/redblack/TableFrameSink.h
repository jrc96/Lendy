#ifndef TABLE_FRAME_SINK_H
#define TABLE_FRAME_SINK_H

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


		//�������
	protected:
		ITableFrame	*					m_pITableFrame;							//��ܽӿ�
	};
}


#endif