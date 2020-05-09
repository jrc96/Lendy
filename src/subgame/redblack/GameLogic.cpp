#include "GameLogic.h"
#include <string>
#include <cassert>
#include <cstring>

namespace SubGame
{
	//�˿�����
	uint8 CGameLogic::m_cbCardListData[52] =
	{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K
	};

	CGameLogic::CGameLogic()
	{
	}

	CGameLogic::~CGameLogic()
	{
	}

	uint8 CGameLogic::GetCardType(uint8 cbCardData[], uint8 cbCardCount)
	{
		assert(cbCardCount == MAX_COUNT);

		uint8 cbTempCardData[MAX_COUNT] = { 0 };
		memcpy(cbTempCardData, cbCardData, sizeof(uint8) * MAX_COUNT);

		SortCardList(cbTempCardData, MAX_COUNT);

		if (cbCardCount == MAX_COUNT)
		{
			//��������
			bool cbSameColor = true, bLineCard = true;
			uint8 cbFirstColor = GetCardColor(cbTempCardData[0]);
			uint8 cbFirstValue = GetCardLogicValue(cbTempCardData[0]);

			//���η���
			for (uint8 i = 1; i < cbCardCount; i++)
			{
				//���ݷ���
				if (GetCardColor(cbTempCardData[i]) != cbFirstColor) cbSameColor = false;
				if (cbFirstValue != (GetCardLogicValue(cbTempCardData[i]) + i)) bLineCard = false;

				//�����ж�
				if ((cbSameColor == false) && (bLineCard == false)) break;
			}

			//����A32
			if (!bLineCard)
			{
				bool bOne = false, bTwo = false, bThree = false;
				for (uint8 i = 0; i < MAX_COUNT; i++)
				{
					if (GetCardValue(cbTempCardData[i]) == 1)		bOne = true;
					else if (GetCardValue(cbTempCardData[i]) == 2)	bTwo = true;
					else if (GetCardValue(cbTempCardData[i]) == 3)	bThree = true;
				}
				if (bOne && bTwo && bThree)bLineCard = true;
			}

			//˳������
			if ((cbSameColor) && (bLineCard)) return CT_SHUN_JIN;

			//˳������
			if ((!cbSameColor) && (bLineCard)) return CT_SHUN_ZI;

			//������
			if ((cbSameColor) && (!bLineCard)) return CT_JIN_HUA;

			//���η���
			bool bDouble = false, bDoubleShine = false, bPanther = true;

			//���Ʒ���
			for (uint8 i = 0; i < cbCardCount - 1; i++)
			{
				for (uint8 j = i + 1; j < cbCardCount; j++)
				{
					if (GetCardLogicValue(cbTempCardData[i]) == GetCardLogicValue(cbTempCardData[j]))
					{
						bDouble = true;
						if (GetCardLogicValue(cbTempCardData[i]) > 8)	bDoubleShine = true;
						break;
					}
				}
				if (bDouble)break;
			}

			//����(����)����
			for (uint8 i = 1; i < cbCardCount; i++)
			{
				if (bPanther && cbFirstValue != GetCardLogicValue(cbTempCardData[i])) bPanther = false;
			}

			//���Ӻͱ����ж�
			if (bDouble == true)
			{
				if (bPanther == true)
				{
					return CT_BAO_ZI;
				}
				else
				{
					if (bDoubleShine == true)
					{
						return CT_DOUBLE_SHINE;
					}
					return CT_DOUBLE;
				}
			}
		}

		return CT_SINGLE;
	}

