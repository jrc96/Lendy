#include "DataQueue.h"
#include <cstring>
#include <algorithm>
#include <assert.h>

namespace Util
{
	DataQueue::DataQueue() : 
		m_dwInsertPos(0),
		m_dwTerminalPos(0),
		m_dwDataQueryPos(0),
		m_dwDataSize(0),
		m_dwDataPacketCount(0),
		m_dwBufferSize(0),
		m_pDataQueueBuffer(nullptr)
	{
	}

	DataQueue::~DataQueue()
	{
		PDELETE(m_pDataQueueBuffer);
	}

	bool DataQueue::InsertData(uint16 wIdentifier, void * pBuffer, uint16 wDataSize)
	{
		//��������
		tagDataHead DataHead;
		memset(&DataHead, 0, sizeof(DataHead));

		//���ñ���
		DataHead.wDataSize = wDataSize;
		DataHead.wIdentifier = wIdentifier;

		//�����洢
		if (RectifyBuffer(sizeof(DataHead) + DataHead.wDataSize) == false)
		{
			assert(nullptr);
			return false;
		}

		//��������
		try
		{
			memcpy(m_pDataQueueBuffer + m_dwInsertPos, &DataHead, sizeof(DataHead));
			
			//��������
			if (wDataSize > 0)
			{
				assert(pBuffer != NULL);
				memcpy(m_pDataQueueBuffer + m_dwInsertPos + sizeof(DataHead), pBuffer, wDataSize);
			}

			//��������
			++m_dwDataPacketCount;
			m_dwInsertPos += sizeof(DataHead) + wDataSize;
			m_dwDataSize += sizeof(DataHead) + wDataSize;
			m_dwTerminalPos = __max(m_dwTerminalPos, m_dwInsertPos); 
			return true;
		}
		catch (...)
		{
			assert(pBuffer != NULL);
			return false;
		}
		return false;
	}

	bool DataQueue::DistillData(tagDataHead & DataHead, void * pBuffer, uint16 wBufferSize)
	{
		//Ч�����
		assert(m_dwDataSize > 0L);
		assert(m_dwDataPacketCount > 0);
		assert(m_pDataQueueBuffer != nullptr);

		//Ч�����
		if (m_dwDataSize == 0) return false;
		if (m_dwDataPacketCount == 0) return false;

		//��������
		if (m_dwDataQueryPos == m_dwTerminalPos)
		{
			m_dwDataQueryPos = 0L;
			m_dwTerminalPos = m_dwInsertPos;
		}

		assert(m_dwBufferSize >= (m_dwDataQueryPos + sizeof(tagDataHead)));
		tagDataHead * pDataHead = (tagDataHead *)(m_pDataQueueBuffer + m_dwDataQueryPos);
		assert(wBufferSize >= pDataHead->wDataSize);

		//��ȡ��С
		uint16 wPacketSize = sizeof(DataHead) + pDataHead->wDataSize;
		assert(m_dwBufferSize >= (m_dwDataQueryPos + wPacketSize));

		//�жϻ���
		uint16 wCopySize = 0;
		assert(wBufferSize >= pDataHead->wDataSize);
		if (wBufferSize >= pDataHead->wDataSize) wCopySize = pDataHead->wDataSize;

		//��������
		DataHead = *pDataHead;
		if (DataHead.wDataSize > 0)
		{
			if (wBufferSize < pDataHead->wDataSize) DataHead.wDataSize = 0;
			else memcpy(pBuffer, pDataHead + 1, DataHead.wDataSize);
		}

		//Ч�����
		assert(wPacketSize <= m_dwDataSize);
		assert(m_dwBufferSize >= (m_dwDataQueryPos + wPacketSize));

		//���ñ���
		--m_dwDataPacketCount;
		m_dwDataSize -= wPacketSize;
		m_dwDataQueryPos += wPacketSize;
		return true;
	}

