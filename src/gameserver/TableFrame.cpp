#include "TableFrame.h"
#include "Header.h"
#include "CMD_GameServer.h"
#include "StringFormat.h"

namespace Game
{
	CTableFrame::CTableFrame() : 
		m_wTableID(0),
		m_wChairCount(0),
		m_wUserCount(0),
		m_bDrawStarted(false),
		m_bGameStarted(false),
		m_pGameServiceOption(nullptr)
	{
		//�û�����
		memset(m_TableUserItemArray, 0, sizeof(m_TableUserItemArray));
	}

	CTableFrame::~CTableFrame()
	{

	}

	void * CTableFrame::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(ITableFrame, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(ITableFrame, uuid);
		return nullptr;
	}

	IRoomUserItem * CTableFrame::SearchUserItem(uint32 dwUserID)
	{
		return nullptr;
	}

	IRoomUserItem * CTableFrame::GetTableUserItem(uint32 wChairID)
	{
		//Ч�����
		assert(wChairID < m_wChairCount);
		if (wChairID >= m_wChairCount) return nullptr;

		//��ȡ�û�
		return m_TableUserItemArray[wChairID];
	}

	IRoomUserItem * CTableFrame::SearchUserItemGameID(uint32 dwGameID)
	{
		return nullptr;
	}

	bool CTableFrame::IsGameStarted()
	{
		return m_bGameStarted;
	}

	bool CTableFrame::IsDrawStarted()
	{
		return m_bDrawStarted;
	}

	bool CTableFrame::IsTableStarted()
	{
		return m_bTableStarted;
	}

	void CTableFrame::SetGameStarted(bool cbGameStatus)
	{
	}

	bool CTableFrame::StartGame()
	{
		//��Ϸ״̬
		assert(!m_bDrawStarted);
		if (m_bDrawStarted) return false;

		//�������
		bool bGameStarted = m_bGameStarted;
		bool bTableStarted = m_bTableStarted;

		//����״̬
		m_bGameStarted = true;
		m_bDrawStarted = true;
		m_bTableStarted = true;

		m_dwDrawStartTime = (uint32)time(nullptr);

		//��ʼ����
		if (!bGameStarted)
		{
			//�����û�
			for (uint16 i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IRoomUserItem * pIServerUserItem = GetTableUserItem(i);

				//�����û�
				if (pIServerUserItem != nullptr)
				{
					//������Ϸ��
					if (m_pGameServiceOption->lServiceScore > 0L)
					{
					}

					//����״̬
					uint8 cbUserStatus = pIServerUserItem->GetUserStatus();
					if ((cbUserStatus != US_OFFLINE) && (cbUserStatus != US_PLAYING))
					{
						pIServerUserItem->SetUserStatus(US_PLAYING, m_wTableID, i);
					}
				}
			}

			//����״̬
			if (bTableStarted != m_bTableStarted) SendTableStatus();
		}

		//֪ͨ�¼�
		assert(m_pITableFrameSink != nullptr);
		if (m_pITableFrameSink != nullptr) m_pITableFrameSink->OnEventGameStart();
		return true;
	}

	bool CTableFrame::DismissGame()
	{
		return false;
	}

	bool CTableFrame::ConcludeGame(uint8 cbGameStatus)
	{
		//Ч��״̬
		if (!m_bGameStarted) return false;

		//�������
		bool bDrawStarted = m_bDrawStarted;

		//����״̬
		m_bDrawStarted = false;
		m_cbGameStatus = cbGameStatus;
		m_bGameStarted = (cbGameStatus >= GAME_STATUS_PLAY) ? true : false;
		++m_wPlayCount;

		//��������
		if (!m_bGameStarted)
		{
			//��������
			bool bOffLineWait = false;

			//�����û�
			for (uint16 i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IRoomUserItem * pIServerUserItem = GetTableUserItem(i);

				//�û�����
				if (pIServerUserItem != nullptr)
				{
					//����״̬
					if (pIServerUserItem->GetUserStatus() == US_OFFLINE)
					{
						PerformStandUpAction(pIServerUserItem);
					}
					else
					{
						if (pIServerUserItem->GetUserStatus() == US_NULL)
						{
							pIServerUserItem->SetUserStatus(US_NULL, m_wTableID, i);
							PerformStandUpAction(pIServerUserItem);
						}
						else
						{
							//����״̬
							pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, i);
						}
					}
				}
			}
		}

		//��������
		assert(m_pITableFrameSink != nullptr);
		if (m_pITableFrameSink != nullptr) m_pITableFrameSink->RepositionSink();

