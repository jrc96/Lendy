#ifndef STRUCT_H
#define STRUCT_H

#include "Define.h"

#pragma pack(1)

namespace comm
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
	struct tagGameServer
	{
		uint16							wKindID;							//��������
		uint16							wSortID;							//��������
		uint16							wServerID;							//��������
		uint16                          wServerKind;                        //��������
		uint16							wServerLevel;						//����ȼ�
		uint16							wServerPort;						//����˿�
		uint64							lCellScore;							//��Ԫ����
		uint64							lEnterScore;						//�������

		uint32							dwServerRule;						//�������

		uint32							dwOnLineCount;						//��������
		uint32							dwAndroidCount;						//��������
		uint32							dwFullCount;						//��Ա����

		wchar_t							szServerAddr[32];					//��������
		wchar_t							szServerName[32];					//��������
	};
}

#pragma pack()


#endif