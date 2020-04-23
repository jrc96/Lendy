#ifndef STRUCT_H
#define STRUCT_H

#include "Define.h"

#pragma pack(1)

namespace Comm
{
	//��Ϸ����
	struct tagGameKind
	{
		uint16							wSortID;							//��������
		uint16							wKindID;							//��������
		uint16							wGameID;							//ģ������

		uint32							dwOnLineCount;						//��������
		uint32							dwAndroidCount;						//��������
		uint32							dwFullCount;						//��Ա����
	};

	//��Ϸ����
	struct tagGameRoom
	{
		uint16							wKindID;							//��������
		uint16							wSortID;							//��������
		uint16							wServerID;							//��������
		uint16                          wServerKind;                        //��������
		uint16							wServerLevel;						//����ȼ�
		uint16							wServerPort;						//����˿�
		uint16							wTableCount;						//������
		uint64							lCellScore;							//��Ԫ����
		uint64							lEnterScore;						//�������

		uint32							dwServerRule;						//�������

		uint32							dwOnLineCount;						//��������
		uint32							dwAndroidCount;						//��������
		uint32							dwFullCount;						//��Ա����

		char							szServerAddr[32];					//��������
		char							szServerName[32];					//��������
	};


	//�㳡����
	struct tagGameLogon
	{
		uint16							wPlazaID;							//�㳡��ʶ
		char							szServerAddr[32];					//�����ַ
		char							szServerName[32];					//��������
	};

	//�û���Ϣ
	struct tagUserInfo
	{
		//��������
		uint32							dwUserID;							//�û� I D
		uint32							dwGameID;							//��Ϸ I D
		char							szNickName[LEN_NICKNAME];			//�û��ǳ�
		char							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

		//ͷ����Ϣ
		uint16							wFaceID;							//ͷ������

		//�û�����
		uint8							cbGender;							//�û��Ա�

		//�û�״̬
		uint16							wTableID;							//��������
		uint16							wLastTableID;					    //��Ϸ����
		uint16							wChairID;							//��������
		uint8							cbUserStatus;						//�û�״̬

		//������Ϣ
		uint64							lScore;								//�û�����

		//��Ϸ��Ϣ
		uint32							dwWinCount;							//ʤ������
		uint32							dwLostCount;						//ʧ������
		uint32							dwDrawCount;						//�;�����
		uint32							dwFleeCount;						//��������
	};

	//��������
	struct tagUserInfoPlus
	{
		//��¼��Ϣ
		uint32							dwLogonTime;						//��¼ʱ��
		uint32							dwInoutIndex;						//������ʶ

		//������Ϣ
		uint16							wBindIndex;							//������
		uint32							dwClientAddr;						//���ӵ�ַ
		char							szMachineID[LEN_MACHINE_ID];		//������ʶ

		//���ӱ���
		uint32							dwUserRight;						//�û�Ȩ��
		uint64							lLimitScore;						//���ƻ���

		//��������
		bool							bAndroidUser;						//�����û�
		char							szPassword[LEN_MD5];				//��������
	};
}

#pragma pack()


#endif