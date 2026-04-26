#include "SSTableBuilder.h"

namespace lsm {

	/*
	std::ofstream file;
	std::vector<Bookmark> index;
	size_t added;
	uint64_t current_offset;

	const size_t INDEX_ENTRY_SIZE = 100;

	struct Bookmark {
		std::string key;
		uint64_t offset;

		Bookmark(const std::string& key, uint64_t offset) : key(key), offset(offset) {}
	};
	*/

	SSTableBuilder::SSTableBuilder(const std::string& filepath) : added(0), current_offset(0) {
		file.open(filepath, std::ios::out | std::ios::trunc | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Unable to open " + path);
		}
	}

	SSTableBuilder::~SSTableBuilder() {
		file.close();
	}

	void SSTableBuilder::flush(const SkipList& memtable) {

		for (auto it = memtable.begin(); it != memtable.end(); ++it) {
			writeEntry(it->is_tombstone, it->key, it->val);
		}

		writeIndex();

	}

	extern std::string encode(bool is_tombstone, const std::string& key, const std::string& val);

	void SSTableBuilder::writeEntry(bool is_tombstone, const std::string& key, const std::string& val) {
		if (added == 0) {
			index.emplace_back(key, current_offset);
		}

		std::string buffer = encode(is_tombstone, key, val);
		file.write(buffer.data(), buffer.size());

		current_offset += buffer.size();
		++added;
		if (added == INDEX_ENTRY_SIZE) {added = 0;}
	}

	void SSTableBuilder::writeIndex() {
		uint64_t index_offset = current_offset;

		for (auto& [key, offset] : index) {

			uint32_t length = key.size();
			file.write(reinterpret_cast<const char*> (&length), sizeof(uint32_t));

			file.write(key.data(), key.size());

			file.write(reinterpret_cast<const char*> (&offset), sizeof(uint64_t));
		}

		file.write(reinterpret_cast<const char*> (&index_offset), sizeof(uint64_t));
	}

}