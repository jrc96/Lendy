#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE


#include "Define.h"

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
#define SUB_CS_S_ROOM_FINISH		115									//�������

//��������
struct CMD_CS_S_ServerOnLine
{
	uint16							wServerID;							//�����ʶ
	uint32							dwOnLineCount;						//��������
	uint32							dwAndroidCount;						//��������
};

#pragma pack()

#endif