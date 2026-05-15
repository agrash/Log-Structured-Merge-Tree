#include "DB.h"

namespace lsm {


	void DB::checkAndHandleFlush() {
		size_t memtable_size = memtable.getSizeBytes();

		if (memtable_size >= FLUSH_TRIGGER) {
			std::cout<<"Trigerring Flush!\n";


			std::string flush_name = prefix + "0_" + std::to_string(readers_with_level[0].size()) + ".db";
			filters[0].emplace_back(bloom_filter_size, num_hashes);

			SSTableBuilder builder(flush_name, filters[0].back());
			builder.flush(memtable);

			readers_with_level[0].emplace_back(make_unique<SSTableReader>(flush_name));

			wal_log.clear();
			memtable.clear();
		}

		size_t curr_level = 0;
		size_t filter_size = 4 * bloom_filter_size;

		while (readers_with_level[curr_level].size() >= COMPACTION_TRIGGER) {

			std::cout<<"Trigerring Compaction \n";

			size_t num_file = readers_with_level[curr_level].size();

			std::vector<std::string> file_paths;
			for (size_t i=0; i<num_file; ++i) {
				file_paths.push_back(readers_with_level[curr_level][i]->getFilePath());
			}

			bool remove_tombstones = false;

			if (curr_level + 1 == readers_with_level.size()) {
				filters.resize(curr_level + 2);
				readers_with_level.resize(curr_level + 2);
				remove_tombstones = true;
			}

			filters[curr_level + 1].emplace_back(filter_size, num_hashes);

			const std::string merged_file_path = prefix + std::to_string(curr_level + 1) + "_" + std::to_string(readers_with_level[curr_level + 1].size()) + ".db";


			SSTableMerger compacter(remove_tombstones, file_paths, merged_file_path, filters[curr_level + 1].back());

			readers_with_level[curr_level + 1].emplace_back(make_unique<SSTableReader>(merged_file_path));

			while (!readers_with_level[curr_level].empty()) {readers_with_level[curr_level].pop_back();}
			while (!filters[curr_level].empty()) {filters[curr_level].pop_back();}

			filter_size *= 4;
			++curr_level;
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

	std::optional<std::string> DB::get(const std::string& key) {
		auto it = memtable.search(key);
		if (it != memtable.end()) {
			if (it->is_tombstone) {return "";}
			return it->val;
		}

		auto [h1, h2] = getHashes(key);

		for (size_t level = 0; level < readers_with_level.size(); ++level) {
			for (int i = (int)readers_with_level[level].size() - 1; i>=0; --i) {
				if (!filters[level][i].contains(h1, h2)) {continue;}

				auto res = readers_with_level[level][i]->findKey(key);
				if (res) {return res;}
			}
		}

		return std::nullopt;
	}

	void DB::recover() {
		wal_log.recover(memtable);
	}


}