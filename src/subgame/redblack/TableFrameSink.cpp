#include "TableFrameSink.h"
#include "CMD_GameServer.h"
#include "CMD_RedBlack.h"
#include "Log.h"
#include "StringUtility.h"
#include "INIReader.h"

namespace SubGame
{
	CTableFrameSink::CTableFrameSink() : 
		m_pITableFrame(nullptr),
		m_pGameServiceOption(nullptr)
	{
		//��ʼ����
		memset(m_lUserStartScore, 0, sizeof(m_lUserStartScore));

		//��ע��
		memset(m_lAllBet, 0, sizeof(m_lAllBet));
		memset(m_lPlayBet, 0, sizeof(m_lPlayBet));

		memset(m_bUserListWin, 0, sizeof(m_bUserListWin));
		memset(m_lUserListScore, 0, sizeof(m_lUserListScore));
		memset(m_wUserPlayCount, 0, sizeof(m_wUserPlayCount));

		//����
		m_lBankerScore = 0;
		memset(m_lPlayScore, 0, sizeof(m_lPlayScore));
		memset(m_lUserWinScore, 0, sizeof(m_lUserWinScore));
		memset(m_lUserRevenue, 0, sizeof(m_lUserRevenue));

		//ʱ�����
		m_cbFreeTime = TIME_FREE;
		m_cbBetTime = TIME_PLACE_JETTON;
		m_cbEndTime = TIME_GAME_END;

		//�˿���Ϣ
		memset(m_cbCardCount, 0, sizeof(m_cbCardCount));
		memset(m_cbTableCardArray, 0, sizeof(m_cbTableCardArray));

		//״̬����
		m_dwBetTime = 0;

		//ׯ����Ϣ
		m_ApplyUserArray.clear();
		m_wCurrentBanker = INVALID_CHAIR;
		m_wOfflineBanker = INVALID_CHAIR;
		m_wBankerTime = 0;
		m_lBankerWinScore = 0;
		m_lBankerCurGameScore = 0;
		m_bEnableSysBanker = true;

		//��¼����
		memset(m_GameRecordArrary, 0, sizeof(m_GameRecordArrary));
		m_dwRecordFirst = 0;
		m_dwRecordLast = 0;
		m_dwRecordCount = 0;
	}

	CTableFrameSink::~CTableFrameSink()
	{
	}

	void CTableFrameSink::Release()
	{
	}
	void * CTableFrameSink::QueryInterface(GGUID Guid)
	{
		QUERY_INTERFACE(ITableFrameSink, Guid);
		QUERY_INTERFACE(ITableUserAction, Guid);
		QUERY_INTERFACE_IUNKNOWNEX(ITableFrameSink, Guid);
		return nullptr;
	}

	void CTableFrameSink::RepositionSink()
	{
		m_wOfflineBanker = INVALID_CHAIR;

		memset(m_lAllBet, 0, sizeof(m_lAllBet));
		memset(m_lPlayBet, 0, sizeof(m_lPlayBet));

		//����
		m_lBankerScore = 0;
		memset(m_lPlayScore, 0, sizeof(m_lPlayScore));
		memset(m_lUserWinScore, 0, sizeof(m_lUserWinScore));
		memset(m_lUserRevenue, 0, sizeof(m_lUserRevenue));
		memset(m_cbOpenResult, 0, sizeof(m_cbOpenResult));
	}

	bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
	{
		//��ѯ�ӿ�
		assert(pIUnknownEx != nullptr);
		m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
		if (m_pITableFrame == nullptr) return false;

		//��ȡ����
		m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
		assert(m_pGameServiceOption != nullptr);

		//��ʼģʽ
		m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

		ReadConfigInformation();
		return true;
	}
	bool CTableFrameSink::OnActionUserOffLine(uint16 wChairID, IRoomUserItem * pIServerUserItem)
	{
		return false;
	}
	bool CTableFrameSink::OnActionUserConnect(uint16 wChairID, IRoomUserItem * pIServerUserItem)
	{
		return false;
	}
	bool CTableFrameSink::OnActionUserSitDown(uint16 wChairID, IRoomUserItem * pIServerUserItem, bool bLookonUser)
	{
		//��ʼ����
		m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();

		//����ʱ��
		if (m_dwBetTime == 0)
		{
			m_dwBetTime = time(nullptr);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1);
			m_pITableFrame->SetGameStatus(GAME_SCENE_BET);
			m_pITableFrame->StartGame();
		}
		return true;
	}
	bool CTableFrameSink::OnActionUserStandUp(uint16 wChairID, IRoomUserItem * pIServerUserItem, bool bLookonUser)
	{
		//��ʼ����
		m_wUserPlayCount[wChairID] = 0;
		m_lUserStartScore[wChairID] = 0;
		memset(m_bUserListWin[wChairID], 0, sizeof(m_bUserListWin[wChairID]));
		memset(m_lUserListScore[wChairID], 0, sizeof(m_lUserListScore[wChairID]));
		return true;
	}
	bool CTableFrameSink::OnActionUserOnReady(uint16 wChairID, IRoomUserItem * pIServerUserItem, void * pData, uint16 wDataSize)
	{
		return false;
	}
	bool CTableFrameSink::OnEventGameStart()
	{
		//��ȡׯ��
		if (m_wCurrentBanker == INVALID_CHAIR)
		{
			m_lBankerScore = 1000000000;
		}
		else
		{
			IRoomUserItem* pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if (pIBankerServerUserItem != nullptr)
			{
				m_lBankerScore = pIBankerServerUserItem->GetUserScore();
			}
		}

		//��������
		CMD_S_GameStart GameStart;

		//���ñ���
		GameStart.cbBetTime = m_cbBetTime;
		for (int i = 0; i < MAX_SEAT_COUNT; ++i)
		{
			GameStart.wSeatUser[i] = INVALID_CHAIR;
		}
		OnGetUserListGameID(GameStart.wSeatUser);

		//�Թ����
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		return true;
	}

	bool CTableFrameSink::OnEventGameConclude(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbReason)
	{
		switch (cbReason)
		{
			case GER_NORMAL:		//�������	
			{
				//�������
				SCORE lBankerWinScore = GameOver();

				//��������
				m_wBankerTime++;

				//������Ϣ
				CMD_S_GameEnd GameEnd;
				memset(&GameEnd, 0, sizeof(GameEnd));

				//�б������Ϣ
				for (uint16 i = 0; i < GAME_PLAYER; ++i)
				{
					IRoomUserItem *pServerUser = m_pITableFrame->GetTableUserItem(i);
					if (!pServerUser) continue;

					if (m_lUserWinScore[i] > 0)
					{
						m_bUserListWin[i][m_wUserPlayCount[i]] = true;
					}

					SCORE lAddScore = 0;
					for (uint8 j = 0; j < AREA_MAX; j++)
					{
						lAddScore += m_lPlayBet[i][j];
					}
					m_lUserListScore[i][m_wUserPlayCount[i]] = lAddScore;
					m_wUserPlayCount[i]++;

					if (m_wUserPlayCount[i] == USER_LIST_COUNT)
					{
						m_wUserPlayCount[i] = 0;
					}
				}

				//ռλ��ҳɼ�
				memset(GameEnd.wSeatUser, INVALID_CHAIR, sizeof(GameEnd.wSeatUser));
				OnGetUserListGameID(GameEnd.wSeatUser);

				for (uint16 i = 0; i < MAX_SEAT_COUNT; ++i)
				{
					if (GameEnd.wSeatUser[i] != INVALID_CHAIR)
					{
						IRoomUserItem *pServerUser = m_pITableFrame->GetTableUserItem(GameEnd.wSeatUser[i]);
						if (!pServerUser) continue;

						GameEnd.lSeatUserWinScore[i] = m_lUserWinScore[GameEnd.wSeatUser[i]];
						GameEnd.lSeatUserRestScore[i] = pServerUser->GetUserScore() + m_lUserWinScore[GameEnd.wSeatUser[i]];
					}
				}

				//ׯ����Ϣ
				GameEnd.lBankerWinScore = lBankerWinScore;
				if (m_wCurrentBanker == INVALID_CHAIR)
				{
					GameEnd.lBankerRestScore = 100000000;
				}
				else
				{
					IRoomUserItem* pBankerUser = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
					if (pBankerUser)
					{
						GameEnd.lBankerRestScore = pBankerUser->GetUserScore() + lBankerWinScore;
					}
				}

				//�˿���Ϣ
				memcpy(GameEnd.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
				memcpy(GameEnd.cbResult, m_cbOpenResult, sizeof(GameEnd.cbResult));
				memcpy(GameEnd.lAllJettonScore, m_lAllBet, sizeof(GameEnd.lAllJettonScore));

				for (uint16 i = 0; i < GAME_PLAYER; ++i)
				{
					IRoomUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem == nullptr) continue;

					GameEnd.lPlayerWinScore = m_lUserWinScore[i];
					GameEnd.lPlayerRestScore = pIServerUserItem->GetUserScore() + GameEnd.lPlayerWinScore;

					//���óɼ�
					memcpy(GameEnd.lPlayAreaScore, m_lPlayScore[i], sizeof(GameEnd.lPlayAreaScore));
					memcpy(GameEnd.lPlayerJettonScore, m_lPlayBet[i], sizeof(GameEnd.lPlayerJettonScore));

					//������Ϣ					
					m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				}

				//tagRBPerGameRecord GameRecord;
				//GameRecord.wTableID = m_pITableFrame->GetTableID() + 1;
				//GameRecord.cbAreaWin[0] = m_cbOpenResult[0];
				//GameRecord.cbAreaWin[1] = m_cbOpenResult[1];
				//GameRecord.cbAreaWin[2] = m_cbOpenResult[2];
				//GameRecord.cbAreaWin[3] = m_cbOpenResult[3];
				//m_pITableFrame->SendTableDataToLookOnUser(SUB_GF_BJL_GAME_RESULT, &GameRecord, sizeof(GameRecord));

				return true;
			}
			case GER_NETWORK_ERROR:		//�����ж�
			case GER_USER_LEAVE:		//�û��뿪
			{
				return true;
			}
		}
		return false;
	}

	bool CTableFrameSink::OnEventSendGameScene(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbGameStatus, bool bSendSecret)
	{
		switch (cbGameStatus)
		{
			case GAME_SCENE_BET:		//��Ϸ״̬
			case GAME_SCENE_END:		//����״̬
			{
				CMD_S_StatusPlay StatusPlay;
				memset(&StatusPlay, 0, sizeof(StatusPlay));
				memset(StatusPlay.wApplyUser, INVALID_CHAIR, sizeof(StatusPlay.wApplyUser));
				memset(StatusPlay.wSeatUser, INVALID_CHAIR, sizeof(StatusPlay.wSeatUser));

				uint8 cbPassTime = (uint16)(time(nullptr) - m_dwBetTime);
				uint8 cbTotalTime = (cbGameStatus == GAME_SCENE_BET ? m_cbBetTime : m_cbEndTime);
				StatusPlay.cbTimeLeave = cbTotalTime - __min(cbPassTime, cbTotalTime);

				//ׯ����Ϣ
				StatusPlay.wBankerUser = m_wCurrentBanker;
				StatusPlay.lBankerWinScore = m_lBankerWinScore;

				//��ȡׯ��
				IRoomUserItem* pIBankerServerUserItem = nullptr;
				if (m_wCurrentBanker == INVALID_CHAIR)
				{
					m_lBankerScore = 1000000000;
				}
				else
				{
					pIBankerServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
					if (pIBankerServerUserItem != nullptr)
					{
						m_lBankerScore = pIBankerServerUserItem->GetUserScore();
					}
				}
				StatusPlay.lBankerScore = m_lBankerScore;

				//������Ϣ
				StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;
				StatusPlay.lAreaLimitScore = m_lAreaLimitScore;

				StatusPlay.cbFreeTime = m_cbFreeTime;
				StatusPlay.cbPlayTime = m_cbBetTime;
				StatusPlay.cbEndTime = m_cbEndTime;

				StatusPlay.lMinXianHong = m_nChip[0];
				StatusPlay.lMaxXianHong = m_lUserLimitScore;

				memcpy(StatusPlay.lPlayerJettonScore, m_lPlayBet[wChairID], sizeof(StatusPlay.lPlayerJettonScore));
				memcpy(StatusPlay.lAllJettonScore, m_lAllBet, sizeof(StatusPlay.lAllJettonScore));

				//ռλ�ɼ�
				memset(StatusPlay.wSeatUser, INVALID_CHAIR, sizeof(StatusPlay.wSeatUser));
				OnGetUserListGameID(StatusPlay.wSeatUser);

				//�����ж�
				if (cbGameStatus == GAME_SCENE_END)
				{
					//�˿���Ϣ
					memcpy(StatusPlay.cbTableCardArray, m_cbTableCardArray, sizeof(m_cbTableCardArray));
					memcpy(StatusPlay.cbResult, m_cbOpenResult, sizeof(StatusPlay.cbResult));
					StatusPlay.lPlayerWinScore = m_lUserWinScore[pIServerUserItem->GetChairID()];

					for (uint16 i = 0; i < MAX_SEAT_COUNT; ++i)
					{
						if (StatusPlay.wSeatUser[i] != INVALID_CHAIR)
						{
							StatusPlay.lSeatUserWinScore[i] = m_lUserWinScore[StatusPlay.wSeatUser[i]];
						}
					}

					if (m_wCurrentBanker != INVALID_CHAIR)
					{
						if (pIBankerServerUserItem != nullptr)
						{
							StatusPlay.lBankerRestScore = pIBankerServerUserItem->GetUserScore() + m_lUserWinScore[m_wCurrentBanker];
						}
					}
					else
					{
						StatusPlay.lBankerRestScore = 1000000000;
					}
					StatusPlay.lPlayerRestScore = pIServerUserItem->GetUserScore() + m_lUserWinScore[pIServerUserItem->GetChairID()];
				}
				else
				{
					if (m_wCurrentBanker != INVALID_CHAIR)
					{
						if (pIBankerServerUserItem != nullptr)
						{
							StatusPlay.lBankerRestScore = pIBankerServerUserItem->GetUserScore();
						}
					}
					else
					{
						StatusPlay.lBankerRestScore = 1000000000;
					}
				}
				StatusPlay.lPlayerRestScore = pIServerUserItem->GetUserScore();

				for (uint16 i = 0; i < MAX_SEAT_COUNT; ++i)
				{
					if (StatusPlay.wSeatUser[i] == INVALID_CHAIR) continue;

					IRoomUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(StatusPlay.wSeatUser[i]);
					if (pIServerUserItem == nullptr) continue;

					uint64 BetCount = 0;
					StatusPlay.SeatPlayerRestScore[i] = pIServerUserItem->GetUserScore();
				}

				memcpy(StatusPlay.nChip, m_nChip, sizeof(StatusPlay.nChip));

				for (int nUserIdx = 0; nUserIdx < m_ApplyUserArray.size(); ++nUserIdx)
				{
					uint16 wChairID = m_ApplyUserArray[nUserIdx];

					//��ȡ���
					IRoomUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (!pServerUserItem) continue;

					//ׯ���ж�
					if (pServerUserItem->GetChairID() == m_wCurrentBanker) continue;
					StatusPlay.wApplyUser[nUserIdx] = wChairID;
				}

				for (uint16 i = 0; i < MAX_SEAT_COUNT; ++i)
				{
					if (StatusPlay.wSeatUser[i] != INVALID_CHAIR)
					{
						uint16 wSeatChair = StatusPlay.wSeatUser[i];
						memcpy(StatusPlay.lSeatUserAreaScore[i], m_lPlayBet[wSeatChair], sizeof(StatusPlay.lSeatUserAreaScore[i]));
					}
				}

				//���ͳ���
				bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));

				SendGameRecord(pIServerUserItem);

				return true;
			}
		}
		return false;
	}
	bool CTableFrameSink::OnEventGetBetStatus(uint16 wChairID, IRoomUserItem * pIServerUserItem)
	{
		for (uint16 i = AREA_XIAN; i <= AREA_MAX; ++i)
		{
			if (m_lPlayBet[wChairID][i] != 0)
			{
				return true;
			}
		}

		if (wChairID == m_wCurrentBanker)
		{
			return true;
		}
		return false;
	}
	void CTableFrameSink::OnGetGameRecord(void * GameRecord)
	{
		CMD_GF_RBRoomStatus *pRoomStatus = (CMD_GF_RBRoomStatus *)GameRecord;

		pRoomStatus->tagGameInfo.wTableID = m_pITableFrame->GetTableID() + 1;
		pRoomStatus->tagGameInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

		pRoomStatus->tagTimeInfo.cbBetTime = m_cbBetTime;
		pRoomStatus->tagTimeInfo.cbEndTime = m_cbEndTime;
		pRoomStatus->tagTimeInfo.cbPassTime = (uint32)time(nullptr) - m_dwBetTime;
		pRoomStatus->tagTimeInfo.lMinXianHong = m_nChip[0];
		pRoomStatus->tagTimeInfo.lMaxXianHong = m_lUserLimitScore;

		//�ͻ���ֻ��ʾ48��
		int nIndex = 0;

		pRoomStatus->cbRecordCount = 0;
		int nArrayIndex = 0;

		while (nIndex != m_dwRecordLast)
		{
			if (nArrayIndex >= 48)
			{
				break;
			}
			pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[0] = m_GameRecordArrary[nIndex].bPlayer;
			pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[1] = m_GameRecordArrary[nIndex].bBanker;
			pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[2] = m_GameRecordArrary[nIndex].cbPing;
			pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[3] = m_GameRecordArrary[nIndex].cbCardType;

			nArrayIndex++;
			nIndex = (nIndex + 1) % MAX_SCORE_HISTORY;
		}
	}
	bool CTableFrameSink::OnGameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem)
	{
		switch (wSubCmdID)
		{
			case SUB_C_PLACE_JETTON:		//�û���ע
			{
				//Ч������
				assert(wDataSize == sizeof(CMD_C_PlaceBet));
				if (wDataSize != sizeof(CMD_C_PlaceBet)) return true;

				//��Ϣ����
				CMD_C_PlaceBet * pPlaceBet = (CMD_C_PlaceBet *)pData;

				return OnUserPlayBet(pIServerUserItem->GetChairID(), pPlaceBet->cbBetArea, pPlaceBet->lBetScore);
			}
			case SUB_C_CHEHUI:			//������Ϣ
			{
				uint16 wChairID = pIServerUserItem->GetChairID();

				for (int i = 0; i < AREA_MAX; i++)
				{
					m_lAllBet[i] -= m_lPlayBet[wChairID][i];
				}
				memset(m_lPlayBet[wChairID], 0, sizeof(m_lPlayBet[wChairID]));

				CMD_S_CheHui CheHui;
				memset(&CheHui, 0, sizeof(CheHui));
				CheHui.wChairID = wChairID;
				CheHui.lUserRestScore = pIServerUserItem->GetUserScore();
				memcpy(CheHui.SurplusChip, m_lAllBet, sizeof(CheHui.SurplusChip));

				for (uint16 i = 0; i < GAME_PLAYER; i++)
				{
					IRoomUserItem *pServerUser = m_pITableFrame->GetTableUserItem(i);
					if (!pServerUser) continue;
					m_pITableFrame->SendTableData(i, SUB_S_CHEHUI, &CheHui, sizeof(CheHui));
				}
				return true;
			}
			case SUB_C_ONLINE_PLAYER:		//��ȡ�û��б�
			{
				return OnUserOnlineList(pIServerUserItem->GetChairID());
			}
		}
		return false;
	}
	bool CTableFrameSink::OnFrameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem)
	{
		return false;
	}

	bool CTableFrameSink::OnTimerMessage(uint32 dwTimerID)
	{
		switch (dwTimerID)
		{
			case IDI_PLACE_JETTON:		//��עʱ��
			{
				//״̬�ж�
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
				{
					//����״̬
					m_pITableFrame->SetGameStatus(GAME_SCENE_END);

					//������Ϸ
					OnEventGameConclude(INVALID_CHAIR, nullptr, GER_NORMAL);

					//����ʱ��
					m_dwBetTime = (uint32)time(nullptr);
					m_pITableFrame->SetGameTimer(IDI_GAME_END, m_cbEndTime * 1000, 1);
				}
				return true;
			}
			case IDI_GAME_END:
			{
				//д�����
				for (uint16 wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
				{
					IRoomUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
					if (pIServerUserItem == nullptr)
					{
						continue;
					}

					//�ж��Ƿ�д��
					bool bXie = false;
					for (uint16 iA = 0; iA < AREA_MAX; ++iA)
					{
						if (m_lPlayBet[wUserChairID][iA] != 0)
						{
							bXie = true;
							break;
						}
					}

					//��ǰׯ��
					if (wUserChairID == m_wCurrentBanker)
					{
						bXie = true;
					}

					//����ׯ��
					if (wUserChairID == m_wOfflineBanker)
					{
						bXie = true;
					}

					if (!bXie)
					{
						continue;
					}

					//��ֹ����
					if ((pIServerUserItem->GetUserScore() + m_lUserWinScore[wUserChairID]) < 0L)
					{
						m_lUserWinScore[wUserChairID] = -pIServerUserItem->GetUserScore();
					}

					//д�����				
					m_pITableFrame->WriteUserScore(wUserChairID, m_lUserWinScore[wUserChairID]);
				}

				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

				//��ʼ��Ϸ
				m_pITableFrame->StartGame();

				//������ʱ
				m_dwBetTime = time(nullptr);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1);
			}
		}
		return false;
	}

	bool CTableFrameSink::OnUserPlayBet(uint16 wChairID, uint8 cbBetArea, SCORE lBetScore)
	{
		//Ч�����
		assert((cbBetArea <= AREA_MAX) && (lBetScore > 0L));
		if ((cbBetArea > AREA_MAX) || (lBetScore <= 0L))
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("��ע��Ϣ����");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
			return true;
		}

		//Ч��״̬
		if (m_pITableFrame->GetGameStatus() != GAME_SCENE_BET)
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("��ǰ������ע״̬��������ע��");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
			return true;
		}

		//ׯ���ж�
		if (m_wCurrentBanker == wChairID)
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("ׯ�Ҳ�����ע��");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
			return true;
		}

		if (!m_bEnableSysBanker && m_wCurrentBanker == INVALID_CHAIR)
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("�÷���ϵͳ��ׯ������ע��");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
			return true;
		}

		//ׯ�һ���
		SCORE lBankerScore = 0;
		if (INVALID_CHAIR != m_wCurrentBanker)
		{
			IRoomUserItem * pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			lBankerScore = pIServerUserItemBanker->GetUserScore();
		}

		//��������
		IRoomUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		SCORE lUserScore = pIServerUserItem->GetUserScore();
		SCORE lBetCount = 0;
		for (int i = 0; i < AREA_MAX; ++i)
		{
			lBetCount += m_lPlayBet[wChairID][i];
		}

		SCORE lAllBetCount = 0L;
		for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex)
		{
			lAllBetCount += m_lAllBet[nAreaIndex];
		}

		//�ɹ���ʶ
		bool bPlaceBetSuccess = true;

		//�Ϸ�У��
		if (lUserScore < lBetCount + lBetScore)
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("���Ľ�Ҳ��㣡");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
			return true;
		}

		if (m_lUserLimitScore < lBetCount + lBetScore)
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("������ע�ѵ������ƣ�");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
			return true;
		}

		//�Ϸ���֤
		if (GetMaxPlayerScore(cbBetArea, wChairID) >= lBetScore)
		{
			m_lAllBet[cbBetArea] += lBetScore;
			m_lPlayBet[wChairID][cbBetArea] += lBetScore;
		}
		else
		{
			bPlaceBetSuccess = false;
		}

		if (bPlaceBetSuccess)
		{
			//��������
			CMD_S_PlaceBet PlaceBet;
			memset(&PlaceBet, 0, sizeof(PlaceBet));

			//�������
			PlaceBet.wChairID = wChairID;
			PlaceBet.cbBetArea = cbBetArea;
			PlaceBet.lBetScore = lBetScore;

			SCORE lPlayerTableScore = m_lPlayBet[wChairID][0] + m_lPlayBet[wChairID][1] + m_lPlayBet[wChairID][2];
			PlaceBet.lPlayerRestScore = pIServerUserItem->GetUserScore() - lPlayerTableScore;

			uint16 wSeatUser[MAX_SEAT_COUNT] = { 0 };
			memset(wSeatUser, INVALID_CHAIR, sizeof(wSeatUser));
			OnGetUserListGameID(wSeatUser);

			for (uint16 i = 0; i < GAME_PLAYER; i++)
			{
				bool bIsSeatUser = false;
				for (uint16 j = 0; j < MAX_SEAT_COUNT; j++)
				{
					if (wSeatUser[j] == wChairID)
					{
						bIsSeatUser = true;
					}
				}

				//������Ϣ
				if (bIsSeatUser) 
				{
					m_pITableFrame->SendTableData(i, SUB_S_SEAT_JETTON, &PlaceBet, sizeof(PlaceBet));
				}
				else if (i == wChairID)
				{
					m_pITableFrame->SendTableData(i, SUB_S_PLACE_JETTON, &PlaceBet, sizeof(PlaceBet));
				}
				else
				{
					m_pITableFrame->SendTableData(i, SUB_S_OTHER_JETTON, &PlaceBet, sizeof(PlaceBet));
				}
			}
		}
		else
		{
			CMD_S_PlaceBetFail PlaceBetFail;
			std::wstring wstrMessage = Util::StringUtility::StringToWString("������ע�ѵ������ƣ�");
			swprintf((wchar_t*)PlaceBetFail.szBuffer, sizeof(PlaceBetFail.szBuffer), L"%ls", wstrMessage.c_str());
			m_pITableFrame->SendTableData(wChairID, SUB_S_PLACE_JETTON_FAIL, &PlaceBetFail, sizeof(PlaceBetFail));
		}

		return true;
	}

	bool CTableFrameSink::OnUserOnlineList(uint16 wChairID)
	{
		CMD_S_UserListInfo UserListInfo[GAME_PLAYER];
		memset(UserListInfo, 0, sizeof(UserListInfo));

		uint8 cbListUserCount = 0;
		for (uint16 i = 0; i < GAME_PLAYER; i++)
		{
			IRoomUserItem *pServerUser = m_pITableFrame->GetTableUserItem(i);
			if (!pServerUser) continue;

			SCORE lAddScore = 0;
			uint8 cbWinCount = 0;

			for (uint8 j = 0; j < USER_LIST_COUNT; j++)
			{
				if (m_bUserListWin[i][j])
				{
					cbWinCount++;
				}
				lAddScore += m_lUserListScore[i][j];
			}

			UserListInfo[cbListUserCount].wWinNum = cbWinCount;
			UserListInfo[cbListUserCount].lAllBet = lAddScore;
			UserListInfo[cbListUserCount].lUserScore = pServerUser->GetUserScore();
			swprintf((wchar_t*)UserListInfo[cbListUserCount].szNickName, sizeof(UserListInfo[cbListUserCount].szNickName), L"%ls", pServerUser->GetNickName());
			UserListInfo[cbListUserCount].wFaceID = pServerUser->GetUserInfo()->wFaceID;
			cbListUserCount++;
		}

		//�ҳ�һ����ʤ�������ķŵ�һ��
		for (uint16 i = 1; i < cbListUserCount; i++)
		{
			bool bExchange = false;
			if (UserListInfo[i].wWinNum > UserListInfo[0].wWinNum)
			{
				bExchange = true;
			}
			else if (UserListInfo[i].wWinNum == UserListInfo[0].wWinNum)
			{
				if (UserListInfo[i].lAllBet > UserListInfo[0].lAllBet)
				{
					bExchange = true;
				}
			}

			if (bExchange)
			{
				CMD_S_UserListInfo UserTempInfo;
				memset(&UserTempInfo, 0, sizeof(UserTempInfo));
				memcpy(&UserTempInfo, &UserListInfo[i], sizeof(UserTempInfo));
				memcpy(&UserListInfo[i], &UserListInfo[0], sizeof(UserTempInfo));
				memcpy(&UserListInfo[0], &UserTempInfo, sizeof(UserTempInfo));
			}
		}

		//ʣ�µİ���ע������
		for (uint16 i = 1; i < cbListUserCount; i++)
		{
			for (uint16 j = i + 1; j < cbListUserCount; j++)
			{
				if (UserListInfo[j].lAllBet > UserListInfo[i].lAllBet)
				{
					CMD_S_UserListInfo UserTempInfo;
					memset(&UserTempInfo, 0, sizeof(UserTempInfo));

					memcpy(&UserTempInfo, &UserListInfo[i], sizeof(UserTempInfo));
					memcpy(&UserListInfo[i], &UserListInfo[j], sizeof(UserTempInfo));
					memcpy(&UserListInfo[j], &UserTempInfo, sizeof(UserTempInfo));
				}
			}
		}

		uint8 cbMaxTimes = cbListUserCount / 20;
		if (cbListUserCount % 20 != 0)
		{
			cbMaxTimes += 1;
		}

		for (int i = 0; i < cbMaxTimes; i++)
		{
			uint8 cbRestCount = 20;
			if (i == (cbMaxTimes - 1))
			{
				cbRestCount = cbListUserCount % 20;
			}

			CMD_S_UserList UpdateUserList;
			memset(&UpdateUserList, 0, sizeof(UpdateUserList));
			UpdateUserList.wCount = cbRestCount;
			if (i != (cbMaxTimes - 1))
			{
				UpdateUserList.bEnd = false;
			}
			else
			{
				UpdateUserList.bEnd = true;
			}

			for (int j = 0; j < cbRestCount; j++)
			{
				UpdateUserList.cbIndex[j] = i * 20 + j;
			}

			for (int j = 0; j < cbRestCount; j++)
			{
				UpdateUserList.cbWinTimes[j] = UserListInfo[i * 20 + j].wWinNum;
			}
			for (int j = 0; j < cbRestCount; j++)
			{
				UpdateUserList.lBetScore[j] = UserListInfo[i * 20 + j].lAllBet;
			}
			for (int j = 0; j < cbRestCount; j++)
			{
				UpdateUserList.lUserScore[j] = UserListInfo[i * 20 + j].lUserScore;
			}

			for (int j = 0; j < cbRestCount; j++)
			{
				swprintf((wchar_t*)UpdateUserList.szUserNick[j], sizeof(UpdateUserList.szUserNick[j]), L"%ls", UserListInfo[i * 20 + j].szNickName);
			}
			for (int j = 0; j < cbRestCount; j++)
			{
				UpdateUserList.wFaceID[j] = UserListInfo[i * 20 + j].wFaceID;
			}
			m_pITableFrame->SendTableData(wChairID, SUB_S_ONLINE_PLAYER, &UpdateUserList, sizeof(UpdateUserList));
		}
		return true;
	}

	bool CTableFrameSink::OnGetUserListGameID(uint16 wSeatUser[MAX_SEAT_COUNT])
	{
		CMD_S_UserListInfo UserListInfo[GAME_PLAYER];
		memset(UserListInfo, 0, sizeof(UserListInfo));

		uint8 cbListUserCount = 0;
		for (uint16 i = 0; i < GAME_PLAYER; ++i)
		{
			IRoomUserItem *pServerUser = m_pITableFrame->GetTableUserItem(i);
			if (!pServerUser) continue;

			SCORE lAddScore = 0;
			uint8 cbWinCount = 0;

			for (uint8 j = 0; j < USER_LIST_COUNT; j++)
			{
				if (m_bUserListWin[i][j])
				{
					cbWinCount++;
				}
				lAddScore += m_lUserListScore[i][j];
			}

			UserListInfo[cbListUserCount].wWinNum = cbWinCount;
			UserListInfo[cbListUserCount].lAllBet = lAddScore;
			UserListInfo[cbListUserCount].lUserScore = pServerUser->GetUserScore();
			UserListInfo[cbListUserCount].wChairID = pServerUser->GetChairID();

			std::wstring wstrNickName = Util::StringUtility::StringToWString(pServerUser->GetNickName());
			swprintf((wchar_t*)UserListInfo[cbListUserCount].szNickName, sizeof(UserListInfo[cbListUserCount].szNickName), L"%ls", wstrNickName.c_str());
			cbListUserCount++;
		}

	
		for (uint16 i = 1; i < cbListUserCount; ++i)
		{
			bool bExchange = false;
			if (UserListInfo[i].wWinNum > UserListInfo[0].wWinNum)
			{
				bExchange = true;
			}
			else if (UserListInfo[i].wWinNum == UserListInfo[0].wWinNum)
			{
				if (UserListInfo[i].lAllBet > UserListInfo[0].lAllBet)
				{
					bExchange = true;
				}
			}

			if (bExchange)
			{
				CMD_S_UserListInfo UserTempInfo;
				memset(&UserTempInfo, 0, sizeof(UserTempInfo));

				memcpy(&UserTempInfo, &UserListInfo[i], sizeof(UserTempInfo));
				memcpy(&UserListInfo[i], &UserListInfo[0], sizeof(UserTempInfo));
				memcpy(&UserListInfo[0], &UserTempInfo, sizeof(UserTempInfo));
			}
		}

		//ʣ�µİ���ע������
		for (uint16 i = 1; i < cbListUserCount; ++i)
		{
			for (uint16 j = i + 1; j < cbListUserCount; j++)
			{
				if (UserListInfo[j].lAllBet > UserListInfo[i].lAllBet)
				{
					CMD_S_UserListInfo UserTempInfo;
					memset(&UserTempInfo, 0, sizeof(UserTempInfo));

					memcpy(&UserTempInfo, &UserListInfo[i], sizeof(UserTempInfo));
					memcpy(&UserListInfo[i], &UserListInfo[j], sizeof(UserTempInfo));
					memcpy(&UserListInfo[j], &UserTempInfo, sizeof(UserTempInfo));
				}
			}
		}

		for (int i = 0; i < MAX_SEAT_COUNT; i++)
		{
			if (cbListUserCount > i)
			{
				wSeatUser[i] = UserListInfo[i].wChairID;
			}
		}
		return true;
	}

	SCORE CTableFrameSink::GameOver()
	{
		//�������
		SCORE lBankerWinScore = 0;
		bool bSuccess = false;

		//��Ϸ��¼
		DispatchTableCard();

		tagServerGameRecord& GameRecord = m_GameRecordArrary[m_dwRecordLast];
		CalculateScore(lBankerWinScore, GameRecord);

		//�ۼƻ���
		m_lBankerWinScore += lBankerWinScore;

		//��ǰ����
		m_lBankerCurGameScore = lBankerWinScore;

		//�ƶ��±�
		m_dwRecordLast = (m_dwRecordLast + 1) % MAX_SCORE_HISTORY;
		if (m_dwRecordLast == m_dwRecordFirst)
		{
			m_dwRecordLast = m_dwRecordFirst;
			memset(m_GameRecordArrary, 0, sizeof(m_GameRecordArrary));
		}
		return lBankerWinScore;
	}

	SCORE CTableFrameSink::CalculateScore(SCORE & lBankerWinScore, tagServerGameRecord & GameRecord)
	{
		//�����Ƶ�
		uint8 cbPlayerCount = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_PLAYER], m_cbCardCount[INDEX_PLAYER]);
		uint8 cbBankerCount = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_BANKER], m_cbCardCount[INDEX_BANKER]);

		//ϵͳ��Ӯ
		SCORE lSystemScore = 0;

		//�ƶ����
		uint8 cbWinArea[AREA_MAX] = { 0 };
		uint8 cbWinXian = m_GameLogic.CompareCard(m_cbTableCardArray[INDEX_PLAYER], m_cbTableCardArray[INDEX_BANKER], MAX_COUNT);
		if (cbWinXian)
		{
			cbWinArea[AREA_XIAN] = TRUE;
		}
		else
		{
			cbWinArea[AREA_ZHUANG] = TRUE;
		}
		uint8 cbPointRed = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_PLAYER], MAX_COUNT);
		uint8 cbPointBlack = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_BANKER], MAX_COUNT);
		if (cbPointRed > 2 || cbPointBlack > 2)
		{
			cbWinArea[AREA_PING] = TRUE;
		}
		memcpy(m_cbOpenResult, cbWinArea, sizeof(cbWinArea));

		//��Ϸ��¼
		GameRecord.bPlayer = cbWinArea[AREA_XIAN] == TRUE;
		GameRecord.bBanker = cbWinArea[AREA_ZHUANG] == TRUE;

		//��ҳɼ�
		SCORE lUserLostScore[GAME_PLAYER] = {};
		memset(m_lPlayScore, 0, sizeof(m_lPlayScore));
		memset(m_lUserWinScore, 0, sizeof(m_lUserWinScore));
		memset(m_lUserRevenue, 0, sizeof(m_lUserRevenue));
		memset(lUserLostScore, 0, sizeof(lUserLostScore));

		//����һ��
		int pingPoint = 0;
		int pingType = 0;
		GameRecord.cbCardType = 1;

		if (cbPlayerCount == 7 || cbBankerCount == 7)
		{
			pingPoint = 11;
			pingType = 7;
			GameRecord.cbCardType = 6;
		}
		else if (cbPlayerCount == 6 || cbBankerCount == 6)
		{
			pingPoint = 6;
			pingType = 6;
			GameRecord.cbCardType = 5;
		}
		else if (cbPlayerCount == 5 || cbBankerCount == 5)
		{
			pingPoint = 4;
			pingType = 5;
			GameRecord.cbCardType = 4;
		}
		else if (cbPlayerCount == 4 || cbBankerCount == 4)
		{
			pingPoint = 3;
			pingType = 4;
			GameRecord.cbCardType = 3;
		}
		else if (cbPlayerCount == 3 || cbBankerCount == 3)
		{
			pingPoint = 2;
			pingType = 3;
			GameRecord.cbCardType = 2;
		}
		GameRecord.cbPing = pingType > 0 ? 1 : 0;

		m_cbOpenResult[AREA_MAX] = GameRecord.cbCardType;

		//�������
		for (uint16 wChairID = 0; wChairID < GAME_PLAYER; ++wChairID)
		{
			//ׯ���ж�
			if (m_wCurrentBanker == wChairID) continue;

			//��ȡ�û�
			IRoomUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIServerUserItem == nullptr) continue;

			for (uint16 wAreaIndex = 0; wAreaIndex < AREA_MAX; ++wAreaIndex)
			{

				if (cbWinArea[wAreaIndex] == TRUE)
				{
					if (wAreaIndex == AREA_XIAN || wAreaIndex == AREA_ZHUANG)
					{
						m_lUserWinScore[wChairID] += m_lPlayBet[wChairID][wAreaIndex];
						m_lPlayScore[wChairID][wAreaIndex] += m_lPlayBet[wChairID][wAreaIndex];
						lBankerWinScore -= m_lPlayBet[wChairID][wAreaIndex];
					}
					else
					{
						m_lUserWinScore[wChairID] += (m_lPlayBet[wChairID][wAreaIndex] * (pingPoint - 1));
						m_lPlayScore[wChairID][wAreaIndex] += (m_lPlayBet[wChairID][wAreaIndex] * (pingPoint - 1));
						lBankerWinScore -= (m_lPlayBet[wChairID][wAreaIndex] * (pingPoint - 1));
					}
				}
				else
				{

					lUserLostScore[wChairID] -= m_lPlayBet[wChairID][wAreaIndex];
					m_lPlayScore[wChairID][wAreaIndex] -= m_lPlayBet[wChairID][wAreaIndex];
					lBankerWinScore += m_lPlayBet[wChairID][wAreaIndex];
				}
			}

			//�ܵķ���
			m_lUserWinScore[wChairID] += lUserLostScore[wChairID];

			//����˰��
			if (m_lUserWinScore[wChairID] > 0)
			{
				float fRevenuePer = float(m_pGameServiceOption->wRevenueRatio / 1000.0);
				m_lUserRevenue[wChairID] = SCORE(m_lUserWinScore[wChairID] * fRevenuePer);
				m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
			}
		}
		return lSystemScore;
	}

	void CTableFrameSink::DispatchTableCard()
	{
		//����˿�
		m_GameLogic.RandCardList(m_cbTableCardArray[0], sizeof(m_cbTableCardArray) / sizeof(m_cbTableCardArray[0][0]));

		//�״η���
		m_cbCardCount[INDEX_PLAYER] = MAX_COUNT;
		m_cbCardCount[INDEX_BANKER] = MAX_COUNT;

		//�������
		uint8 cbBankerCount = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_BANKER], m_cbCardCount[INDEX_BANKER]);
		uint8 cbPlayerTwoCardCount = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_PLAYER], m_cbCardCount[INDEX_PLAYER]);
		return;
	}

	void CTableFrameSink::ReadConfigInformation()
	{
		using namespace LogComm;
		using namespace Util;

		std::string strConfigFile = Util::StringFormat("RedBlack_%s.ini", m_pGameServiceOption->wServerID);
		sConfigMgr->Parse(strConfigFile.c_str());

		for (int i = 0; i < MAX_CHIP_COUNT; ++i)
		{
			std::string strChip = Util::StringFormat("nChip%d", i);
			m_nChip[i] = sConfigMgr->GetInt32("RedBlack", strChip, 0);
		}
		
		m_lUserLimitScore = sConfigMgr->GetInt32("RedBlack", "nLimitMax", 20000);

		//��ׯ
		m_lApplyBankerCondition = sConfigMgr->GetInt32("RedBlack", "ApplyBankerCondition", 0);
		m_nBankerTimeLimit = sConfigMgr->GetInt32("RedBlack", "BankerTimeLimit", 0);
		m_nBankerTimeAdd = sConfigMgr->GetInt32("RedBlack", "BankerTimeAdd", 0);
		m_lExtraBankerScore = sConfigMgr->GetInt32("RedBlack", "ExtraBankerScore", 0);
		m_nExtraBankerTime = sConfigMgr->GetInt32("RedBlack", "ExtraBankerTime", 0);
		m_bEnableSysBanker = sConfigMgr->GetInt32("RedBlack", "EnableSysBanker", 0)?true:false;

		//ʱ��	
		m_cbFreeTime = sConfigMgr->GetInt32("RedBlack", "FreeTime", 0);
		m_cbBetTime = sConfigMgr->GetInt32("RedBlack", "BetTime", 0);
		m_cbEndTime = sConfigMgr->GetInt32("RedBlack", "EndTime", 0);
		if (m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99) m_cbFreeTime = TIME_FREE;
		if (m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99) m_cbBetTime = TIME_PLACE_JETTON;
		if (m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99) m_cbEndTime = TIME_GAME_END;

		//��ע
		m_lAreaLimitScore = sConfigMgr->GetInt32("RedBlack", "BetAreaLimitScore", 0);
	}
	SCORE CTableFrameSink::GetMaxPlayerScore(uint8 cbBetArea, uint16 wChairID)
	{
		//��ȡ���
		IRoomUserItem *pIMeServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (nullptr == pIMeServerUserItem)
		{
			return 0L;
		}

		if (cbBetArea >= AREA_MAX)
		{
			return 0L;
		}
		//����ע��
		SCORE lNowBet = 0l;
		for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex)
		{
			lNowBet += m_lPlayBet[wChairID][nAreaIndex];
		}

		//�����ע
		SCORE lMaxBet = 0L;

		//�����ע
		lMaxBet = __min(pIMeServerUserItem->GetUserScore() - lNowBet, m_lUserLimitScore - m_lPlayBet[wChairID][cbBetArea]);
		lMaxBet = __min(lMaxBet, m_lAreaLimitScore - m_lAllBet[cbBetArea]);

		//��������
		assert(lMaxBet >= 0);
		lMaxBet = __max(lMaxBet, 0);

		return lMaxBet;
	}
	void CTableFrameSink::SendGameRecord(IRoomUserItem * pIServerUserItem)
	{
		uint16 wBufferSize = 0;
		uint8 cbBuffer[SOCKET_TCP_BUFFER] = {};
		int nIndex = m_dwRecordFirst;
		while (nIndex != m_dwRecordLast)
		{
			if ((wBufferSize + sizeof(tagServerGameRecord)) > sizeof(cbBuffer))
			{
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_SEND_RECORD, cbBuffer, wBufferSize);
				wBufferSize = 0;
			}
			memcpy(cbBuffer + wBufferSize, &m_GameRecordArrary[nIndex], sizeof(tagServerGameRecord));
			wBufferSize += sizeof(tagServerGameRecord);

			nIndex = (nIndex + 1) % MAX_SCORE_HISTORY;
		}
		if (wBufferSize > 0) m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_SEND_RECORD, cbBuffer, wBufferSize);
	}
}