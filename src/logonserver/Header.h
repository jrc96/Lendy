#ifndef HEADER_LOGON_H
#define HEADER_LOGON_H

#include <unordered_map>

namespace Logon
{
	using namespace Net;
	using namespace DB;
}

namespace Logon
{
	enum ServiceUnitsControl
	{
		SUC_CONNECT_CORRESPOND  = 1,
		SUC_LOAD_DB_GAME_LIST	= 2
	};

	enum LogonErrorCode
	{
		LEC_NONE = 0,
		LEC_LIMIT_IP = 1,
		LEC_LIMIT_MAC = 2,
		LEC_LIMIT_FREEZE = 4,
		LEC_MAX_CODE
	};

	typedef std::pair<LogonErrorCode, std::string> K;
	typedef std::unordered_map<LogonErrorCode, std::string> LogonErrorContainer;
	static LogonErrorContainer LogonError =
	{
		K(LEC_LIMIT_IP, "��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����"),
		K(LEC_LIMIT_MAC, "��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����"),
		K(LEC_LIMIT_FREEZE, "�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����")
	};
}

#endif