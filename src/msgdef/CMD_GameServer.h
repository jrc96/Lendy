#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#include "Define.h"

#pragma pack(1)

using namespace Comm;


//��ʼģʽ
enum StartGameMode
{
	START_MODE_ALL_READY			= 0x00,								//����׼��
	START_MODE_FULL_READY			= 0x01,								//���˿�ʼ
	START_MODE_PAIR_READY			= 0x02,								//��Կ�ʼ
	START_MODE_TIME_CONTROL			= 0x10,								//ʱ�����
	START_MODE_MASTER_CONTROL		= 0x11								//�������
};

//����ԭ��
enum GameEndResult
{
	GER_NORMAL						= 0x00,								//�������
	GER_DISMISS						= 0x01,								//��Ϸ��ɢ
	GER_USER_LEAVE					= 0x02,								//�û��뿪
	GER_NETWORK_ERROR				= 0x03,								//�������
};

//��Ϸ״̬
enum GameSceneStatus
{
	GAME_STATUS_FREE				= 0	,								//����״̬
	GAME_STATUS_PLAY				= 100,								//��Ϸ״̬
	GAME_STATUS_WAIT				= 200,								//�ȴ�״̬
};

#define MDM_CM_SYSTEM				1000								//ϵͳ����

#define SUB_CM_SYSTEM_MESSAGE		1									//ϵͳ��Ϣ
#define SUB_CM_ACTION_MESSAGE		2									//������Ϣ
#define SUB_CM_DOWN_LOAD_MODULE		3									//������Ϣ

//ϵͳ��Ϣ
struct CMD_CM_SystemMessage
{
	uint16							wType;								//��Ϣ����
	uint16							wLength;							//��Ϣ����
	wchar							szString[1024];						//��Ϣ����
};

//////////////////////////////////////////////////////////////////////////////////
//��¼����

#define MDM_GR_LOGON				1									//��¼��Ϣ

//��¼ģʽ
#define SUB_GR_LOGON_USERID			1									//I D ��¼
#define SUB_GR_LOGON_MOBILE			2									//�ֻ���¼

//��¼���
#define SUB_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GR_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GR_LOGON_FINISH			102									//��¼���

//�ֻ���¼
struct CMD_GR_LogonMobile
{
	//�汾��Ϣ
	uint16							wGameID;							//��Ϸ��ʶ
	uint32							dwProcessVersion;					//���̰汾

	//��������
	uint8                           cbDeviceType;                       //�豸����
	uint16                          wBehaviorFlags;                     //��Ϊ��ʶ
	uint16                          wPageTableCount;                    //��ҳ����

	//��¼��Ϣ
	uint32							dwUserID;							//�û� I D
	wchar							szPassword[LEN_MD5];				//��¼����
	wchar							szServerPasswd[LEN_PASSWORD];       //��������
	wchar							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��¼�ɹ�
struct CMD_GR_LogonSuccess
{
	uint32							dwUserRight;						//�û�Ȩ��
	uint32							dwMasterRight;						//����Ȩ��
};

//��¼ʧ��
struct CMD_GR_LogonFailure
{
	uint32							lResultCode;						//�������
	wchar							szDescribeString[128];				//������Ϣ

	uint32							dwLockKindID;						//��ס�������ϷKindID
	uint32							dwLockServerID;						//��ס����ķ���ServerID
};


//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_CONFIG				2									//������Ϣ

#define SUB_GR_CONFIG_SERVER		101									//��������
#define SUB_GR_CONFIG_FINISH		103									//�������

//////////////////////////////////////////////////////////////////////////////////
//��������
struct CMD_GR_ConfigServer
{
	//��������
	uint16							wTableCount;						//������Ŀ
	uint16							wChairCount;						//������Ŀ

