#ifndef CMD_RED_BLACK_H
#define CMD_RED_BLACK_H

#include "Define.h"
#include "CMD_GameServer.h"

#pragma pack(1)

namespace SubGame
{
	enum RedBlack
	{
		//��������
		INDEX_PLAYER			=	0	,								//�������
		INDEX_BANKER			=	1	,								//�ڼ�����


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
		GAME_SCENE_BET			=	GAME_STATUS_FREE,					//��ע״̬
		GAME_SCENE_BET_FINISH	=	GAME_STATUS_FREE+1,					//��ע״̬
		GAME_SCENE_END			=	GAME_STATUS_PLAY,					//����״̬
	};

	enum SubGameTimerID
	{
		//��עʱ��
		IDI_PLACE_JETTON		=	2,									//��עʱ��

		IDI_GAME_END			=	3,									//����ʱ��
	};
	
	struct tagServerGameRecord
	{
		bool							bPlayer;
		bool							bBanker;
		uint8							cbPing;
		uint8							cbCardType;  //1���ţ�2����,3˳��,4��,5˳��,6����
	};

#define SUB_S_GAME_FREE				2002									//��Ϸ����
#define SUB_S_GAME_START			2003									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			2004									//�û���ע
#define SUB_S_PLACE_JETTON_FAIL		2005									//��עʧ��
#define SUB_S_GAME_END				2006									//��Ϸ����

#define SUB_S_ONLINE_PLAYER			2014									//�����û�
#define SUB_S_SEND_RECORD			2017									//��Ϸ��¼
#define SUB_S_CHEHUI				2019									//��ע����

#define SUB_S_OTHER_JETTON			2021									//���������ע
#define SUB_S_SEAT_JETTON			2022									//ռλ�����ע

	//��Ϸ״̬
	struct CMD_S_StatusPlay
	{
		//ȫ����Ϣ
		uint8							cbTimeLeave;						//ʣ��ʱ��

		int								nChip[MAX_CHIP_COUNT];				//��������

		uint16							wApplyUser[GAME_PLAYER];			//��ׯ�б���ҵ�GameID

		uint16							wSeatUser[MAX_SEAT_COUNT];	//6��������ҵ����Ӻ�

		SCORE							lSeatUserAreaScore[MAX_SEAT_COUNT][AREA_MAX];	//6��������ҵ�������ע��Ϣ

		uint16							wBankerUser;						//��ǰׯ�ҵ�GameID
		SCORE							lBankerScore;						//ׯ�ҷ���
		SCORE							lAreaLimitScore;					//��������
		SCORE							lApplyBankerCondition;				//��������

		uint8							cbFreeTime;							//����ʱ��
		uint8							cbPlayTime;							//��Ϸʱ��
		uint8							cbEndTime;							//����ʱ��

		SCORE							lMinXianHong;
		SCORE							lMaxXianHong;

		SCORE							lPlayerJettonScore[AREA_MAX];		//����µ�ע
		SCORE							lAllJettonScore[AREA_MAX];			//��������µ�ע

		uint8							cbTableCardArray[CARD_COUNT][AREA_MAX];		//�����˿�

		uint8							cbResult[AREA_MAX + 1];					//���  0:�� 1�� 2��

		SCORE							lBankerWinScore;					//ׯ��Ӯ��
		SCORE							lPlayerWinScore;					//���Ӯ��
		SCORE							lSeatUserWinScore[MAX_SEAT_COUNT];	//���µ������Ӯ
		SCORE							lPlayerRestScore;					//��һ�ʣ����Ǯ
		SCORE							lBankerRestScore;					//ׯ�һ�ʣ����Ǯ
		SCORE							SeatPlayerRestScore[MAX_SEAT_COUNT];
	};


	//��Ϸ��ʼ
	struct CMD_S_GameStart
	{
		uint8							cbBetTime;							//ʣ��ʱ��
		uint16							wSeatUser[MAX_SEAT_COUNT];			
	};

	//ʧ�ܽṹ
	struct CMD_S_PlaceBetFail
	{
		wchar							szBuffer[64];						//������Ϣ
	};

	//�û���ע
	struct CMD_S_PlaceBet
	{
		uint16							wChairID;							//�û�λ��
		uint8							cbBetArea;							//��������
		SCORE							lBetScore;							//��ע��Ŀ
		SCORE							lPlayerRestScore;					//��ע���ʣ����
	};

	//��Ϸ����
	struct CMD_S_GameEnd
	{
		SCORE							lPlayAreaScore[3];			//��ҳɼ�
		SCORE							lPlayerWinScore;					//���Ӯ��Ǯ
		SCORE							lPlayerRestScore;					//��һ�ʣ����Ǯ
		SCORE							lBankerWinScore;					//ׯ�ҳɼ�
		SCORE							lBankerRestScore;					//ׯ�һ�ʣ����Ǯ

		uint16							wSeatUser[6];			//3��������ҵ����Ӻ�
		SCORE							lSeatUserWinScore[6];	//���µ������Ӯ
		SCORE							lPlayerJettonScore[3];		//����µ�ע
		SCORE							lAllJettonScore[3];			//��������µ�ע
		SCORE							lSeatUserRestScore[6];	//���µ���һ�ʣ����Ǯ

		uint8							cbTableCardArray[2][3];		//�����˿�
		uint8							cbResult[3 + 1];				//���
	};

	//����б�
	struct CMD_S_UserList
	{
		uint16							wCount;								//��������
		bool							bEnd;								//�Ƿ����
		uint8							cbIndex[USER_LIST_COUNT];			//����
		wchar							szUserNick[USER_LIST_COUNT][32];	//�ǳ�
		SCORE							lBetScore[USER_LIST_COUNT];			//��20����ע���
		uint8							cbWinTimes[USER_LIST_COUNT];		//��20��Ӯ�˶��پ�
		SCORE							lUserScore[USER_LIST_COUNT];		//��ҽ��
		uint8							wFaceID[USER_LIST_COUNT];			//���ͷ��
		uint16							wChairID[USER_LIST_COUNT];
	};

	//����б�������
	struct CMD_S_UserListInfo
	{
		uint16							wWinNum;							//��ʤ����
		SCORE							lAllBet;							//��ע����
		SCORE							lUserScore;							//�û�����
		wchar							szNickName[32];						//�û��ǳ�
		uint8							wFaceID;							//���ͷ��
		uint16							wChairID;
	};

	//�û�����
	struct CMD_S_CheHui
	{
		uint16							wChairID;							//���ص��û�
		SCORE							lUserRestScore;						//�û�ʣ����
		SCORE							SurplusChip[AREA_MAX];				//����ʣ����
	};

	//////////////////////////////////////////////////////////////////////////
	//�ͻ�������ṹ
#define SUB_C_PLACE_JETTON			100									//�û���ע
#define SUB_C_APPLY_BANKER			101									//����ׯ��
#define SUB_C_CANCEL_BANKER			102									//ȡ������
#define SUB_C_ONLINE_PLAYER			103									//�����û�
#define SUB_C_CHEHUI				105									//������ע

	//�û���ע
	struct CMD_C_PlaceBet
	{
		uint8							cbBetArea;							//��������
		SCORE							lBetScore;							//��ע��Ŀ
	};
}

#pragma pack()

#endif