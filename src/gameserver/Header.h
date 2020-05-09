#ifndef HEADER_LOGON_H
#define HEADER_LOGON_H

#ifdef LENDY_COMPILER_14
#include <unordered_map>
#else
#include <map>
#endif
#include "DBEnvHeader.h"
#include "KernelEngineHead.h"

//�����ռ�
namespace Game
{
	using namespace Net;
	using namespace DB;
}

//��������
namespace Game
{
#define NETWORK_CORRESPOND			1									//��¼����

#define IDI_MAIN_MODULE_START		1									//��ʼ��ʶ
#define IDI_MAIN_MODULE_FINISH		99									//��ֹ��ʶ

	enum ServiceUnitsControl
	{
		SUC_CONNECT_CORRESPOND  = 1,
		SUC_LOAD_DB_GAME_LIST	= 2
	};

	enum UIDataControl
	{
		UDC_CORRESPOND_RESULT	= 1,
		UDC_LOAD_DB_LIST_RESULT = 2
	};

	enum SMT
	{
		SMT_CHAT				= 0x0001,								//������Ϣ
		SMT_EJECT				= 0x0002,								//������Ϣ
		SMT_GLOBAL				= 0x0004,								//ȫ����Ϣ
		SMT_PROMPT				= 0x0008,								//��ʾ��Ϣ
		SMT_TABLE_ROLL			= 0x0010,								//������Ϣ

		SMT_CLOSE_ROOM			= 0x0100,								//�رշ���
		SMT_CLOSE_GAME			= 0x0200,								//�ر���Ϸ
		SMT_CLOSE_LINK			= 0x0400,								//�ж�����
		SMT_CLOSE_INSURE		= 0x0800								//�ر�����
	}; 

	enum GameTimerID
	{
		//���ӷ�Χ
		IDI_TABLE_MODULE_START  = 10000,								//��ʼ��ʶ
		IDI_TABLE_MODULE_FINISH = 50000,								//��ֹ��ʶ


		IDI_TABLE_SINK_RANGE	= 50,									//��ʶ��Χ
		IDI_TABLE_MODULE_RANGE	= 100,									//��ʶ��Χ
	};

	enum LogonErrorRoom
	{
		LER_NORMAL				= 0x00,									//�����뿪
		LER_SYSTEM				= 0x01,									//ϵͳԭ��
		LER_NETWORK				= 0x02,									//����ԭ��
		LER_USER_IMPACT			= 0x03,									//�û���ͻ
		LER_SERVER_FULL			= 0x04,									//����Ϊ��
		LER_SERVER_CONDITIONS	= 0x05									//��������
	};

	enum LogonErrorCode
	{
		LEC_NONE = 0,

		LEC_LIMIT_IP = 1,
		LEC_LIMIT_MAC = 2,
		LEC_LIMIT_FREEZE = 4,

		LEC_ROOM_FULL = 5,
		LEC_ROOM_ENTER_SCORE_LESS = 7,

		LEC_USER_PLAYING = 100,
		LEC_USER_ROOM_FULL = 101,
		LEC_USER_TABLE_NOT_CHAIR = 102,
		LEC_USER_ENTER_TABLE = 103,

		LEC_PW_EMPTY = 200,
		LEC_MAX_CODE
	};

	typedef std::pair<LogonErrorCode, std::string> K_LE;
#ifdef LENDY_COMPILER_14
	typedef std::unordered_map<LogonErrorCode, std::string> LogonErrorContainer;
#else
	typedef std::map<LogonErrorCode, std::string> LogonErrorContainer;
#endif
	static LogonErrorContainer LogonError =
	{
		K_LE(LEC_LIMIT_IP, "��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����"),
		K_LE(LEC_LIMIT_MAC, "��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����"),
		K_LE(LEC_LIMIT_FREEZE, "�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����"),

		K_LE(LEC_ROOM_FULL, "��Ǹ�����ڴ˷����Ѿ����������ܼ��������ˣ�"),
		K_LE(LEC_ROOM_ENTER_SCORE_LESS, "����÷�����Ҫ%ld��ң�"),

		K_LE(LEC_USER_PLAYING, "��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),
		K_LE(LEC_USER_ROOM_FULL, "��ǰ��Ϸ�����Ѿ�����Ϊ���ˣ���ʱû�п������������λ�ã����Ժ����ԣ�"),
		K_LE(LEC_USER_TABLE_NOT_CHAIR, "���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"),
		K_LE(LEC_USER_ENTER_TABLE, "��ӭ�����롰%s����Ϸ��ף����Ϸ��죡"),

		K_LE(LEC_PW_EMPTY, "�ܱ�Ǹ�����ĵ�¼������󣬲�����������룡"),
	};

	//���ƽ��
	struct ControlResult
	{
		uint8							cbSuccess;							//�ɹ���־
	};

	struct tagGameAddressOption
	{
		char							szIP[32];
		uint16							wPort;
		uint16							wKindID;
		uint16							wThreadCount;
	};
}


#endif