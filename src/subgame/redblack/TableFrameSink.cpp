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
		//����״̬
		m_pITableFrame->SetGameStatus(GAME_SCENE_BET);

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

		//������ʱ
		m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
		m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, m_cbBetTime * 1000, 1);
		return true;
	}

	bool CTableFrameSink::OnEventGameConclude(uint16 wChairID, IRoomUserItem * pIServerUserItem, uint8 cbReason)
	{
		switch (cbReason)
		{
			case GER_NORMAL:		//�������	
			{
				//�������
				uint64 lBankerWinScore = GameOver();

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

					uint64 lAddScore = 0;
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
			case GAME_SCENE_BET_FINISH:
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

		pRoomStatus->tagGameInfo.wTableID = 1;
		pRoomStatus->tagGameInfo.cbGameStatus = 0;

		pRoomStatus->tagTimeInfo.cbBetTime = 15;
		pRoomStatus->tagTimeInfo.cbEndTime = 15;
		pRoomStatus->tagTimeInfo.cbPassTime = (uint32)time(nullptr) - 0;
		pRoomStatus->tagTimeInfo.lMinXianHong = 5;
		pRoomStatus->tagTimeInfo.lMaxXianHong = 10;

		//�ͻ���ֻ��ʾ48��
		int nIndex = 0;

		pRoomStatus->cbRecordCount = 0;
		int nArrayIndex = 0;

		//while (nIndex != m_nRecordLast)
		//{
		//	if (nArrayIndex >= 48)
		//		break;
		//	pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[0] = m_GameRecordArrary[nIndex].bPlayer;
		//	pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[1] = m_GameRecordArrary[nIndex].bBanker;
		//	pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[2] = m_GameRecordArrary[nIndex].bPing;
		//	pRoomStatus->GameRecordArrary[nArrayIndex].cbAreaWin[3] = m_GameRecordArrary[nIndex].cbCardType;

		//	nArrayIndex++;
		//	nIndex = (nIndex + 1) % MAX_SCORE_HISTORY;
		//}
	}
	bool CTableFrameSink::OnGameMessage(uint16 wSubCmdID, void * pData, uint16 wDataSize, IRoomUserItem * pIServerUserItem)
	{
		switch (wSubCmdID)
		{
			case SUB_C_PLACE_JETTON:		//�û���ע
			{
				return true;
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
					m_pITableFrame->SetGameStatus(GAME_SCENE_BET_FINISH);

					//����ʱ��
					m_dwBetTime = (uint32)time(nullptr);
					m_pITableFrame->SetGameTimer(IDI_GAME_END, m_cbEndTime * 1000, 1);
				}
				return true;
			}
			case IDI_GAME_END:
			{
				if (m_pITableFrame->GetGameStatus() != GAME_SCENE_BET_FINISH)
				{
					//����״̬
					m_pITableFrame->SetGameStatus(GAME_SCENE_END);

					//������Ϸ
					OnEventGameConclude(INVALID_CHAIR, nullptr, GER_NORMAL);

					//������Ϸ
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

					//��ʼ��Ϸ
					m_pITableFrame->StartGame();
				}
			}
		}
		return false;
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

			uint64 lAddScore = 0;
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
			swprintf(UserListInfo[cbListUserCount].szNickName, sizeof(UserListInfo[cbListUserCount].szNickName), L"%s", wstrNickName.c_str());
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

	uint32 CTableFrameSink::GameOver()
	{
		//�������
		uint64 lBankerWinScore = 0;
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

	uint64 CTableFrameSink::CalculateScore(uint64 & lBankerWinScore, tagServerGameRecord & GameRecord)
	{
		//�����Ƶ�
		uint8 cbPlayerCount = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_PLAYER], m_cbCardCount[INDEX_PLAYER]);
		uint8 cbBankerCount = m_GameLogic.GetCardType(m_cbTableCardArray[INDEX_BANKER], m_cbCardCount[INDEX_BANKER]);

		//ϵͳ��Ӯ
		uint64 lSystemScore = 0;

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
		uint64 lUserLostScore[GAME_PLAYER] = {};
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
				m_lUserRevenue[wChairID] = uint64(m_lUserWinScore[wChairID] * fRevenuePer);
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
}