	bool DataQueue::RectifyBuffer(uint64 dwNeedSize)
	{
		//				0			    m_dwInsert				  m_dwQueueSize		
		//				|				     |		        		     |
		//				|____________________|___________________________|
		//				|	/	/	/	/	/	/	/	/	/	/	/   /|
		//				|__/___/___/___/___/___/___/___/___/___/___/___/_|
		//								|					|			
		//							m_dwQuery			m_dwQuery
		///////////////////////////////////////////////////////////////////////////
		bool bAlloc = false;

		//�����ж�
		if ((m_dwDataSize + dwNeedSize) > m_dwBufferSize) bAlloc = true;

		//���¿�ʼ
		if ((m_dwInsertPos == m_dwTerminalPos) && ((m_dwInsertPos + dwNeedSize) > m_dwBufferSize))
		{
			if (m_dwDataQueryPos >= dwNeedSize)
			{
				m_dwInsertPos = 0;
				if (m_dwDataQueryPos == m_dwTerminalPos && m_dwDataPacketCount > 0)
				{
					m_dwDataQueryPos = 0L;
					bAlloc = true;
				}
			}
			else bAlloc = true;
		}

		//�����ж�
		if ((m_dwInsertPos < m_dwTerminalPos) && ((m_dwInsertPos + dwNeedSize) > m_dwDataQueryPos)) bAlloc = true;
		////////////////////////////////////////////////////  
		//ͷ׷��β��β׷��ͷ  
		if (m_dwInsertPos + dwNeedSize > m_dwDataQueryPos && m_dwDataQueryPos >= m_dwInsertPos)
		{
			//β׷��ͷ  
			if (m_dwDataSize > 0) bAlloc = true;
		}

		try
		{
			if (bAlloc)
			{
				//�����ڴ�
				uint64 dwReSize = __max(m_dwBufferSize / 2L, dwNeedSize * 10L);
				uint8* pNewQueueServiceBuffer = new uint8[m_dwBufferSize + dwReSize];

				//�����ж�
				assert(pNewQueueServiceBuffer != nullptr);
				if (pNewQueueServiceBuffer == nullptr) return false;

				//��������
				uint64 dwRemainSize = 0;
				if (m_pDataQueueBuffer != nullptr)
				{
					//Ч��״̬
					assert(m_dwTerminalPos >= m_dwDataSize);
					assert(m_dwTerminalPos >= m_dwDataQueryPos);

					//��������
					uint32 dwPartOneSize = m_dwTerminalPos - m_dwDataQueryPos;
					if (dwPartOneSize > 0L) memcpy(pNewQueueServiceBuffer, m_pDataQueueBuffer + m_dwDataQueryPos, dwPartOneSize);
					if (m_dwDataSize > dwPartOneSize)
					{
						assert((m_dwInsertPos + dwPartOneSize) == m_dwDataSize);
						memcpy(pNewQueueServiceBuffer + dwPartOneSize, m_pDataQueueBuffer, m_dwInsertPos);
					}
				}

				//���ñ���
				m_dwDataQueryPos = 0L;
				m_dwInsertPos = m_dwDataSize;
				m_dwTerminalPos = m_dwDataSize;
				m_dwBufferSize = m_dwBufferSize + dwReSize;

				//���û���
				PDELETE(m_pDataQueueBuffer);
				m_pDataQueueBuffer = pNewQueueServiceBuffer;
			}
		}
		catch (...) { return false; }


		//try
		//{
		//	//
		//	if (m_dwInsert > m_IndexQuery.dwIndex)
		//	{
		//		if (m_dwInsert + dwNeedSize > m_dwQueueSize)
		//		{
		//			if (m_IndexQuery.dwIndex > dwNeedSize)
		//			{
		//				m_dwInsert = 0;
		//			}
		//			else
		//			{
		//				throw 0;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		if (m_dwInsert + dwNeedSize > m_IndexQuery.dwIndex)
		//		{
		//			throw 0;
		//		}
		//	}
		//}
		//catch (...)
		//{
		//	try
		//	{
		//		//�����ڴ�
		//		uint64 dwReSize = __max(m_dwQueueSize / 2L, dwNeedSize * 10L);
		//		uint8* pNewQueueBuffer = new uint8[m_dwQueueSize + dwReSize];

		//		//�����ж�
		//		assert(pNewQueueBuffer != nullptr);
		//		if (pNewQueueBuffer == nullptr) return false;

		//		//��������
		//		uint64 dwRemainSize = 0;
		//		if (m_pQueueBuffer != NULL)
		//		{
		//			dwRemainSize = m_dwQueueSize - m_IndexQuery.dwIndex;
		//			memcpy(pNewQueueBuffer, m_pQueueBuffer + m_IndexQuery.dwIndex, m_dwQueueSize - m_IndexQuery.dwIndex);
		//			if (m_dwInsert < m_IndexQuery.dwIndex)
		//			{
		//				memcpy(pNewQueueBuffer + m_dwQueueSize - m_IndexQuery.dwIndex, m_pQueueBuffer, m_dwInsert);
		//				dwRemainSize += m_dwInsert;
		//			}
		//		}

		//		//���ñ���
		//		memset(&m_IndexQuery, 0, sizeof(m_IndexQuery));
		//		m_dwInsert = dwRemainSize;
		//		m_dwQueueSize = m_dwQueueSize + dwReSize;

		//		uint16 uLastSize = 0;
		//		std::deque<tagDataIndex> d;
		//		for (std::deque<tagDataIndex>::iterator it = m_deque.begin(); it != m_deque.end(); ++it)
		//		{
		//			if (it->dwIndex > m_deque.begin()->dwIndex)
		//			{
		//				it->dwIndex -= m_deque.begin()->dwIndex;
		//				uLastSize = it->dwIndex + it->wDataSize;
		//			}
		//			else
		//			{
		//				it->dwIndex += uLastSize;
		//			}
		//			d.emplace_back(*it);
		//		}
		//		m_deque.assign(d.begin(),d.end());

		//		//���û���
		//		PDELETE(m_pQueueBuffer);
		//		m_pQueueBuffer = pNewQueueBuffer;
		//	}
		//	catch (...) { return false; }
		//}
		return true;
	}
}