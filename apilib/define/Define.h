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
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef volatile int vint;
typedef unsigned char BYTE;
typedef unsigned long ulong;
typedef uint64 uuid64;


namespace comm
{
    static const uint32 MaxModuleIDCount = 8;

	static const uint32 MaxMsgLen = 1024 * 1024;   

	static const int MaxFullLen = 2048;            // ֧�ֵ����ȫ·���ļ�������

	static const int ConfigFileLen = 128;	

	static const int send_buff_block_size = 1024;

	static const int recv_buff_block_size = 1024 << 4;


	static const int PATH_MAX = 256;
	
	////////////////////////������ܳ��ñ���
	//�ʺų���
	static const int LEN_ACCOUNTS		= 32;
	//���г���
	static const int LEN_MACHINE_ID		= 33;
	//�ƶ��绰
	static const int LEN_MOBILE_PHONE	= 12;	


}

#endif