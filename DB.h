#pragma once
#include "helper.h"
#include "SkipList.h"
#include "WAL.h"
#include "SSTableBuilder.h"
#include "SSTableReader.h"
#include "SSTableMerger.h"

namespace lsm {


	class DB {
	private:
		SkipList memtable;
		WAL wal_log;
		static constexpr size_t FLUSH_TRIGGER = 4 * 1024 * 1024;
		static constexpr size_t bloom_filter_size = FLUSH_TRIGGER / (16);
		const int num_hashes = 10;

		const size_t COMPACTION_TRIGGER = 4; // Merge after reaching this many files on a level.

		const std::string prefix = "./Tables/sstable";

		std::vector<std::vector<BloomFilter>> filters;
		std::vector<std::vector<std::unique_ptr<SSTableReader>>> readers_with_level;

		void checkAndHandleFlush();

	public:
		DB() : wal_log("wal.log"), filters(1), readers_with_level(1) {}

		void put(const std::string& key, const std::string& val);
		void remove(const std::string& key);
		std::optional<std::string> get(const std::string& key);

		void recover();
	};

}