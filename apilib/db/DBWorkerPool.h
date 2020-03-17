#ifndef DB_ENGINE_H
#define DB_ENGINE_H

#include "Define.h"
#include "DBEnvHeader.h"
#include <functional>
#include <array>
#include <string>
#include <vector>
#include <queue>
#include <stack>

namespace Util
{
	template<typename T>
	class ProducerConsumerQueue;
}

namespace DB
{
	class QueryCallback;
	class SQLOperation;
	class MySQLConnection;
	struct MySQLConnectionInfo;

	template<typename T>
	class DBWorkerPool
	{
	public:
		DBWorkerPool();
		virtual ~DBWorkerPool();

	private:
		enum InternalIndex
		{
			IDX_ASYNC,
			IDX_SYNCH,
			IDX_SIZE
		};

		//�����ӿ�
	public:
		bool Start(DBWorkerPool<T> &pool);

	private:
		using Predicate = std::function<bool()>;
		using Closer = std::function<void()>;

		bool Load();
		bool Process(std::queue<Predicate>& queue);

		uint32 const m_updateFlags;
		EXPORT_BEGIN
		std::queue<Predicate> m_open, m_populate, m_update, m_prepare;
		std::stack<Closer> m_close;
		EXPORT_END
		
		//////////////////////////////////////////////////////////////////
		//��ѯ����
	public:
		//��ȡԤ��
		typedef typename T::Statements PreparedStatementIndex;
		PreparedStatement* GetPreparedStatement(PreparedStatementIndex index);
		
		//ͬ����ѯ
		QueryResult Query(char const* sql, T* connection = nullptr);

		//ͬ����ѯ
		PreparedQueryResult Query(PreparedStatement* stmt);

		void DirectExecute(char const* sql);
		
		//�첽��ѯ
		QueryCallback AsyncQuery(PreparedStatement* stmt);

		inline MySQLConnectionInfo const* GetConnectionInfo() const
		{
			return m_connectionInfo.get();
		}

	private:
		//������Ϣ
		void SetConnectionInfo(std::string const& infoString, uint8 const asyncThreads, uint8 const synchThreads);

		//�������ݿ�
		uint32 Open();

		//�������
		void Close();

		//Ԥ����ָ��
		bool PrepareStatements();

		//��������
		uint32 OpenConnections(InternalIndex type, uint8 numConnections);

		//��������
		T* GetFreeConnection();

		//���ݿ���
		char const* GetDatabaseName() const;

		uint8	m_asyncThreads, m_synchThreads;
		EXPORT_BEGIN
		std::unique_ptr<Util::ProducerConsumerQueue<SQLOperation*>>			m_queue;
		std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE>				m_connections;
		std::unique_ptr<MySQLConnectionInfo>								m_connectionInfo;
		std::vector<uint8>													m_preparedStatementSize;
		EXPORT_END
	};
}

#endif