#ifndef TIMER_ENGINE_H
#define TIMER_ENGINE_H

#include "KernelEngineHead.h"
#include "Strand.h"
#include <map>
#include <thread>
#include <mutex>

namespace Net
{
#define NO_TIME_LEAVE	uint32(-1)
#define TIMES_INFINITY	NO_TIME_LEAVE

	//��ʱ������
	struct tagTimerItem
	{
		//ʱ�����
		uint32							dwElapse;							//����ʱ��
		uint32							dwTimerID;							//ʱ���ʶ
		uint32							dwTimeLeave;						//ʣ��ʱ��
		uint32							dwRepeatTimes;						//�ظ�����
	};

	typedef std::vector<tagTimerItem*>		CTimerItemArray;
	typedef std::map<uint32, tagTimerItem*>	CTimerItemStore;

	class CTimerEngine : public ITimerEngine
	{
		//��������
	public:
		//���캯��
		CTimerEngine();
		//��������
		virtual ~CTimerEngine();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release();
		//�ӿڲ�ѯ
		virtual void *QueryInterface(GGUID uuid);
		
		//����ӿ�
	public:
		//��������
		virtual bool Start(Net::IOContext*);
		//ֹͣ����
		virtual bool Stop();

		//���ܽӿ�
	public:
		//���ö�ʱ��
		virtual bool SetTimer(uint32 dwTimerID, uint32 dwElapse, uint32 dwRepeat);
		//ɾ����ʱ��
		virtual bool KillTimer(uint32 dwTimerID);
		//ɾ����ʱ��
		virtual bool KillAllTimer();

		//�ڲ�����
	private:
		void Run();

		void Loop();

		//�ڲ�����
	private:
		//��ʱ��֪ͨ
		void OnTimerThreadSink();

		//���ýӿ�
	public:
		//���ýӿ�
		virtual bool SetTimerEngineEvent(IUnknownEx * pIUnknownEx);

	protected:
		uint32								m_dwCount;
		uint32								m_dwTimerSpace;
		uint32								m_dwTimePass;						//����ʱ��
		uint32								m_dwTimeLeave;
		uint32								m_dwLastTickCount;
		uint32								m_dwShortTime;

		ITimerEngineEvent *					m_pITimerEngineEvent;				//�¼��ӿ�
		CTimerItemArray						m_TimerItemFree;					//��������
		CTimerItemStore						m_TimerItemActive;					//��������

	protected:
		std::shared_ptr<Net::IOContext> 	m_ioContext;
		int									m_iCount;
		std::mutex							m_mutex;
		std::thread*						m_pThread;
		std::thread*						m_pThreadLoop;
		asio::steady_timer					m_updateTimer;
	};
}

#endif