	void CGameLogic::SortCardList(uint8 cbCardData[], uint8 cbCardCount)
	{
		//ת����ֵ
		uint8 cbLogicValue[MAX_COUNT] = {};
		for (uint8 i = 0; i < cbCardCount; i++) cbLogicValue[i] = GetCardLogicValue(cbCardData[i]);

		//�������
		bool bSorted = true;
		uint8 cbTempData, bLast = cbCardCount - 1;
		do
		{
			bSorted = true;
			for (uint8 i = 0; i < bLast; i++)
			{
				if ((cbLogicValue[i] < cbLogicValue[i + 1]) ||
					((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
				{
					//����λ��
					cbTempData = cbCardData[i];
					cbCardData[i] = cbCardData[i + 1];
					cbCardData[i + 1] = cbTempData;
					cbTempData = cbLogicValue[i];
					cbLogicValue[i] = cbLogicValue[i + 1];
					cbLogicValue[i + 1] = cbTempData;
					bSorted = false;
				}
			}
			bLast--;
		} while (bSorted == false);

		return;
	}

	void CGameLogic::RandCardList(uint8 cbCardBuffer[], uint8 cbBufferCount)
	{
		//����׼��
		uint8 cbCardData[ARR_LEN(m_cbCardListData)];
		memcpy(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

		//�����˿�
		uint8 bRandCount = 0, bPosition = 0;
		do
		{
			bPosition = rand() % (ARR_LEN(m_cbCardListData) - bRandCount);
			cbCardBuffer[bRandCount++] = cbCardData[bPosition];
			cbCardData[bPosition] = cbCardData[ARR_LEN(m_cbCardListData) - bRandCount];
		} while (bRandCount < cbBufferCount);

		return;
	}

	uint8 CGameLogic::GetCardLogicValue(uint8 cbCardData)
	{
		//�˿�����
		uint8 bCardColor = GetCardColor(cbCardData);
		uint8 bCardValue = GetCardValue(cbCardData);

		//ת����ֵ
		return (bCardValue == 1) ? (bCardValue + 13) : bCardValue;
	}

	uint8 CGameLogic::CompareCard(uint8 cbFirstData[], uint8 cbNextData[], uint8 cbCardCount)
	{
		//���ñ���
		uint8 FirstData[MAX_COUNT], NextData[MAX_COUNT];
		memcpy(FirstData, cbFirstData, sizeof(FirstData));
		memcpy(NextData, cbNextData, sizeof(NextData));

		//��С����
		SortCardList(FirstData, cbCardCount);
		SortCardList(NextData, cbCardCount);

		//��ȡ����
		uint8 cbNextType = GetCardType(NextData, cbCardCount);
		uint8 cbFirstType = GetCardType(FirstData, cbCardCount);

		//�����������
		if ((cbNextType + cbFirstType) == (CT_SPECIAL + CT_BAO_ZI))return (uint8)(cbFirstType > cbNextType);

		//��ԭ��������
		if (cbNextType == CT_SPECIAL)cbNextType = CT_SINGLE;
		if (cbFirstType == CT_SPECIAL)cbFirstType = CT_SINGLE;

		//�����ж�
		if (cbFirstType != cbNextType) return (cbFirstType > cbNextType) ? 1 : 0;

		//������
		switch (cbFirstType)
		{
			case CT_BAO_ZI:			//����
			case CT_SINGLE:			//����
			case CT_JIN_HUA:		//��
			{
				//�Ա���ֵ
				for (uint8 i = 0; i < cbCardCount; i++)
				{
					uint8 cbNextValue = GetCardLogicValue(NextData[i]);
					uint8 cbFirstValue = GetCardLogicValue(FirstData[i]);
					if (cbFirstValue != cbNextValue) return (cbFirstValue > cbNextValue) ? 1 : 0;
				}
				return DRAW;
			}
			case CT_SHUN_ZI:		//˳��
			case CT_SHUN_JIN:		//˳�� 432>A32
			{
				uint8 cbNextValue = GetCardLogicValue(NextData[0]);
				uint8 cbFirstValue = GetCardLogicValue(FirstData[0]);

				//����A32
				if (cbNextValue == 14 && GetCardLogicValue(NextData[cbCardCount - 1]) == 2)
				{
					cbNextValue = 3;
				}
				if (cbFirstValue == 14 && GetCardLogicValue(FirstData[cbCardCount - 1]) == 2)
				{
					cbFirstValue = 3;
				}

				//�Ա���ֵ
				if (cbFirstValue != cbNextValue) return (cbFirstValue > cbNextValue) ? 1 : 0;;
				return DRAW;
			}
			case CT_DOUBLE_SHINE:
			case CT_DOUBLE:			//����
			{
				uint8 cbNextValue = GetCardLogicValue(NextData[0]);
				uint8 cbFirstValue = GetCardLogicValue(FirstData[0]);

				//���Ҷ���/����
				uint8 bNextDouble = 0, bNextSingle = 0;
				uint8 bFirstDouble = 0, bFirstSingle = 0;
				if (cbNextValue == GetCardLogicValue(NextData[1]))
				{
					bNextDouble = cbNextValue;
					bNextSingle = GetCardLogicValue(NextData[cbCardCount - 1]);
				}
				else
				{
					bNextDouble = GetCardLogicValue(NextData[cbCardCount - 1]);
					bNextSingle = cbNextValue;
				}
				if (cbFirstValue == GetCardLogicValue(FirstData[1]))
				{
					bFirstDouble = cbFirstValue;
					bFirstSingle = GetCardLogicValue(FirstData[cbCardCount - 1]);
				}
				else
				{
					bFirstDouble = GetCardLogicValue(FirstData[cbCardCount - 1]);
					bFirstSingle = cbFirstValue;
				}

				if (bNextDouble != bFirstDouble)return (bFirstDouble > bNextDouble) ? 1 : 0;
				if (bNextSingle != bFirstSingle)return (bFirstSingle > bNextSingle) ? 1 : 0;
				return DRAW;
			}
		}

		return DRAW;
	}
}