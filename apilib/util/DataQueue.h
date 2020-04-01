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
		//��ȡλ��
	protected:
		//����λ��
		uint32							m_dwInsertIndex;
		//��ѯλ��
		uint32							m_dwQueryIndex;
		//β��λ��
		uint32							m_dwTailIndex;

		//���ݱ���
	protected:
		uint32							m_dwDataSize;						//���ݴ�С
		uint32							m_dwDataPacketCount;				//���ݰ���
				
		//�������
	protected:
		uint32							m_dwQueueSize;					//���峤��
		uint8*							m_pQueueBuffer;					//����ָ��

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