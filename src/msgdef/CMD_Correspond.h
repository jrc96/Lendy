#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE


#include "Define.h"

#pragma pack(1)

#define MDM_CS_REGISTER				1									//����ע��

//����ע��
#define SUB_CS_C_REGISTER_PLAZA		100									//ע��㳡

//ע����
#define SUB_CS_S_REGISTER_FAILURE	200									//ע��ʧ��

//ע��㳡
struct CMD_CS_C_RegisterPlaza
{
	wchar_t							szServerAddr[32];					//�����ַ
	wchar_t							szServerName[32];					//��������
};

//ע��ʧ��
struct CMD_CS_S_RegisterFailure
{
	long							lErrorCode;							//�������
	wchar_t							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//������Ϣ

#define MDM_CS_SERVICE_INFO			2									//������Ϣ
//��������
#define SUB_CS_S_SERVER_INFO		110									//������Ϣ
#define SUB_CS_S_SERVER_ONLINE		111									//��������
#define SUB_CS_S_SERVER_INSERT		112									//�����б�
#define SUB_CS_S_SERVER_FINISH		115									//�������

//��������
struct CMD_CS_S_ServerOnLine
{
	uint16							wServerID;							//�����ʶ
	uint32							dwOnLineCount;						//��������
	uint32							dwAndroidCount;						//��������
};

#pragma pack()

#endif