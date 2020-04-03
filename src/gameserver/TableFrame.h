#ifndef TABLE_FRAME_H
#define TABLE_FRAME_H

#include "GameComponent.h"

namespace Game
{
	class CTableFrame
	{
		//��������
	public:
		//���캯��
		CTableFrame();
		//��������
		virtual ~CTableFrame();

	public:
		//��������
		bool InitializationFrame(uint16 wTableID, tagTableFrameParameter & TableFrameParameter);
	
		//��Ϸ����
	protected:
		uint16							m_wTableID;							//���Ӻ���
		uint16							m_wChairCount;						//������Ŀ
	
		//���ݽӿ�
	protected:
		Net::ITCPSocketService *		m_pITCPSocketService;				//�������
		ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�

	//������Ϣ
	protected:
		tagGameServiceOption *			m_pGameServiceOption;					//���ò���
	};
}

#endif