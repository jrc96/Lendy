#ifndef DB_UPDATER_H
#define DB_UPDATER_H

#include "Define.h"
#include "DBEnvHeader.h"
#include <set>
#include <unordered_map>
#include <string>
#include <vector>
#ifdef LENDY_COMPILER_14
#include <filesystem>
#endif

namespace DB
{
#ifdef LENDY_COMPILER_14
	typedef std::tr2::sys::path Path;
	using namespace std::tr2::sys;
#endif

	///////////////////��ȡ��////////////////
	enum UpdateMode
	{
		MODE_APPLY,
		MODE_REHASH
	};

	enum State
	{
		RELEASED,
		ARCHIVED
	};

	struct UpdateResult;
	struct AppliedFileEntry;
	struct DirectoryEntry;
	
	class UpdateFetcher
	{
	public:
#ifdef LENDY_COMPILER_14
		UpdateFetcher(Path const& updateDirectory,
			std::function<void(std::string const&)> const& apply,
			std::function<void(Path const& path)> const& applyFile,
			std::function<QueryResult(std::string const&)> const& retrieve);
#else
		UpdateFetcher(std::string const& updateDirectory,
			std::function<void(std::string const&)> const& apply,
			std::function<void(std::string const& path)> const& applyFile,
			std::function<QueryResult(std::string const&)> const& retrieve);
#endif

		~UpdateFetcher();

		UpdateResult Update(bool const redundancyChecks, bool const allowRehash,
			bool const archivedRedundancy, int32 const cleanDeadReferencesMaxCount) const;

	private:
		
#ifdef LENDY_COMPILER_14
		typedef std::pair<Path, State> LocaleFileEntry;
#else
		typedef std::pair<std::string, State> LocaleFileEntry;
#endif
		struct PathCompare
		{
			bool operator()(LocaleFileEntry const& left, LocaleFileEntry const& right) const;
		};

		typedef std::set<LocaleFileEntry, PathCompare> LocaleFileStorage;
		typedef std::unordered_map<std::string, std::string> HashToFileNameStorage;
		typedef std::unordered_map<std::string, AppliedFileEntry> AppliedFileStorage;
		typedef std::vector<DirectoryEntry> DirectoryStorage;

		LocaleFileStorage GetFileList() const;

#ifdef LENDY_COMPILER_14
		void FillFileListRecursively(Path const& path, LocaleFileStorage& storage,
			State const state, uint32 const depth) const;
#else
		void FillFileListRecursively(std::string const& path, LocaleFileStorage& storage,
			State const state, uint32 const depth) const;
#endif

		DirectoryStorage ReceiveIncludedDirectories() const;
		AppliedFileStorage ReceiveAppliedFiles() const;

#ifdef LENDY_COMPILER_14
		std::string ReadSQLUpdate(Path const& file) const;

		uint32 Apply(Path const& path) const;
#else
		std::string ReadSQLUpdate(std::string const& file) const;

		uint32 Apply(std::string const& path) const;
#endif

		void UpdateEntry(AppliedFileEntry const& entry, uint32 const speed = 0) const;
		void RenameEntry(std::string const& from, std::string const& to) const;
		void CleanUp(AppliedFileStorage const& storage) const;

		void UpdateState(std::string const& name, State const state) const;

#ifdef LENDY_COMPILER_14
		std::unique_ptr<Path> const _sourceDirectory;
		std::function<void(Path const& path)> const _applyFile;
#else
		std::string const _sourceDirectory;
		std::function<void(std::string const& path)> const _applyFile;
#endif
		std::function<void(std::string const&)> const _apply;
		std::function<QueryResult(std::string const&)> const _retrieve;
	};
	////////////////////////////


	///////������//////////
	template <class T>
	class DBWorkerPool;

	enum BaseLocation
	{
		LOCATION_REPOSITORY,
		LOCATION_DOWNLOAD
	};

	template <class T>
	class LENDY_COMMON_API DBUpdater
	{
	public:
		static bool Create(DBWorkerPool<T>& pool);

		static bool Populate(DBWorkerPool<T>& pool);

		static bool Update(DBWorkerPool<T>& pool);

		static inline std::string GetTableName();

		static inline std::string GetSourceDirectory();

		static inline  std::string GetBaseFile();

		static BaseLocation GetBaseLocationType();

		static QueryResult Retrieve(DBWorkerPool<T>& pool, std::string const& query);

		static void Apply(DBWorkerPool<T>& pool, std::string const& query);

	private:

#ifdef LENDY_COMPILER_14
		static void ApplyFile(DBWorkerPool<T>& pool, Path const& path);
		static void ApplyFile(DBWorkerPool<T>& pool, std::string const& host, std::string const& user,
			std::string const& password, std::string const& portsocket, std::string const& database, Path const& path);
#else
		static void ApplyFile(DBWorkerPool<T>& pool, std::string const& path);
		static void ApplyFile(DBWorkerPool<T>& pool, std::string const& host, std::string const& user,
			std::string const& password, std::string const& portsocket, std::string const& database, std::string const& path);
#endif

		//mysql·��
		static std::string& CorrectedPath();

	private:
		//���mysqlӦ�ó���
		static bool CheckExecutable();
	};
}

#endif