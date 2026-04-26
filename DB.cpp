#include "DB.h"

using namespace lsm {

	/*
	SkipList memtable;
	WAL log;
	constexpr size_t FLUSH_TRIGGER = 4 * 1024 * 1024;

	constexpr std::string sstable_prefix = "./sstable_";
	int num_tables;
	*/

	void DB::checkAndHandleFlush() {
		size_t memtable_size = memtable.getSizeBytes();

		if (memtable_size > FLUSH_TRIGGER) {
			std::cout<<"Trigerring Flush!\n";

			//write logic for flush.
		}
	}

	void DB::put(string& key, string& val) {
		log.append(false, key, val);
		memtable.insert(false, key, val);

		checkAndHandleFlush();
	}

	returnStruct DB::get(const std::string& key) {
		auto it = memtable.search(key);
		if (it != memtable.end()) {
			if (it->is_tombstone) {return returnStruct(true, true, "");}
			return returnStruct(true, false, it->val);
		}

		for (int i=num_tables; i>0; --i) {
			std::string filepath = sstable_prefix + std::to_string(i);

			SSTableReader reader(filepath);

			returnStruct res = reader.findKey(key);
			if (res.key_found) {return res;}
		}

		return returnStruct(false, false, "");
	}

}