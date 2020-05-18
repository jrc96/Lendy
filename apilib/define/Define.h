#ifndef DEFINE_H
#define DEFINE_H

#include <cstddef>
#include <cinttypes>
#include "Macro.h"

#ifdef _DEBUG
#include <assert.h>
#endif

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

typedef uint64_t uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef volatile int vint;
typedef unsigned char dwSocket;
typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef uint64 uuid64;

typedef unsigned short wchar;
typedef double SCORE;

namespace Comm
{
	static const int FILE_PATH_MAX = 256;

	////////////////////////������ܳ��ñ���
	//�ʺų���
	static const int LEN_ACCOUNTS		= 32;
	//�ǳƳ���
	static const int LEN_NICKNAME		= 32;
	//���볤��
	static const int LEN_PASSWORD		= 33;	
	//���г���
	static const int LEN_MACHINE_ID		= 33;
	//�ƶ��绰
	static const int LEN_MOBILE_PHONE	= 12;	
	//��������
	static const int LEN_ERROR_DESCRIBE = 128;
	//��������
	static const int LEN_MD5			= 33;
	//��Ч��ֵ
	static const uint16 INVALID_WORD	= uint16(-1);	
	static const uint32 INVALID_DWORD	= uint32(-1);
	static const uint16 INVALID_TABLE	= uint16(-1);
	static const uint16 INVALID_CHAIR	= uint16(-1);

	//�û�״̬
	static const uint8 US_NULL			= 0x00;								//û��״̬
	static const uint8 US_FREE			= 0x01;								//վ��״̬
	static const uint8 US_SIT			= 0x02;								//����״̬
	static const uint8 US_READY			= 0x03;								//ͬ��״̬
	static const uint8 US_LOOKON		= 0x04;								//�Թ�״̬
	static const uint8 US_PLAYING		= 0x05;								//��Ϸ״̬
	static const uint8 US_OFFLINE		= 0x06;								//����״̬

	//��������
	static const uint16 MAX_CHAIR		= 200;								//�������
}

#endif