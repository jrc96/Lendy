#include "DBUpdater.h"
#include "DBWorkerPool.h"
#include "MySQLConnection.h"
#include "Implementation/LogonDatabase.h"

#include "Log.h"
#include "INIReader.h"
#include "StringUtility.h"
#include "SHA1.h"
#include "GitVision.h"

#include "Field.h"
#include "QueryResult.h"

#include <sstream>
#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
#include <Windows.h>
#else

#endif
#include <iostream>
#include <fstream>
#include <filesystem>
#include <io.h>

namespace DB
{
	using namespace LogComm;


	template<typename Fn>
	static std::string GetStringWithDefaultValueFromFunction(
		std::string const& key, Fn getter)
	{
		std::string const value = sConfigMgr->Get("DB", key, "");
		return value.empty() ? getter() : value;
	}

	template<class T>
	inline bool DBUpdater<T>::Create(DBWorkerPool<T>& pool)
	{
		LOG_INFO("sql.updates", "���ݿ� \"%s\" ������, �Ƿ���д���? [yes (default) / no]: ",
			pool.GetConnectionInfo()->database.c_str());

		std::string answer;
		std::getline(std::cin, answer);
		if (!answer.empty() && !(answer.substr(0, 1) == "y"))
		{
			return false;
		}

		LOG_INFO("sql.updates", "���ڴ������ݿ� \"%s\"...", pool.GetConnectionInfo()->database.c_str());
		
		static Path const temp("create_table.sql");

		std::ofstream file(temp.generic_string());
		if (!file.is_open())
		{
			LOG_FATAL("sql.updates", "Failed to create temporary query file \"%s\"!", temp.generic_string().c_str());
			return false;
		}

		file << "CREATE DATABASE `" << pool.GetConnectionInfo()->database << "` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci\n\n";

		file.close();

		try
		{
			DBUpdater<T>::ApplyFile(pool, pool.GetConnectionInfo()->host, pool.GetConnectionInfo()->user, pool.GetConnectionInfo()->password,
				pool.GetConnectionInfo()->portsocket, "", temp);
		}
		catch (...)
		{
			LOG_FATAL("sql.updates", "Failed to create database %s! Does the user (named in *.conf) have `CREATE`, `ALTER`, `DROP`, `INSERT` and `DELETE` privileges on the MySQL server?", pool.GetConnectionInfo()->database.c_str());
			std::tr2::sys::remove(temp);
			return false;
		}

		LOG_INFO("sql.updates", "Done.");
		std::tr2::sys::remove(temp);
		return true;
	}

	template<class T>
	bool DBUpdater<T>::Populate(DBWorkerPool<T>& pool)
	{
		QueryResult const result = Retrieve(pool, "SHOW TABLES");
		if (result && (result->GetRowCount() > 0))
		{
			return true;
		}

		if (!CheckExecutable())
		{
			return false;
		}

		LOG_INFO("sql.updates", "���ݿ� %s Ϊ��, �Զ�Ǩ��...", DBUpdater<T>::GetTableName().c_str());

		std::string const p = DBUpdater<T>::GetBaseFile();
		if (p.empty())
		{
			LOG_INFO("sql.updates", ">> û�л����ļ��ṩ, ����!");
			return true;
		}

		Path const base(p);
		if (!exists(p))
		{
			switch (DBUpdater<T>::GetBaseLocationType())
			{
				case LOCATION_REPOSITORY:
				{
					LOG_ERROR("sql.updates", ">> �����ļ� \"%s\" ȱʧ.",
						base.generic_string().c_str());
					break;
				}
				case LOCATION_DOWNLOAD:
				{
					std::string const filename = base.filename().generic_string();
					LOG_ERROR("sql.updates", ">> �ļ� \"%s\" ȱʧ.", filename.c_str());
					break;
				}
			}
			return false;
		}

		LOG_INFO("sql.updates", ">> ����Ӧ�� \'%s\'...", base.generic_string().c_str());

		try
		{
			ApplyFile(pool, base);
		}
		catch (...)
		{
			return false;
		}
		LOG_INFO("sql.updates", ">> ���!");

		return true;
	}

