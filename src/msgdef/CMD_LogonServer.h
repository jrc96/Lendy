#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#include "Define.h"

#pragma pack(1)

using namespace comm;

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
	uint64							dwPlazaVersion;						//�㳡�汾
	char							szAgentID[LEN_ACCOUNTS];			//�����ʶ
	uint8                           cbDeviceType;                       //�豸����

	//������Ϣ
	char							szMachineID[LEN_MACHINE_ID];		//������ʶ
	char							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};


//��¼ʧ��
struct CMD_MB_LogonFailure
{
	uint16							lResultCode;						//�������
	char							szDescribe[LEN_ERROR_DESCRIBE];		//������Ϣ
};

#pragma pack()

#endif