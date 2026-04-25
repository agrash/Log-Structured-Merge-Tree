#include "helper.h"
#include "SkipList.h"
#include "WAL.h"
#include "SSTableReader.h"

namespace lsm {

	class DB {
	private:
		SkipList memtable;
		WAL log;
		constexpr size_t FLUSH_TRIGGER = 4 * 1024 * 1024;

		constexpr std::string sstable_prefix = "./sstable_";
		int num_tables;

		void checkAndHandleFlush();

	public:
		DB::DB(std::string log_path) : log(log_path), num_tables(0) {}

		void put(std::string& key, std::string& val);
		std::string get(const std::string& key);
	};

}