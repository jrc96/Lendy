#include "GameServerManager.h"
#include "TableFrameSink.h"

namespace SubGame
{
	CGameServerManager::CGameServerManager(void)
	{
	}

	CGameServerManager::~CGameServerManager(void)
	{
	}

	CGameServerManager * CGameServerManager::GetInstance()
	{
		static CGameServerManager* _instance = nullptr;
		if (_instance == nullptr)
		{
			_instance = new CGameServerManager();
		}
		return _instance;
	}

	void * CGameServerManager::QueryInterface(GGUID Guid)
	{
		QUERY_INTERFACE(IGameServiceManager, Guid);
		QUERY_INTERFACE_IUNKNOWNEX(IGameServiceManager, Guid);
		return nullptr;
	}

	void * CGameServerManager::CreateTableFrameSink(GGUID Guid)
	{
		//��������
		CTableFrameSink * pTableFrameSink = NULL;
		try
		{
			pTableFrameSink = new CTableFrameSink();
			if (pTableFrameSink == nullptr) throw TEXT("����ʧ��");
			void * pObject = pTableFrameSink->QueryInterface(Guid);
			if (pObject == nullptr) throw TEXT("�ӿڲ�ѯʧ��");
			return pObject;
		}
		catch (...) {}

		//�������
		PDELETE(pTableFrameSink);
		return nullptr;
	}

	void * CGameServerManager::CreateAndroidUserItemSink(GGUID Guid)
	{
		return nullptr;
	}

	void * CGameServerManager::CreateGameDataBaseEngineSink(GGUID Guid)
	{
		return nullptr;
	}

	bool CGameServerManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
	{
		return false;
	}
}

//����������
extern "C" LENDY_GAME_API void* CreateGameServiceManager(GGUID Guid)
{
	return sGSMgr->QueryInterface(Guid);
}