		//�߳����
		if (!m_bGameStarted)
		{
			for (uint16 i = 0; i < m_wChairCount; ++i)
			{
				//��ȡ�û�
				if (m_TableUserItemArray[i] == nullptr) continue;
				IRoomUserItem * pIServerUserItem = m_TableUserItemArray[i];

				//��������
				if ((m_pGameServiceOption->lMinEnterScore != 0) && (pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinEnterScore))
				{
					//�û�����
					PerformStandUpAction(pIServerUserItem);
					continue;
				}
			}
		}

		//��������
		ConcludeTable();

		//����״̬
		SendTableStatus();

		return true;
	}

	bool CTableFrame::ConcludeTable()
	{
		//��������
		if (!m_bGameStarted && m_bTableStarted)
		{
			//�����ж�
			uint16 wTableUserCount = GetSitUserCount();
			if (wTableUserCount == 0) m_bTableStarted = false;
			if (m_pGameServiceOption->wChairCount == MAX_CHAIR) m_bTableStarted = false;

			//ģʽ�ж�
			if (m_cbStartMode == START_MODE_FULL_READY) m_bTableStarted = false;
			if (m_cbStartMode == START_MODE_PAIR_READY) m_bTableStarted = false;
			if (m_cbStartMode == START_MODE_ALL_READY) m_bTableStarted = false;
		}

		return true;
	}

	bool CTableFrame::SendGameScene(IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize)
	{
		//�û�Ч��
		assert((pIServerUserItem != nullptr) && pIServerUserItem->IsClientReady());
		if ((pIServerUserItem == nullptr) || !pIServerUserItem->IsClientReady()) return false;

		//���ͳ���
		assert(m_pIMainServiceFrame != nullptr);
		m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_SCENE, pData, wDataSize);
		return true;
	}

	bool CTableFrame::SetGameTimer(uint32 dwTimerID, uint32 dwElapse, uint32 dwRepeat)
	{
		//Ч�����
		assert((dwTimerID > 0) && (dwTimerID < IDI_TABLE_MODULE_RANGE));
		if ((dwTimerID <= 0) || (dwTimerID >= IDI_TABLE_MODULE_RANGE)) return false;

		//����ʱ��
		uint32 dwEngineTimerID = IDI_TABLE_MODULE_START + m_wTableID * IDI_TABLE_MODULE_RANGE;
		if (m_pITimerEngine != nullptr) m_pITimerEngine->SetTimer(dwEngineTimerID + dwTimerID, dwElapse, dwRepeat);

		return true;
	}

	bool CTableFrame::KillGameTimer(uint32 dwTimerID)
	{
		//Ч�����
		assert((dwTimerID > 0) && (dwTimerID <= IDI_TABLE_MODULE_RANGE));
		if ((dwTimerID <= 0) || (dwTimerID > IDI_TABLE_MODULE_RANGE)) return false;

		//ɾ��ʱ��
		uint32 dwEngineTimerID = IDI_TABLE_MODULE_START + m_wTableID * IDI_TABLE_MODULE_RANGE;
		if (m_pITimerEngine != nullptr) m_pITimerEngine->KillTimer(dwEngineTimerID + dwTimerID);

		return true;
	}

	uint8 CTableFrame::GetGameStatus()
	{
		return uint8();
	}

	void CTableFrame::SetGameStatus(uint8 bGameStatus)
	{
	}

	bool CTableFrame::SendTableData(uint16 wChairID, uint16 wSubCmdID, void * pData, uint16 wDataSize, uint16 wMainCmdID)
	{
		//�û�Ⱥ��
		if (wChairID == INVALID_CHAIR)
		{
			for (uint16 i = 0; i < m_wChairCount; ++i)
			{
				//��ȡ�û�
				IRoomUserItem * pIServerUserItem = GetTableUserItem(i);
				if ((pIServerUserItem == nullptr) || !pIServerUserItem->IsClientReady()) continue;

				//��������
				m_pIMainServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
			}
			return true;
		}
		else
		{
			//��ȡ�û�
			IRoomUserItem * pIServerUserItem = GetTableUserItem(wChairID);
			if ((pIServerUserItem == nullptr) || !pIServerUserItem->IsClientReady()) return false;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
			return true;
		}
		return false;
	}

	bool CTableFrame::PerformStandUpAction(IRoomUserItem * pIServerUserItem, bool bInitiative)
	{
		//Ч�����
		if (pIServerUserItem == nullptr)
		{
			return false;
		}

		if (pIServerUserItem->GetTableID() != m_wTableID)
		{
			return false;
		}

		if (pIServerUserItem->GetChairID() > m_wChairCount)
		{
			return false;
		}

		//�û�����
		uint16 wChairID = pIServerUserItem->GetChairID();
		uint8 cbUserStatus = pIServerUserItem->GetUserStatus();
		IRoomUserItem * pITableUserItem = GetTableUserItem(wChairID);

		//��Ϸ�û�
		bool bUserStatus = (cbUserStatus == US_PLAYING) || (cbUserStatus == US_OFFLINE);
		if (m_bGameStarted && bUserStatus)
		{
			//������Ϸ
			uint8 cbConcludeReason = (cbUserStatus == US_OFFLINE) ? GER_NETWORK_ERROR : GER_USER_LEAVE;
			m_pITableFrameSink->OnEventGameConclude(wChairID, pIServerUserItem, cbConcludeReason);

			//�뿪���
			if (m_pGameServiceOption->cbOffLineTrustee == TRUE && m_pITableFrameSink->OnEventGetBetStatus(wChairID, pIServerUserItem))
			{
				pIServerUserItem->SetUserStatus(US_OFFLINE, m_wTableID, wChairID);
				pIServerUserItem->SetClientReady(true);
				return true;
			}

			//�뿪�ж�
			if (m_TableUserItemArray[wChairID] != pIServerUserItem) return true;
		}

		//���ñ���
		if (pIServerUserItem == pITableUserItem)
		{
			//ɾ����ʱ
			if (m_pGameServiceOption->wChairCount < MAX_CHAIR) 
			{
				//KillGameTimer(IDI_START_OVERTIME + wChairID);
			}

			//���ñ���
			m_TableUserItemArray[wChairID] = nullptr;

			//�����������
			m_pIMainServiceFrame->UnLockScoreLockUser(pIServerUserItem->GetUserID(), pIServerUserItem->GetInoutIndex(), LER_NORMAL);

			//�¼�֪ͨ
			if (m_pITableUserAction != nullptr)
			{
				m_pITableUserAction->OnActionUserStandUp(wChairID, pIServerUserItem, false);
			}

			//�û�״̬
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus((cbUserStatus == US_OFFLINE) ? US_NULL : US_FREE, INVALID_TABLE, INVALID_CHAIR);

			//��������
			bool bTableStarted = IsTableStarted();
			m_wUserCount = GetSitUserCount();

			//��������
			ConcludeTable();

			//��ʼ�ж�
			if (EfficacyStartGame(INVALID_CHAIR))
			{
				StartGame();
			}

			//����״̬
			if (bTableStarted != IsTableStarted())
			{
				SendTableStatus();
			}
			return true;
		}
		return true;
	}

	bool CTableFrame::PerformSitDownAction(uint16 wChairID, IRoomUserItem * pIServerUserItem, const char * szPassword)
	{
		//Ч�����
		assert((pIServerUserItem != nullptr) && (wChairID < m_wChairCount));
		assert((pIServerUserItem->GetTableID() == INVALID_TABLE) && (pIServerUserItem->GetChairID() == INVALID_CHAIR));

		//��������
		tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
		IRoomUserItem * pITableUserItem = GetTableUserItem(wChairID);

		//���ֱ���
		uint64 lUserScore = pIServerUserItem->GetUserScore();
		uint64 lMinTableScore = m_pGameServiceOption->lMinEnterScore;
		//uint64 lLessEnterScore = m_pITableFrameSink->QueryLessEnterScore(wChairID, pIServerUserItem);

		//�����ж�
		if (pITableUserItem != nullptr)
		{
			//������Ϣ
			//TCHAR szDescribe[128] = TEXT("");
			//_sntprintf(szDescribe, CountArray(szDescribe), TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"), pITableUserItem->GetNickName());
			//SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NORMAL);
			return false;
		}

		//���ñ���
		m_TableUserItemArray[wChairID] = pIServerUserItem;
		//m_wDrawCount = 0;

		//�û�״̬
		if (IsGameStarted())
		{
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);
		}
		else
		{
			////���ñ���
			//m_wOffLineCount[wChairID] = 0L;
			//m_dwOffLineTime[wChairID] = 0L;

			////������Ϸ��
			//if (m_pGameServiceOption->lServiceScore > 0L)
			//{
			//	m_lFrozenedScore[wChairID] = m_pGameServiceOption->lServiceScore;
			//	pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
			//}
			//����״̬
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_PLAYING, m_wTableID, wChairID);
		}

		//���ñ���
		m_wUserCount = GetSitUserCount();

		//������ʱ
		if (!IsGameStarted())
		{
			//SetGameTimer(IDI_START_OVERTIME + wChairID, TIME_OVERTIME, 1, wChairID);
		}

		//�¼�֪ͨ
		if (m_pITableUserAction != nullptr)
		{
			if (m_wChairCount >= MAX_CHAIR)
			{
				m_pITableUserAction->OnActionUserSitDown(wChairID, pIServerUserItem, false);
			}
			else
			{
				m_pITableUserAction->OnActionUserSitDown(wChairID, pIServerUserItem, false);
			}
		}
		return true;
	}

	tagGameServiceOption * CTableFrame::GetGameServiceOption()
	{
		return m_pGameServiceOption;
	}

	bool CTableFrame::OnEventTimer(uint32 dwTimerID)
	{
		//�ص��¼�
		if ((dwTimerID >= 0) && (dwTimerID < IDI_TABLE_SINK_RANGE))
		{
			assert(m_pITableFrameSink != nullptr);
			return m_pITableFrameSink->OnTimerMessage(dwTimerID);
		}
	}

	bool CTableFrame::OnEventSocketFrame(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem)
	{
		//��Ϸ����
		if (m_pITableFrameSink->OnFrameMessage(wSubCmdID, pData, wDataSize, pIServerUserItem)) return true;

		//Ĭ�ϴ���
		switch (wSubCmdID)
		{
			case SUB_GF_GAME_OPTION:	//��Ϸ����
			{
				//Ч�����
				assert(wDataSize == sizeof(CMD_GF_GameOption));
				if (wDataSize != sizeof(CMD_GF_GameOption)) return false;

				//��������
				CMD_GF_GameOption * pGameOption = (CMD_GF_GameOption *)pData;

				//��ȡ����
				uint16 wChairID = pIServerUserItem->GetChairID();
				uint8 cbUserStatus = pIServerUserItem->GetUserStatus();

				//����״̬
				pIServerUserItem->SetClientReady(true);
				
				//����״̬
				CMD_GF_GameStatus GameStatus;
				GameStatus.cbGameStatus = 0;
				GameStatus.cbAllowLookon = TRUE;
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));

				//������Ϣ
				std::string strMsg = Util::StringFormat(LogonError[LEC_USER_ENTER_TABLE].c_str(), m_pGameServiceOption->strGameName);
				m_pIMainServiceFrame->SendGameMessage(pIServerUserItem, strMsg.c_str(), SMT_CHAT);

				//���ͳ���
				bool bSendSecret = cbUserStatus != US_LOOKON;
				m_pITableFrameSink->OnEventSendGameScene(wChairID, pIServerUserItem, 0, bSendSecret);

				//׼������
				for (uint16 i = 0; i < m_wChairCount; ++i)
				{
					//��ȡ�û�
					IRoomUserItem * pITableUserItem = GetTableUserItem(i);
					if (pITableUserItem != nullptr) continue;

					//����״̬
					CMD_GF_GameUserData GameUserData;
					GameUserData.cbUserCharID = i;
					m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_USER_DATA, &GameUserData, sizeof(GameUserData));
				}

				//��ʼ�ж�
				if ((cbUserStatus == US_READY) && (EfficacyStartGame(wChairID)))
				{
					StartGame();
				}
				return true;
			}
		}
		return false;
	}

	bool CTableFrame::EfficacyStartGame(uint16 wReadyChairID)
	{
		return false;
	}

	bool CTableFrame::InitializationFrame(uint16 wTableID, tagTableFrameParameter & TableFrameParameter)
	{
		m_wTableID = wTableID;
		m_wChairCount = TableFrameParameter.pGameServiceOption->wChairCount;
		m_pGameServiceOption = TableFrameParameter.pGameServiceOption;
		m_pITimerEngine = TableFrameParameter.pITimerEngine;
		m_pIMainServiceFrame = TableFrameParameter.pIMainServiceFrame;
		
		//��������
		IGameServiceManager * pIGameServiceManager = TableFrameParameter.pIGameServiceManager;
		m_pITableFrameSink = (ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink);

		//��չ�ӿ�
		m_pITableUserAction = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, ITableUserAction);

		//��������
		IUnknownEx * pITableFrame = QUERY_ME_INTERFACE(IUnknownEx);
		if (!m_pITableFrameSink->Initialization(pITableFrame)) return false;
		return true;
	}

	void CTableFrame::OnGetGameRecord(void * GameRecord)
	{
		m_pITableFrameSink->OnGetGameRecord(GameRecord);
	}

	uint16 CTableFrame::GetNullChairID()
	{
		//��������
		for (uint16 i = 0; i < m_wChairCount; ++i)
		{
			if (m_TableUserItemArray[i] == nullptr)
			{
				return i;
			}
		}
		return INVALID_CHAIR;
	}

	uint16 CTableFrame::GetSitUserCount()
	{
		//��������
		uint16 wUserCount = 0;

		//��Ŀͳ��
		for (uint16 i = 0; i < m_wChairCount; ++i)
		{
			if (GetTableUserItem(i) != nullptr)
			{
				++wUserCount;
			}
		}
		return wUserCount;
	}

	bool CTableFrame::SendTableStatus()
	{
		return false;
	}
	
}