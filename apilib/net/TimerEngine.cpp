#include "TimerEngine.h"
#include "Timer.h"
#include <chrono>

namespace Net
{
#define IS_TE_RUN \
	assert(m_pThread);	\
	if (!m_pThread) return false; 

#define IS_TE_STOP \
	assert(!m_pThread);	\
	if (m_pThread) return false; 
	
	CTimerEngine::CTimerEngine():
		m_ioContext(new Net::IOContext()),
		m_dwTimePass(0),
		m_dwTimeLeave(uint32(-1)),
		m_dwTimerSpace(30),
		m_dwShortTime(uint32(-1)),
		m_pThread(nullptr),
		m_updateTimer(*m_ioContext),
		m_dwCount(0),
		m_iCount(0)
	{

	}

	CTimerEngine::~CTimerEngine()
	{
		Stop();
	}

	void CTimerEngine::Release()
	{
		delete this;
	}

	void * CTimerEngine::QueryInterface(GGUID uuid)
	{
		QUERY_INTERFACE(ITimerEngine, uuid);
		QUERY_INTERFACE(IServiceModule, uuid);
		QUERY_INTERFACE_IUNKNOWNEX(ITimerEngine, uuid);
		return nullptr;
	}

	bool CTimerEngine::Start(Net::IOContext *)
	{
		IS_TE_STOP
		if (m_pThread = new std::thread(&CTimerEngine::Run, this), m_pThread == nullptr)
		{
			assert(nullptr);
			return false;
		}

		if (m_pThreadLoop = new std::thread(&CTimerEngine::Loop, this), m_pThreadLoop == nullptr)
		{
			assert(nullptr);
			return false;
		}
		return true;
	}

	bool CTimerEngine::Stop()
	{
		IS_TE_RUN
		m_ioContext->stop();
		if (m_pThread)
		{
			m_pThread->join();
			PDELETE(m_pThread);
		}
		return true;
	}

	bool CTimerEngine::SetTimer(uint32 dwTimerID, uint32 dwElapse, uint32 dwRepeat)
	{
		std::lock_guard<std::mutex> _lock(m_mutex);
		try
		{
			dwElapse = (dwElapse + m_dwTimerSpace - 1) / m_dwTimerSpace * m_dwTimerSpace;

			tagTimerItem * pTimerItem = nullptr;
			bool bTimerExist = false;

			if (m_TimerItemActive.find(dwTimerID) != m_TimerItemActive.end())
			{
				bTimerExist = true;
			}

			if (!bTimerExist)
			{
				if (!m_TimerItemFree.empty())
				{
					pTimerItem = m_TimerItemFree.back();
					assert(pTimerItem != nullptr);
					m_TimerItemFree.pop_back();
				}
				else
				{
					try
					{
						pTimerItem = new tagTimerItem;
						assert(pTimerItem != nullptr);
						if (pTimerItem == nullptr)
						{
							return false;
						}
					}
					catch (...)
					{
						return false;
					}
				}
			}

			//���ò���
			assert(pTimerItem != nullptr);
			pTimerItem->dwElapse = dwElapse;
			pTimerItem->dwTimerID = dwTimerID;
			pTimerItem->dwRepeatTimes = dwRepeat;
			pTimerItem->dwTimeLeave = dwElapse + m_dwTimePass;

			//���ʱ��
			m_dwTimeLeave = __min(m_dwTimeLeave, dwElapse);
			if (!bTimerExist)
			{
				m_TimerItemActive[dwTimerID] = pTimerItem;
			}
		}
		catch (...)
		{
		}
		return true;
	}

	bool CTimerEngine::KillTimer(uint32 dwTimerID)
	{
		return false;
	}

	bool CTimerEngine::KillAllTimer()
	{
		return false;
	}

	void CTimerEngine::Run()
	{
		asio::io_context::work work(*m_ioContext);
		m_ioContext->run();
	}

	void CTimerEngine::Loop()
	{
		//��ʱ����
		m_dwLastTickCount = DB::getMSTime();

		OnTimerThreadSink();

		///////////////////////////////////////////////////
		//��ȡʱ��
		uint32 dwTimerSpace = m_dwTimerSpace;
		uint32 dwNowTickCount = DB::getMSTime();

		//�ȴ�����
		if ((m_dwLastTickCount != 0L) && (dwNowTickCount > m_dwLastTickCount))
		{
			uint32 dwHandleTickCount = dwNowTickCount - m_dwLastTickCount;
			dwTimerSpace = (dwTimerSpace > dwHandleTickCount) ? (dwTimerSpace - dwHandleTickCount) : 0L;
		}

		//ʱ�䴦��
		m_updateTimer.expires_from_now(std::chrono::milliseconds(dwTimerSpace));
		m_updateTimer.async_wait(std::bind(&CTimerEngine::Loop, this));
	}

	void CTimerEngine::OnTimerThreadSink()
	{
		if (m_mutex.try_lock())
		{
			try
			{
				//����ʱ��
				if (m_TimerItemActive.empty())
				{
					m_mutex.unlock();
					return;
				}

				//����ʱ��
				m_dwTimePass += m_dwTimerSpace;
				m_dwTimeLeave -= m_dwTimerSpace;

				//��ѯ��ʱ��
				if (m_dwTimeLeave == 0)
				{
					//��������
					bool bKillTimer = false;
					uint32 dwTimeLeave = NO_TIME_LEAVE;

					//ʱ������
					for (CTimerItemStore::iterator it = m_TimerItemActive.begin(); it != m_TimerItemActive.end(); )
					{
						
						assert(it->second->dwTimeLeave >= m_dwTimePass);

						//���ñ���
						bKillTimer = false;
						it->second->dwTimeLeave -= m_dwTimePass;

						//֪ͨ�ж�
						if (it->second->dwTimeLeave == 0L)
						{
							//����֪ͨ
							assert(m_pITimerEngineEvent != NULL);
							m_pITimerEngineEvent->OnEventTimer(it->second->dwTimerID);
							
							//���ô���
							if (it->second->dwRepeatTimes != TIMES_INFINITY)
							{
								if (it->second->dwRepeatTimes == 1L)
								{
									bKillTimer = true;
									m_TimerItemFree.emplace_back(it->second);
									m_TimerItemActive.erase(it++);
								}
								else it->second->dwRepeatTimes--;
							}

							//����ʱ��
							if (!bKillTimer) it->second->dwTimeLeave = it->second->dwElapse;
						}

						//��������
						if (!bKillTimer)
						{
							dwTimeLeave = __min(dwTimeLeave, it->second->dwTimeLeave);
							assert(dwTimeLeave%m_dwTimerSpace == 0);
							++it;
						}
					}
					//������Ӧ
					m_dwTimePass = 0L;
					m_dwTimeLeave = dwTimeLeave;
				}
				m_mutex.unlock();
			}
			catch (...)
			{
				m_mutex.unlock();
			}
		}
		return;
	}

	bool CTimerEngine::SetTimerEngineEvent(IUnknownEx * pIUnknownEx)
	{
		IS_TE_STOP
		//���ýӿ�
		if (pIUnknownEx != nullptr)
		{
			//��ѯ�ӿ�
			assert(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITimerEngineEvent) != nullptr);
			m_pITimerEngineEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITimerEngineEvent);

			//�ɹ��ж�
			if (m_pITimerEngineEvent == nullptr) return false;
		}
		else m_pITimerEngineEvent = nullptr;

		return true;
	}

	DECLARE_CREATE_MODULE(TimerEngine);
}