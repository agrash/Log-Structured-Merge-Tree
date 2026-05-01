#include "DB.h"

namespace lsm {

	/*
	SkipList memtable;
	WAL wal_log;
	static constexpr size_t FLUSH_TRIGGER = 4 * 1024 * 1024;

	const std::string prefix = "sstable";
	const std::string wal_file = "wal.log";

	std::vector<std::string> sstables;
	*/

	void DB::checkAndHandleFlush() {
		size_t memtable_size = memtable.getSizeBytes();

		if (memtable_size > FLUSH_TRIGGER) {
			std::cout<<"Trigerring Flush!\n";

			//write logic for flush.

			std::string flush_name = prefix + std::to_string(sstables.size()) + ".db";
			SSTableBuilder builder(flush_name);
			builder.flush(memtable);
			sstables.push_back(flush_name);

			wal_log.clear();
			memtable.clear();
		}
	}

	void DB::put(const std::string& key, const std::string& val) {
		wal_log.append(false, key, val);
		memtable.insert(false, key, val);

		checkAndHandleFlush();
	}

	void DB::remove(const std::string& key) {
		wal_log.append(true, key, "");
		memtable.insert(true, key, "");
	}

	returnStruct DB::get(const std::string& key) {
		auto it = memtable.search(key);
		if (it != memtable.end()) {
			if (it->is_tombstone) {return returnStruct(true, true, "");}
			return returnStruct(true, false, it->val);
		}

		for (auto it = sstables.rbegin(); it != sstables.rend(); ++it) {
			SSTableReader reader(*it);

			returnStruct res = reader.findKey(key);
			if (res.key_found) {return res;}
		}

		return returnStruct(false, false, "");
	}

	void DB::recover() {
		wal_log.recover(memtable);
	}


}