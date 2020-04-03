#ifndef GAME_SERVER_MANAGER_H
#define GAME_SERVER_MANAGER_H

#include "../gameserver/GameComponent.h"

namespace SubGame
{
	using namespace Game;

	class CGameServerManager : public IGameServiceManager
	{
		//��������
	public:
		//���캯��
		CGameServerManager(void);
		//��������
		virtual ~CGameServerManager(void);

		static CGameServerManager* GetInstance();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release() { }
		//�ӿڲ�ѯ
		virtual void * QueryInterface(GGUID Guid);

		//�����ӿ�
	public:
		//��������
		virtual void * CreateTableFrameSink(GGUID Guid);
		//��������
		virtual void * CreateAndroidUserItemSink(GGUID Guid);
		//��������
		virtual void * CreateGameDataBaseEngineSink(GGUID Guid);

		//�����ӿ�
	public:
		//��������
		virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);
	};
}

#define sGSMgr SubGame::CGameServerManager::GetInstance()

#endif