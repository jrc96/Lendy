#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "Define.h"

namespace  SubGame
{
#define MAX_COUNT					3									//�����Ŀ
#define	DRAW						2									//�;�����

	//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿�����
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_DOUBLE_SHINE				3									//�������
#define	CT_SHUN_ZI					4									//˳������
#define CT_JIN_HUA					5									//������
#define	CT_SHUN_JIN					6									//˳������
#define	CT_BAO_ZI					7									//��������
#define CT_SPECIAL					8									//��������

	//��Ϸ�߼���
	class CGameLogic
	{
		//��������
	private:
		static uint8						m_cbCardListData[52];				//�˿˶���

		//��������
	public:
		//���캯��
		CGameLogic();
		//��������
		virtual ~CGameLogic();

		//���ͺ���
	public:
		//��ȡ����
		uint8 GetCardType(uint8 cbCardData[], uint8 cbCardCount);
		//��ȡ��ֵ
		uint8 GetCardValue(uint8 cbCardData) { return cbCardData & LOGIC_MASK_VALUE; }
		//��ȡ��ɫ
		uint8 GetCardColor(uint8 cbCardData) { return cbCardData & LOGIC_MASK_COLOR; }

		//���ƺ���
	public:
		//�����˿�
		void SortCardList(uint8 cbCardData[], uint8 cbCardCount);
		//�����˿�
		void RandCardList(uint8 cbCardBuffer[], uint8 cbBufferCount);

		//���ܺ���
	public:
		//�߼���ֵ
		uint8 GetCardLogicValue(uint8 cbCardData);
		//�Ա��˿�
		uint8 CompareCard(uint8 cbFirstData[], uint8 cbNextData[], uint8 cbCardCount);
	};
}

#endif