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
			sstables.push_back(flush_name);
			filters.emplace_back(bloom_filter_size, num_hashes);

			SSTableBuilder builder(flush_name);
			builder.flush(memtable, filters.back());

			readers.emplace_back(make_unique<SSTableReader>(flush_name));

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

	extern std::pair<uint32_t, uint32_t> getHashes(const std::string& key);

	std::string DB::get(const std::string& key) {
		auto it = memtable.search(key);
		if (it != memtable.end()) {
			if (it->is_tombstone) {return "";}
			return it->val;
		}

		auto [h1, h2] = getHashes(key);

		for (int i = sstables.size()-1; i>=0; --i) {
			if (!filters[i].contains(h1, h2)) {continue;}

			std::string res = readers[i]->findKey(key);
			if (res != "") {return std::move(res);}
		}

		return "";
	}

	void DB::recover() {
		wal_log.recover(memtable);
	}


}