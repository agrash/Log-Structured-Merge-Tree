#include "helper.h"
#include "SkipList.h"
#include "WAL.h"
#include "SSTableBuilder.h"
#include "SSTableReader.h"

namespace lsm {


	class DB {
	private:
		SkipList memtable;
		WAL wal_log;
		static constexpr size_t FLUSH_TRIGGER = 4 * 1024 * 1024;
		static constexpr size_t bloom_filter_size = FLUSH_TRIGGER / (16);
		const int num_hashes = 10;

		const std::string prefix = "sstable";

		std::vector<std::string> sstables;
		std::vector<BloomFilter> filters;
		std::vector<std::unique_ptr<SSTableReader>> readers;

		void checkAndHandleFlush();

	public:
		DB() : wal_log("wal.log") {}

		void put(const std::string& key, const std::string& val);
		void remove(const std::string& key);
		returnStruct get(const std::string& key);

		void recover();
	};

}