	template<class T>
	bool DBUpdater<T>::Update(DBWorkerPool<T>& pool)
	{
		if (!CheckExecutable())
		{
			return false;
		}

		LOG_INFO("sql.updates", "���ڸ��� %s ���ݿ�...", DBUpdater<T>::GetTableName().c_str());

		Path const sourceDirectory(GetSourceDirectory());
		if (!is_directory(sourceDirectory))
		{
			LOG_ERROR("sql.updates", "����Ŀ¼ %s ������, ����ָ��sqlĿ¼.", sourceDirectory.generic_string().c_str());
			return false;
		}

		UpdateFetcher updateFetcher(sourceDirectory, [&](std::string const& query) { DBUpdater<T>::Apply(pool, query); },
			[&](Path const& file) { DBUpdater<T>::ApplyFile(pool, file); },
			[&](std::string const& query) -> QueryResult { return DBUpdater<T>::Retrieve(pool, query); });

		UpdateResult result;
		try
		{
			result = updateFetcher.Update(
				sConfigMgr->GetBool("DB", "Updates.Redundancy", true),
				sConfigMgr->GetBool("DB", "Updates.AllowRehash", true),
				sConfigMgr->GetBool("DB", "Updates.ArchivedRedundancy", false),
				sConfigMgr->GetInt32("DB", "Updates.CleanDeadRefMaxCount", 3));
		}
		catch (...)
		{
			return false;
		}

		std::string const info = StringFormat("Containing " "%" PRIuPTR " new and " "%" PRIuPTR " archived updates.",
			result.recent, result.archived);

		if (!result.updated)
			LOG_INFO("sql.updates", ">> %s database is up-to-date! %s", DBUpdater<T>::GetTableName().c_str(), info.c_str());
		else
			LOG_INFO("sql.updates", ">> Applied " "%" PRIuPTR " %s. %s", result.updated, result.updated == 1 ? "query" : "queries", info.c_str());

		return true;
	}

	template<>
	std::string DBUpdater<LogonDatabaseConnection>::GetTableName()
	{
		return "Auths";
	}

	template<class T>
	inline std::string DBUpdater<T>::GetSourceDirectory()
	{
		return GetStringWithDefaultValueFromFunction("SourceDirectory", GitVision::GetSourceDirectory);
	}

	template<>
	std::string DBUpdater<LogonDatabaseConnection>::GetBaseFile()
	{
		return GetSourceDirectory() + "/sql/base/auth_database.sql";
	}

	// All
	template<class T>
	BaseLocation DBUpdater<T>::GetBaseLocationType()
	{
		return LOCATION_REPOSITORY;
	}

	template<class T>
	QueryResult DBUpdater<T>::Retrieve(DBWorkerPool<T>& pool, std::string const& query)
	{
		return pool.Query(query.c_str());
	}

	template<class T>
	void DBUpdater<T>::Apply(DBWorkerPool<T>& pool, std::string const& query)
	{
		pool.DirectExecute(query.c_str());
	}

	template<class T>
	void DBUpdater<T>::ApplyFile(DBWorkerPool<T>& pool, Path const & path)
	{
		DBUpdater<T>::ApplyFile(pool, pool.GetConnectionInfo()->host, pool.GetConnectionInfo()->user, pool.GetConnectionInfo()->password,
			pool.GetConnectionInfo()->portsocket, pool.GetConnectionInfo()->database, path);
	}

