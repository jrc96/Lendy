#ifndef DATA_QUEUE_H
#define DATA_QUEUE_H

#include "Define.h"
#include <deque>

namespace Util
{
	//������ͷ
	struct tagDataHead
	{
		uint16							wDataSize;							//���ݴ�С
		uint16							wIdentifier;						//���ͱ�ʶ
	};

	class LENDY_COMMON_API DataQueue
	{
		//��ѯ����
	protected:
		uint32							m_dwInsertPos;					//����λ��
		uint32							m_dwTerminalPos;				//����λ��
		uint32							m_dwDataQueryPos;				//��ѯλ��

		//���ݱ���
	protected:
		uint32							m_dwDataSize;					//���ݴ�С
		uint32							m_dwDataPacketCount;			//���ݰ���
				
		//�������
	protected:
		uint32							m_dwBufferSize;					//���峤��
		uint8*							m_pDataQueueBuffer;				//����ָ��

		//��������
	public:
		//���캯��
		DataQueue();
		//��������
		virtual ~DataQueue();

		//��������
	public:
		//��������
		bool InsertData(uint16 wIdentifier, void * pBuffer, uint16 wDataSize);

		//��ȡ����
		bool DistillData(tagDataHead & DataHead, void * pBuffer, uint16 wBufferSize);

		//�ڲ�����
	private:
		//�����洢
		bool RectifyBuffer(uint64 dwNeedSize);
	};
}


#endif