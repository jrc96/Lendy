#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE


#include "Define.h"
#include "Struct.h"

#pragma pack(1)

#define MDM_CS_REGISTER				1									//����ע��

//����ע��
#define SUB_CS_C_REGISTER_LOGON		100									//ע��㳡
#define SUB_CS_C_REGISTER_ROOM		101									//ע�᷿��

//ע����
#define SUB_CS_S_REGISTER_FAILURE	200									//ע��ʧ��

//ע��㳡
struct CMD_CS_C_RegisterLogon
{
	char							szServerAddr[32];					//�����ַ
	char							szServerName[32];					//��������
};

//ע��ʧ��
struct CMD_CS_S_RegisterFailure
{
	long							lErrorCode;							//�������
	char							szDescribeString[128];				//������Ϣ
};

//ע����Ϸ
struct CMD_CS_C_RegisterRoom
{
	uint16							wKindID;							//��������
	uint16							wServerID;							//��������
	uint16							wServerPort;						//����˿�
	uint64							lCellScore;							//��Ԫ����
	uint64							lEnterScore;						//�������
	uint32							dwOnLineCount;						//��������
	uint32							dwFullCount;						//��Ա����
	uint32							wTableCount;						//������Ŀ
	uint32							dwServerRule;						//�������
	char							szServerAddr[32];					//�����ַ
	char							szServerName[32];					//��������
};

//////////////////////////////////////////////////////////////////////////////////
//������Ϣ

#define MDM_CS_ROOM_INFO			2									//������Ϣ
//��������
#define SUB_CS_S_ROOM_INFO			110									//������Ϣ
#define SUB_CS_S_ROOM_ONLINE		111									//��������
#define SUB_CS_S_ROOM_INSERT		112									//�����б�
#define SUB_CS_S_ROOM_REMOVE		114									//����ɾ��
#define SUB_CS_S_ROOM_FINISH		115									//�������

//��������
struct CMD_CS_S_RoomOnLine
{
	uint16							wServerID;							//�����ʶ
	uint32							dwOnLineCount;						//��������
	uint32							dwAndroidCount;						//��������
};

//����ɾ��
struct CMD_CS_S_RoomRemove
{
	uint16							wServerID;							//�����ʶ
};

//�û�����
#define MDM_CS_USER_COLLECT			3									//�û�����

//�û�״̬
#define SUB_CS_C_USER_ENTER			1									//�û�����
#define SUB_CS_C_USER_LEAVE			2									//�û��뿪
#define SUB_CS_C_USER_STATUS		4									//�û�״̬
//�û�״̬
#define SUB_CS_S_COLLECT_REQUEST	100									//��������


//�û�����
struct CMD_CS_C_UserEnter
{
	//�û���Ϣ
	uint32							dwUserID;							//�û���ʶ
	uint32							dwGameID;							//��Ϸ��ʶ
	char							szNickName[Comm::LEN_NICKNAME];		//�û��ǳ�

	//��ϸ��Ϣ
	Comm::tagUserInfo				userInfo;							//�û���Ϣ
};


//�û��뿪
struct CMD_CS_C_UserLeave
{
	uint32							dwUserID;							//�û���ʶ
};


//�û�״̬
struct CMD_CS_C_UserStatus
{
	//�û���Ϣ
	uint32							dwUserID;							//�û���ʶ
	uint8							cbUserStatus;						//�û�״̬
	uint16							wKindID;							//��Ϸ��ʶ
	uint16							wServerID;							//�����ʶ
	uint16							wTableID;							//��������
	uint16							wChairID;							//����λ��
};


#pragma pack()

#endif