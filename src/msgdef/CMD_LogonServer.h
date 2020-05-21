#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#include "Define.h"

#pragma pack(1)

using namespace Comm;

//��¼����
#define MDM_MB_LOGON				100									//�㳡��¼
#define SUB_MB_LOGON_VISITOR		5									//�ο͵�¼


//��¼���
#define SUB_MB_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_MB_LOGON_FAILURE		101									//��¼ʧ��

//�ο͵�¼
struct CMD_MB_LogonVisitor
{
	//ϵͳ��Ϣ
	uint16							wModuleID;							//ģ���ʶ
	uint32							dwPlazaVersion;						//�㳡�汾
	wchar							szAgentID[LEN_ACCOUNTS];			//�����ʶ
	uint8                           cbDeviceType;                       //�豸����

	//������Ϣ
	wchar							szMachineID[LEN_MACHINE_ID];		//������ʶ
	wchar							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//��¼�ɹ�
struct CMD_MB_LogonSuccess
{
	uint16							wFaceID;							//ͷ���ʶ
	uint8							cbGender;							//�û��Ա�
	uint32							dwCustomID;							//�Զ�ͷ��
	uint32							dwUserID;							//�û� I D
	uint32							dwGameID;							//��Ϸ I D
	uint32							dwSpreaderID;						//�ƹ�ID
	uint32							dwExperience;						//������ֵ
	wchar							szAccounts[LEN_ACCOUNTS];			//�û��ʺ�
	wchar							szNickName[LEN_NICKNAME];			//�û��ǳ�
	wchar							szAliPayAcccount[30];				//֧�����˻�
	wchar							szBinkID[20];						//���п��˻�
	wchar							szDynamicPass[LEN_PASSWORD];		//��̬����

	//�Ƹ���Ϣ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//�û�����	

	//��չ��Ϣ
	uint8							cbInsureEnabled;					//ʹ�ܱ�ʶ
	uint8							cbIsAgent;							//�����ʶ
	uint8							cbMoorMachine;						//��������

	//Լս�����
	int								TodayAlmsCount;						//ÿ�յͱ�����ȡ����
	uint32							dwLockServerID;						//��������
	uint32							dwKindID;							//��Ϸ����

	wchar							szMobilePhone[LEN_MOBILE_PHONE];	//���ֻ�
};

//��¼ʧ��
struct CMD_MB_LogonFailure
{
	uint32							lResultCode;						//�������
	wchar							szDescribe[LEN_ERROR_DESCRIBE];		//������Ϣ
};


//�����б�
#define MDM_MB_SERVER_LIST			101									//�б���Ϣ

#define SUB_MB_KIND_LIST			100									//�����б�
#define SUB_MB_ROOM_LIST			101									//�����б�
#define SUB_MB_LIST_FINISH			200									//�б����


//��Ϸ����
struct CMD_MB_GameKindItem
{
	uint16							wTypeID;							//��������
	uint16							wJoinID;							//�ҽ�����
	uint16							wSortID;							//��������
	uint16							wKindID;							//��������
	uint16							wGameID;							//ģ������
	uint16							wRecommend;							//�Ƽ���Ϸ
	uint16							wGameFlag;							//��Ϸ��־
	uint32							dwOnLineCount;						//��������
	uint32							dwAndroidCount;						//��������
	uint32							dwDummyCount;						//��������
	uint32							dwFullCount;						//��Ա����
	uint32							dwSuportType;						//֧������
	wchar							szKindName[32];						//��Ϸ����
	wchar							szProcessName[32];					//��������
};

//��Ϸ����
struct CMD_MB_GameRoomItem
{
	uint16							wKindID;							//��������
	uint16							wNodeID;							//�ڵ�����
	uint16							wSortID;							//��������
	uint16							wServerID;							//��������
	uint16                          wServerKind;                        //��������
	uint16							wServerType;						//��������
	uint16							wServerLevel;						//����ȼ�
	uint16							wServerPort;						//����˿�
	SCORE							lCellScore;							//��Ԫ����
	uint8							cbEnterMember;						//�����Ա
	SCORE							lEnterScore;						//�������
	SCORE							lTableScore;						//������Ϸ����
	uint32							dwServerRule;						//�������
	uint32							dwOnLineCount;						//��������
	uint32							dwAndroidCount;						//��������
	uint32							dwFullCount;						//��Ա����
	wchar							szServerAddr[32];					//��������
	wchar							szServerName[32];					//��������
	
	//˽�˷����
	uint32							dwSurportType;						//֧������
	uint16							wTableCount;						//������Ŀ
	uint32							dwDummyCount;						//��������
};

#pragma pack()

#endif