	template<class T>
	void DBUpdater<T>::ApplyFile(DBWorkerPool<T>& pool, std::string const & host, std::string const & user, std::string const & password, std::string const & portsocket, std::string const & database, Path const & path)
	{
		std::vector<std::string> args;
		args.reserve(8);

		// args[0] represents the program name
		args.push_back("mysql");

		// CLI Client connection info
		args.push_back("-h " + host);
		args.push_back("-u" + user);

		if (!password.empty())
			args.push_back("-p" + password);

#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS

		args.push_back("-P " + portsocket);

#else

		if (!std::isdigit(port_or_socket[0]))
		{
			// We can't check if host == "." here, because it is named localhost if socket option is enabled
			args.push_back("-P0");
			args.push_back("--protocol=SOCKET");
			args.push_back("-S" + port_or_socket);
		}
		else
			// generic case
			args.push_back("-P" + port_or_socket);

#endif

		// Set the default charset to utf8
		args.push_back("--default-character-set=utf8");

		// Set max allowed packet to 1 GB
		args.push_back("--max-allowed-packet=1GB");

		// Database
		if (!database.empty())
			args.push_back(database);

		// Invokes a mysql process which doesn't leak credentials to logs
		int ret = EXIT_SUCCESS;
		
#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
		
		std::ostringstream os;
		os << "cmd.exe /c ";
		for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
		{
			os << it->c_str() << " ";
		}
		os << "< " << path.generic_string();

		PROCESS_INFORMATION pi;
		STARTUPINFO		si;//����ָ���½��̵����������Ե�һ���ṹ
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;//SW_HIDE���ش���

		BOOL result = CreateProcess(NULL, (char*)os.str().c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		if (result)
		{
			DWORD dwExitCode = 0;

			CloseHandle(pi.hThread);

			WaitForSingleObject(pi.hProcess, INFINITE);

			GetExitCodeProcess(pi.hProcess, &dwExitCode);

			CloseHandle(pi.hProcess);

			ret = dwExitCode;
		}
#else

#endif
		if (ret != EXIT_SUCCESS)
		{
			LOG_FATAL("sql.updates", "�����ļ� \'%s\' �����ݿ� \'%s\' ʧ��!",\
				path.generic_string().c_str(), pool.GetConnectionInfo()->database.c_str());
			try
			{
				throw "update failed";
			}
			catch (const std::exception& ec)
			{
				LOG_ERROR("sql.updates", ec.what());
			}
		}
	}

	template<class T>
	bool DBUpdater<T>::CheckExecutable()
	{
		//����mysqlӦ�ó���
		std::string path = ::getenv("PATH");

		std::string result;
		Util::Tokenizer tok(path, ';');
		for (Util::Tokenizer::const_iterator it = tok.begin(); it != tok.end(); ++it)
		{
			Path p = *it;

#if LENDY_PLATFORM == LENDY_PLATFORM_WINDOWS
			p /= "mysql.exe";
#else
			p /= "mysql";
#endif

			if (!::access(p.generic_string().c_str(), 0))  //X_OR
			{
				result = p.string();
				CorrectedPath() = *it;
				break;
			}
		}

		if (result.empty())
		{
			LOG_FATAL("sql.updates", "MySQL ���޷���ѯ, ���������ļ� (\"MySQLExecutable\").");
		}

		return !result.empty();
	}

	template<class T>
	std::string & DBUpdater<T>::CorrectedPath()
	{
		static std::string path;
		return path;
	}

	template class LENDY_COMMON_API DBUpdater<LogonDatabaseConnection>;

	//////////////////////////////������ȡ��//////////////////
	struct UpdateResult
	{
		UpdateResult()
			: updated(0), recent(0), archived(0) { }

		UpdateResult(size_t const updated_, size_t const recent_, size_t const archived_)
			: updated(updated_), recent(recent_), archived(archived_) { }

		size_t updated;
		size_t recent;
		size_t archived;
	};

	struct DirectoryEntry
	{
		DirectoryEntry(Path const& path_, State state_) : path(path_), state(state_) { }

		Path const path;
		State const state;
	};

	struct AppliedFileEntry
	{
		AppliedFileEntry(std::string const& name_, std::string const& hash_, State state_, uint64 timestamp_)
			: name(name_), hash(hash_), state(state_), timestamp(timestamp_) { }

		std::string const name;

		std::string const hash;

		State const state;

		uint64 const timestamp;

		static inline State StateConvert(std::string const& state)
		{
			return (state == "RELEASED") ? RELEASED : ARCHIVED;
		}

		static inline std::string StateConvert(State const state)
		{
			return (state == RELEASED) ? "RELEASED" : "ARCHIVED";
		}

		std::string GetStateAsString() const
		{
			return StateConvert(state);
		}
	};

	UpdateFetcher::UpdateFetcher(Path const & updateDirectory, 
		std::function<void(std::string const&)> const & apply, 
		std::function<void(Path const&path)> const & applyFile, 
		std::function<QueryResult(std::string const&)> const & retrieve):
		_sourceDirectory(std::make_unique<Path>(updateDirectory)),
		_apply(apply), 
		_applyFile(applyFile),
		_retrieve(retrieve)
	{
	}

	UpdateFetcher::~UpdateFetcher()
	{
	}

	UpdateResult UpdateFetcher::Update(bool const redundancyChecks, bool const allowRehash, bool const archivedRedundancy, int32 const cleanDeadReferencesMaxCount) const
	{
		LocaleFileStorage const available = GetFileList();
		AppliedFileStorage applied = ReceiveAppliedFiles();

		size_t countRecentUpdates = 0;
		size_t countArchivedUpdates = 0;

		// Count updates
		for (auto const& entry : applied)
		{
			if (entry.second.state == RELEASED)
			{
				++countRecentUpdates;
			}
			else
			{
				++countArchivedUpdates;
			}
		}

		HashToFileNameStorage hashToName;
		for (auto entry : applied)
		{
			hashToName.insert(std::make_pair(entry.second.hash, entry.first));
		}

		size_t importedUpdates = 0;

		for (auto const& availableQuery : available)
		{
			LOG_DEBUG("sql.updates", "���ڼ����� \"%s\"...", availableQuery.first.filename().generic_string().c_str());

			AppliedFileStorage::const_iterator iter = applied.find(availableQuery.first.filename().string());

			if (iter != applied.end())
			{
				// If redundancy is disabled, skip it, because the update is already applied.
				if (!redundancyChecks)
				{
					LOG_DEBUG("sql.updates", ">> ������Ӧ�ã������������顣");
					applied.erase(iter);
					continue;
				}

				// If the update is in an archived directory and is marked as archived in our database, skip redundancy checks (archived updates never change).
				if (!archivedRedundancy && (iter->second.state == ARCHIVED) && (availableQuery.second == ARCHIVED))
				{
					LOG_DEBUG("sql.updates", ">> �����Ѵ浵�����Ϊ�Ѵ浵�����ݿ��У��Ӷ������������顣");
					applied.erase(iter);
					continue;
				}
			}

			std::string const hash = CalculateSHA1Hash(ReadSQLUpdate(availableQuery.first));

			UpdateMode mode = MODE_APPLY;

			if (iter == applied.end())
			{
				//�������������ļ�����ͬ������ϣ��ͬ��
				HashToFileNameStorage::const_iterator const hashIter = hashToName.find(hash);
				if (hashIter != hashToName.end())
				{
					LocaleFileStorage::const_iterator localeIter;
					for (localeIter = available.begin(); (localeIter != available.end()) &&
						(localeIter->first.filename().string() != hashIter->second); ++localeIter);

					//��ͻ
					if (localeIter != available.end())
					{
						LOG_WARN("sql.updates", ">> �����ļ� \"%s\" \'%s\' ������, ���ļ���Ȼ����! " \
							"���Ե������ļ�! (���������ļ���δ�޸ĸ��� \"%s\")",
							availableQuery.first.filename().string().c_str(), hash.substr(0, 7).c_str(),
							localeIter->first.filename().string().c_str());
					}
				
					else
					{
						LOG_INFO("sql.updates", ">> �����������ļ� \"%s\" �� \"%s\" \'%s\'.",
							hashIter->second.c_str(), availableQuery.first.filename().string().c_str(), hash.substr(0, 7).c_str());

						RenameEntry(hashIter->second, availableQuery.first.filename().string());
						applied.erase(hashIter->second);
						continue;
					}
				}
				// ��δ����
				else
				{
					LOG_INFO("sql.updates", ">> Ӧ�ø��� \"%s\" \'%s\'...",
						availableQuery.first.filename().string().c_str(), hash.substr(0, 7).c_str());
				}
			}
			//���������Ŀ���������ǵ����ݿ��У���ʹ���µĹ�ϣֵ���¹�ϣ��
			else if (allowRehash && iter->second.hash.empty())
			{
				mode = MODE_REHASH;

				LOG_INFO("sql.updates", ">> ����hash \"%s\" \'%s\'...", availableQuery.first.filename().string().c_str(),
					hash.substr(0, 7).c_str());
			}
			else
			{
				// ����ļ��Ĺ�ϣֵ�����ݿ��д洢�Ĺ�ϣֵ��ͬ��������Ӧ�ø��£���Ϊ���Ѹ��ģ�.
				if (iter->second.hash != hash)
				{
					LOG_INFO("sql.updates", ">> ����Ӧ�ø��� \"%s\" \'%s\' -> \'%s\' (�ı�)...", availableQuery.first.filename().string().c_str(),
						iter->second.hash.substr(0, 7).c_str(), hash.substr(0, 7).c_str());
				}
				else
				{
					// ����ļ�û�и��ģ�ֻ���ƶ��ˣ��������״̬�����б�Ҫ��.
					if (iter->second.state != availableQuery.second)
					{
						LOG_DEBUG("sql.updates", ">> Updating the state of \"%s\" to \'%s\'...",
							availableQuery.first.filename().string().c_str(), AppliedFileEntry::StateConvert(availableQuery.second).c_str());

						UpdateState(availableQuery.first.filename().string(), availableQuery.second);
					}

					LOG_DEBUG("sql.updates", ">> ������Ӧ�ò���ƥ���ϣ \'%s\'.", hash.substr(0, 7).c_str());

					applied.erase(iter);
					continue;
				}
			}

			uint32 speed = 0;
			AppliedFileEntry const file = { availableQuery.first.filename().string(), hash, availableQuery.second, 0 };

			switch (mode)
			{
			case MODE_APPLY:
				speed = Apply(availableQuery.first);
				/* fallthrough */
			case MODE_REHASH:
				UpdateEntry(file, speed);
				break;
			}

			if (iter != applied.end())
				applied.erase(iter);

			if (mode == MODE_APPLY)
				++importedUpdates;
		}
			
		// �����������Ŀ��������ã�
		if (!applied.empty())
		{
			bool const doCleanup = (cleanDeadReferencesMaxCount < 0) || (applied.size() <= static_cast<size_t>(cleanDeadReferencesMaxCount));

			for (auto const& entry : applied)
			{
				LOG_WARN("sql.updates", ">> The file \'%s\' was applied to the database, but is missing in" \
					" your update directory now!", entry.first.c_str());

				if (doCleanup)
					LOG_INFO("sql.updates", "Deleting orphaned entry \'%s\'...", entry.first.c_str());
			}

			if (doCleanup)
				CleanUp(applied);
			else
			{
				LOG_ERROR("sql.updates", "Cleanup is disabled! There were  " "%" PRIuPTR " dirty files applied to your database, " \
					"but they are now missing in your source directory!", applied.size());
			}
		}

		return UpdateResult(importedUpdates, countRecentUpdates, countArchivedUpdates);
	}
	
	bool UpdateFetcher::PathCompare::operator()(LocaleFileEntry const & left, LocaleFileEntry const & right) const
	{
		return left.first.filename().string() < right.first.filename().string();
	}

	UpdateFetcher::LocaleFileStorage UpdateFetcher::GetFileList() const
	{
		LocaleFileStorage files;
		DirectoryStorage directories = ReceiveIncludedDirectories();
		for (auto const& entry : directories)
		{
			FillFileListRecursively(entry.path, files, entry.state, 1);
		}
		return files;
	}

	UpdateFetcher::DirectoryStorage UpdateFetcher::ReceiveIncludedDirectories() const
	{
		DirectoryStorage directories;
		QueryResult const result = _retrieve("SELECT `path`, `state` FROM `updates_include`");
		if (!result)
		{
			return directories;
		}

		do
		{
			Field* fields = result->Fetch();
			std::string path = fields[0].GetString();
			if (path.substr(0, 1) == "$")
			{
				path = _sourceDirectory->generic_string() + path.substr(1);
			}
			Path const p(path);
			if (!is_directory(p))
			{
				LOG_WARN("sql.updates", "����Ŀ¼ \"%s\" ������, ����!", p.generic_string().c_str());
				continue;
			}

			DirectoryEntry const entry = { p, AppliedFileEntry::StateConvert(fields[1].GetString()) };
			directories.push_back(entry);

			LOG_TRACE("sql.updates", "��Ŀ¼ \"%s\" ��Ӹ����ļ�.", p.filename().generic_string().c_str());

		} while (result->NextRow());

		return directories;
	}

	UpdateFetcher::AppliedFileStorage UpdateFetcher::ReceiveAppliedFiles() const
	{
		AppliedFileStorage map;

		QueryResult result = _retrieve("SELECT `name`, `hash`, `state`, UNIX_TIMESTAMP(`timestamp`) FROM `updates` ORDER BY `name` ASC");
		if (!result)
			return map;

		do
		{
			Field* fields = result->Fetch();

			AppliedFileEntry const entry = { fields[0].GetString(), fields[1].GetString(),
				AppliedFileEntry::StateConvert(fields[2].GetString()), fields[3].GetUInt64() };

			map.insert(std::make_pair(entry.name, entry));
		} while (result->NextRow());

		return map;
	}

	std::string UpdateFetcher::ReadSQLUpdate(Path const & file) const
	{
		std::ifstream in(file.c_str());
		if (!in.is_open())
		{
			LOG_FATAL("sql.updates", "�޷���sql���� \"%s\" ���ж�ȡ! "
				"ֹͣ�������Ա������ݿ�������, "
				"����ȷ������������������ݿ���³���.",
				file.generic_string().c_str());

			throw "�򿪸����ļ�ʧ��!";
		}

		auto update = [&in] {
			std::ostringstream ss;
			ss << in.rdbuf();
			return ss.str();
		}();

		in.close();
		return update;
	}

	uint32 UpdateFetcher::Apply(Path const& path) const
	{
		using Time = std::chrono::high_resolution_clock;

		// Benchmark query speed
		auto const begin = Time::now();

		// Update database
		_applyFile(path);

		// Return the time it took the query to apply
		return uint32(std::chrono::duration_cast<std::chrono::milliseconds>(Time::now() - begin).count());
	}

	void UpdateFetcher::UpdateEntry(AppliedFileEntry const& entry, uint32 const speed) const
	{
		std::string const update = "REPLACE INTO `updates` (`name`, `hash`, `state`, `speed`) VALUES (\"" +
			entry.name + "\", \"" + entry.hash + "\", \'" + entry.GetStateAsString() + "\', " + std::to_string(speed) + ")";

		// Update database
		_apply(update);
	}

	void UpdateFetcher::RenameEntry(std::string const & from, std::string const & to) const
	{
		//����ɾ��
		{
			std::string const update = "DELETE FROM `updates` WHERE `name`=\"" + to + "\"";

			//�������ݿ�
			_apply(update);
		}

		//������
		{
			std::string const update = "UPDATE `updates` SET `name`=\"" + to + "\" WHERE `name`=\"" + from + "\"";

			//�������ݿ�
			_apply(update);
		}
	}

	void UpdateFetcher::CleanUp(AppliedFileStorage const& storage) const
	{
		if (storage.empty())
			return;

		std::stringstream update;
		size_t remaining = storage.size();

		update << "DELETE FROM `updates` WHERE `name` IN(";

		for (auto const& entry : storage)
		{
			update << "\"" << entry.first << "\"";
			if ((--remaining) > 0)
				update << ", ";
		}

		update << ")";

		// Update database
		_apply(update.str());
	}

	void UpdateFetcher::UpdateState(std::string const& name, State const state) const
	{
		std::string const update = "UPDATE `updates` SET `state`=\'" + AppliedFileEntry::StateConvert(state) + "\' WHERE `name`=\"" + name + "\"";

		// Update database
		_apply(update);
	}
	
	void UpdateFetcher::FillFileListRecursively(Path const & path, LocaleFileStorage & storage, State const state, uint32 const depth) const
	{
		static uint32 const MAX_DEPTH = 10;
		static directory_iterator const end;

		for (directory_iterator itr(path); itr != end; ++itr)
		{
			if (is_directory(itr->path()))
			{
				if (depth < MAX_DEPTH)
				{
					FillFileListRecursively(itr->path(), storage, state, depth + 1);
				}
			}
			else if (itr->path().extension() == ".sql")
			{
				LOG_TRACE("sql.updates", "��Ӹ����ļ� \"%s\".", itr->path().filename().generic_string().c_str());
				
				LocaleFileEntry const entry = { itr->path(), state };

				if (storage.find(entry) != storage.end())
				{
					LOG_FATAL("sql.updates", "�ļ��� \"%s\" �ظ�. ���ڸ����ǰ��ļ�������ģ� " \
						"���ÿ�����ƶ�����Ψһ!", itr->path().generic_string().c_str());
					throw "����ʧ��, �����뿴��־��Ϣ.";
				}

				storage.insert(entry);
			}
		}
	}
}