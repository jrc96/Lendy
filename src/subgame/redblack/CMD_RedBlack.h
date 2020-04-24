#ifndef CMD_RED_BLACK_H
#define CMD_RED_BLACK_H

#include "Define.h"
#include "CMD_GameServer.h"

#pragma pack(1)

namespace SubGame
{
	enum RedBlack
	{
		CARD_COUNT = 2,
		MAX_CHIP_COUNT = 5,
		USER_LIST_COUNT = 20,
		MAX_SCORE_HISTORY = 72,

		MAX_SEAT_COUNT = 6,

		AREA_XIAN			=		0,									//��
		AREA_ZHUANG			=		1,									//��
		AREA_PING			=		2,									//����һ��
		AREA_MAX			=		3,									//�������

		TIME_FREE = 1,
		TIME_PLACE_JETTON = 5,
		TIME_GAME_END = 8,

		GAME_PLAYER			=		100									//��Ϸ����
	};

	enum  GameSubMsgID
	{
		SUB_S_GAME_START		=	2003,									//��Ϸ��ʼ
		SUB_S_GAME_END			=	2006,									//��Ϸ����
	};

	enum GameStatus
	{
		GAME_SCENE_FREE			=	GAME_STATUS_FREE,					//�ȴ���ʼ
		GAME_SCENE_BET			=	GAME_STATUS_FREE,					//��ע״̬
		GAME_SCENE_END			=	GAME_STATUS_PLAY,					//����״̬
	};

	enum SubGameTimerID
	{
		//��עʱ��
		IDI_PLACE_JETTON		=	2,									//��עʱ��
	};
	
	struct tagServerGameRecord
	{
		bool							bPlayer;
		bool							bBanker;
		uint8							cbPing;
		uint8							cbCardType;  //1���ţ�2����,3˳��,4��,5˳��,6����
	};

	//��Ϸ״̬
	struct CMD_S_StatusPlay
	{
		//ȫ����Ϣ
		uint8							cbTimeLeave;						//ʣ��ʱ��

		int								nChip[MAX_CHIP_COUNT];				//��������

		uint16							wApplyUser[GAME_PLAYER];			//��ׯ�б���ҵ�GameID

		uint16							wSeatUser[MAX_SEAT_COUNT];	//6��������ҵ����Ӻ�

		uint64							lSeatUserAreaScore[MAX_SEAT_COUNT][AREA_MAX];	//6��������ҵ�������ע��Ϣ

		uint16							wBankerUser;						//��ǰׯ�ҵ�GameID
		uint64							lBankerScore;						//ׯ�ҷ���
		uint64							lAreaLimitScore;					//��������
		uint64							lApplyBankerCondition;				//��������

		uint8							cbFreeTime;							//����ʱ��
		uint8							cbPlayTime;							//��Ϸʱ��
		uint8							cbEndTime;							//����ʱ��

		uint64							lMinXianHong;
		uint64							lMaxXianHong;

		uint64							lPlayerJettonScore[AREA_MAX];		//����µ�ע
		uint64							lAllJettonScore[AREA_MAX];			//��������µ�ע

		uint8							cbTableCardArray[CARD_COUNT][AREA_MAX];		//�����˿�

		uint8							cbResult[AREA_MAX + 1];					//���  0:�� 1�� 2��

		uint64							lBankerWinScore;					//ׯ��Ӯ��
		uint64							lPlayerWinScore;					//���Ӯ��
		uint64							lSeatUserWinScore[MAX_SEAT_COUNT];	//���µ������Ӯ
		uint64							lPlayerRestScore;					//��һ�ʣ����Ǯ
		uint64							lBankerRestScore;					//ׯ�һ�ʣ����Ǯ
		uint64							SeatPlayerRestScore[MAX_SEAT_COUNT];
	};


	//��Ϸ��ʼ
	struct CMD_S_GameStart
	{
		uint8							cbBetTime;							//ʣ��ʱ��
		uint16							wSeatUser[MAX_SEAT_COUNT];			
	};


	//��Ϸ����
	struct CMD_S_GameEnd
	{
		uint64							lPlayAreaScore[AREA_MAX];			//��ҳɼ�
		uint64							lPlayerWinScore;					//���Ӯ��Ǯ
		uint64							lPlayerRestScore;					//��һ�ʣ����Ǯ
		uint64							lBankerWinScore;					//ׯ�ҳɼ�
		uint64							lBankerRestScore;					//ׯ�һ�ʣ����Ǯ

		uint16							wSeatUser[MAX_SEAT_COUNT];			//3��������ҵ����Ӻ�
		uint64							lSeatUserWinScore[MAX_SEAT_COUNT];	//���µ������Ӯ
		uint64							lPlayerJettonScore[AREA_MAX];		//����µ�ע
		uint64							lAllJettonScore[AREA_MAX];			//��������µ�ע
		uint64							lSeatUserRestScore[MAX_SEAT_COUNT];	//���µ���һ�ʣ����Ǯ

		uint8							cbTableCardArray[CARD_COUNT][AREA_MAX];				//�����˿�
		uint8							cbResult[AREA_MAX + 1];				//���
	};


	//����б�������
	struct CMD_S_UserListInfo
	{
		uint16							wWinNum;							//��ʤ����
		uint64							lAllBet;							//��ע����
		uint64							lUserScore;							//�û�����
		wchar_t							szNickName[32];						//�û��ǳ�
		uint8							wFaceID;							//���ͷ��
		uint16							wChairID;
	};

	//////////////////////////////////////////////////////////////////////////
	//�ͻ�������ṹ
#define SUB_C_PLACE_JETTON			100									//�û���ע
}

#pragma pack()

#endif