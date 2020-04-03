#ifndef TABLE_FRAME_SINK_H
#define TABLE_FRAME_SINK_H

#include "GameServerManager.h"

namespace SubGame
{
	class CTableFrameSink : public ITableFrameSink
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

		//��Ϸ�¼�
	public:
		//��Ϸ��ʼ
		virtual bool OnEventGameStart();
		//��Ϸ����
		virtual bool OnEventGameConclude(uint16 wChairID, IServerUserItem * pIServerUserItem, uint8 cbReason);
		//���ͳ���
		virtual bool OnEventSendGameScene(uint16 wChairID, IServerUserItem * pIServerUserItem, uint8 cbGameStatus, bool bSendSecret);

		//����ӿ�
	public:
		//��Ϸ��Ϣ
		virtual bool OnGameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IServerUserItem * pIServerUserItem);
		//�����Ϣ
		virtual bool OnFrameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IServerUserItem * pIServerUserItem);
	};
}


#endif