	//��������
	uint16							wServerType;						//��������
	uint32							dwServerRule;						//�������
};

//////////////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_GR_USER					3									//�û���Ϣ

#define SUB_GR_USER_SITDOWN			3									//��������
#define SUB_GR_USER_STANDUP			4									//��������
#define SUB_GR_USER_CHAIR_INFO_REQ  11                                  //���������û���Ϣ

//�û�״̬
#define SUB_GR_USER_ENTER			100									//�û�����
#define SUB_GR_USER_SCORE			101									//�û�����
#define SUB_GR_USER_STATUS			102									//�û�״̬
#define SUB_GR_USER_REQUEST_FAILURE	103									//����ʧ��
#define SUB_GR_USER_HALL_SCORE		105									//���˳���Ϸ���û�ˢ�½��

//�û�״̬
struct tagUserStatus
{
	uint16							wTableID;							//��������
	uint16							wChairID;							//����λ��
	uint8							cbUserStatus;						//�û�״̬
};

//�û�����
struct tagMobileUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����

	//��Ӯ��Ϣ
	uint32							dwWinCount;							//ʤ������
	uint32							dwLostCount;						//ʧ������
	uint32							dwDrawCount;						//�;�����
	uint32							dwFleeCount;						//��������
};

//�û���Ϣ
struct CMD_GR_UserInfoHead
{
	//�û�����
	uint32							dwGameID;							//��Ϸ I D
	uint32							dwUserID;							//�û� I D
	uint32							dwGroupID;							//���� I D

	//ͷ����Ϣ
	uint16							wFaceID;							//ͷ������
	uint32							dwCustomID;							//�Զ���ʶ

	wchar							szNickName[LEN_NICKNAME];			//����ǳ�

	//�û�����
	bool							bIsAndroid;							//������ʶ
	uint8							cbGender;							//�û��Ա�
	uint8							cbMemberOrder;						//��Ա�ȼ�
	uint8							cbMasterOrder;						//����ȼ�

	//�û�״̬
	uint16							wTableID;							//��������
	uint16							wChairID;							//��������
	uint8							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lIngot;								//�û�Ԫ��
	SCORE							dBeans;								//�û���Ϸ��
	bool							bAndroid;							//�Ƿ�Ϊ������
	
																		//��Ϸ��Ϣ
	uint32							dwWinCount;							//ʤ������
	uint32							dwLostCount;						//ʧ������
	uint32							dwDrawCount;						//�;�����
	uint32							dwFleeCount;						//��������
	uint32							dwExperience;						//�û�����
	uint32							lLoveLiness;						//�û�����
	SCORE							lIntegralCount;						//��������(��ǰ����)

	//������Ϣ
	uint32							dwAgentID;							//���� I D
};

//�û���Ϣ
struct CMD_GR_MobileUserInfoHead
{
	//�û�����
	uint32							dwGameID;							//��Ϸ I D
	uint32							dwUserID;							//�û� I D

	//ͷ����Ϣ
	uint16							wFaceID;							//ͷ������
	uint32							dwCustomID;							//�Զ���ʶ

	wchar							szNickName[LEN_NICKNAME];			//����ǳ�

	//�û�����
	uint8							cbGender;							//�û��Ա�
	uint8							cbMemberOrder;						//��Ա�ȼ�

	//�û�״̬
	uint16							wTableID;							//��������
	uint16							wChairID;							//��������
	uint8							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����

	//��Ϸ��Ϣ
	uint32							dwWinCount;							//ʤ������
	uint32							dwLostCount;						//ʧ������
	uint32							dwDrawCount;						//�;�����
	uint32							dwFleeCount;						//��������
};


