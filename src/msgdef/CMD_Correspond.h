#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE


#include "Define.h"

#pragma pack(1)

#define MDM_CS_REGISTER				1									//����ע��

//����ע��
#define SUB_CS_C_REGISTER_PLAZA		100									//ע��㳡

//ע��㳡
struct CMD_CS_C_RegisterPlaza
{
	wchar_t							szServerAddr[32];					//�����ַ
	wchar_t							szServerName[32];					//��������
};

#pragma pack()

#endif