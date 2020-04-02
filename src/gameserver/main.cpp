#include "Define.h"
#include "IOContext.h"
#include "Log.h"
#include "ServiceUnits.h"

int main(int argc, char** argv)
{
	using namespace LogComm;

	sLogMgr->Initialize("LogonConfig.ini");

	if (argc < 11)
	{
		LOG_INFO("server.game", "you should start by \"%s  -h 192.168.1.217 -p 7000 -s 1 -t 200 -m 1 ...\"\n", argv[0]);
		LOG_INFO("server.game", "============================= ���� ==============================================");
		LOG_INFO("server.game", "");
		LOG_INFO("server.game", "==================== -h IP  ָ��Ҫ������IP��ַ       =============================");
		LOG_INFO("server.game", "==================== -p PORT ָ��Ҫ�����Ķ˿�        =============================");
		LOG_INFO("server.game", "==================== -d û�в�����ָ����̨����       =============================");
		LOG_INFO("server.game", "==================== -s ServerID ָ����Ϸ������ID    =============================");
		LOG_INFO("server.game", "==================== -t KindID ָ����Ϸ����          =============================");
		LOG_INFO("server.game", "==================== -m GameMod  ָ����Ϸģʽ        =============================");
		LOG_INFO("server.game", "");
		LOG_INFO("server.game", "===================================================================================");
		return -1;
	}

	LOG_INFO("server.logon", "��¼����������");
	
	std::shared_ptr<Net::IOContext> ioContext = std::make_shared<Net::IOContext>();

	SrvUnitsMgr->Start(ioContext.get());

	asio::io_context::work work(*ioContext);
	ioContext->run();

	return 0;
}