//��������
struct CMD_GR_UserSitDown
{
	uint16							wTableID;							//����λ��
	uint16							wChairID;							//����λ��
	wchar							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct CMD_GR_UserStandUp
{
	uint16							wTableID;							//����λ��
	uint16							wChairID;							//����λ��
	uint8							cbForceLeave;						//ǿ���뿪
};

//�û�״̬
struct CMD_GR_UserStatus
{
	uint32							dwUserID;							//�û���ʶ
	tagUserStatus					UserStatus;							//�û�״̬
};

//����ʧ��
struct CMD_GR_UserRequestFailure
{
	uint32							lErrorCode;							//�������
	wchar							szDescribeString[256];				//������Ϣ
};


//�û�����
struct CMD_GR_MobileUserScore
{
	uint32							dwUserID;							//�û���ʶ
	tagMobileUserScore				UserScore;							//������Ϣ
};

//�˳���Ϸ�û�����
struct CMD_GR_UserHallScore
{
	uint32							dwUserID;							//�û���ʶ
	SCORE							lUserScore;							//������Ϣ
};

//�����û���Ϣ
struct CMD_GR_ChairUserInfoReq
{
	uint16							wTableID;							//���Ӻ���
	uint16							wChairID;							//����λ��
};

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define MDM_GR_STATUS				4									//״̬��Ϣ

#define SUB_GR_TABLE_INFO			100									//������Ϣ
#define SUB_GR_TABLE_STATUS			101									//����״̬

//////////////////////////////////////////////////////////////////////////////////
//����״̬
struct tagTableStatus
{
	uint8							cbTableLock;						//������־
	uint8							cbPlayStatus;						//��Ϸ��־
	uint32							lCellScore;							//��Ԫ����
};

//������Ϣ
struct CMD_GR_TableInfo
{
	uint16							wTableCount;						//������Ŀ
	tagTableStatus					TableStatusArray[512];				//����״̬
};

//����״̬
struct CMD_GR_TableStatus
{
	uint16							wTableID;							//���Ӻ���
	tagTableStatus					TableStatus;						//����״̬
};


//////////////////////////////////////////////////////////////////////////////////
//�������
#define MDM_GF_FRAME				100									//�������

#define SUB_GF_GAME_OPTION			1									//��Ϸ����
#define SUB_GF_GAME_STATUS			100									//��Ϸ״̬
#define SUB_GF_GAME_SCENE			101									//��Ϸ����
#define SUB_GF_USER_DATA			103									//�������

#define SUB_GF_BJL_GAME_STATUS		2001								//����״̬
#define SUB_GF_BJL_GAME_RESULT		2002								//�ټ���֪ͨ������ҵ�����Ϸ���
#define SUB_GF_BJL_CHANGE_STATUS	2003								//�ټ���֪ͨ���������Ϸ״̬�ı�
#define SUB_GF_BAIREN_STATUS_END	2004								//�����ȡ�б����

//��Ϸ����
struct CMD_GF_GameOption
{
	uint8							cbAllowLookon;						//�Թ۱�־
	uint32							dwFrameVersion;						//��ܰ汾
	uint32							dwClientVersion;					//��Ϸ�汾
};

//��Ϸ����
struct CMD_GF_GameStatus
{
	uint8							cbGameStatus;						//��Ϸ״̬
	uint8							cbAllowLookon;						//�Թ۱�־
};

//��Ϸ����
struct CMD_GF_GameUserData
{
	uint32							cbUserCharID;						//��Ϸ״̬
};

//��Ϸ״̬�л�
struct tagChangeStatus
{
	uint16							wTableID;							//����ID
	uint8							cbGameStatus;						//��Ϸ״̬
};

//���ʱ����Ϣ
struct tagRBTimeInfo
{
	uint8							cbBetTime;							//��עʱ��
	uint8							cbEndTime;							//����ʱ��
	uint8							cbPassTime;							//�ѹ�ʱ��

	SCORE							lMinXianHong;						//��С�޺�
	SCORE							lMaxXianHong;						//����޺�
};

//��ڼ�¼��Ϣ
struct tagRBGameRecord
{
	uint8							cbAreaWin[4];							//����ʤ����ʶ
};

//��ڷ���״̬
struct CMD_GF_RBRoomStatus
{
	tagChangeStatus					tagGameInfo;						//������Ϣ
	tagRBTimeInfo					tagTimeInfo;						//ʱ����Ϣ
	tagRBGameRecord					GameRecordArrary[48];				//·����¼
	uint8							cbRecordCount;						//��¼����
};


//////////////////////////////////////////////////////////////////////////////////
//��Ϸ����

#define MDM_GF_GAME					200									//��Ϸ����

#pragma pack()

#endif