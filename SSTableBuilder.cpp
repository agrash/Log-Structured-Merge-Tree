#include "SSTableBuilder.h"

namespace lsm {

	extern std::string encode(bool is_tombstone, const std::string& key, const std::string& val);

	/*
	std::ofstream file;
	const std::string path;

	struct Bookmark {
		std::string key;
		uint64_t offset;

		Bookmark(const std::string& key, uint64_t offset) : key(key), offset(offset) {}
	};
	*/
	SSTableBuilder::SSTableBuilder(const std::string& filepath) : path(filepath) {
		file.open(path, std::ios::out | std::ios::trunc | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Unable to open file " + path);
		}
	}

	SSTableBuilder::~SSTableBuilder() {
		file.close();
	}

	void SSTableBuilder::flush(const SkipList& memtable) {
		std::vector<Bookmark> index;

		size_t added = 0;
		uint64_t current_offset = 0;
		for (auto it = memtable.begin(); it != memtable.end(); ++it) {
			std::string buffer = encode(it->is_tombstone, it->key, it->val);

			file.write(buffer.data(), buffer.size());

			if (added == 0) {
				index.emplace_back(it->key, current_offset);
			}

			current_offset += buffer.size();
			++added;
			if (added == 100) {added = 0;}
		}

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