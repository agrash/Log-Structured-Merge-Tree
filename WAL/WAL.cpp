#include "WAL.h"

namespace lsm {

	WAL::WAL(const std::string& path) : filepath(path) {
		file.open(path, std::ios::out | std::ios::app | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Unable to open file " + path);
		}
	}

	WAL::~WAL() {
		if (file.is_open()) {
			file.close();
		}
	}

	std::string WAL::encode(bool is_tombstone, const std::string& key, const std::string& val) const {
		std::string buffer;
		uint32_t final_length = 1 + 4 + key.size() + (is_tombstone ? 0 : 4 + val.size());
		buffer.reserve(final_length);

		uint8_t tombstone = is_tombstone ? 1 : 0;
		const char* tombstone_ptr = reinterpret_cast<const char*> (&tombstone);
		buffer.insert(buffer.end(), tombstone_ptr, tombstone_ptr + sizeof(uint8_t));

		auto helper = [&buffer] (const std::string& s) {
			uint32_t length = s.size();
			const char* ptr = reinterpret_cast<const char*> (&length);
			buffer.insert(buffer.end(), ptr, ptr + sizeof(uint32_t));

			buffer.insert(buffer.end(), s.begin(), s.end());
		};

		helper(key);
		if (is_tombstone) {return buffer;}

		helper(val);
		return buffer;
	}

	void WAL::append(bool is_tombstone, const std::string& key, const std::string& val) {
		auto buffer = encode(is_tombstone, key, val);
		file.write(buffer.data(), buffer.size());
	}

	void WAL::flush() {
		file.flush();
	}

	void WAL::recover(lsm::SkipList<std::string, std::string>& memtable) const {
		std::ifstream infile(filepath, std::ios::in | std::ios::binary);
		if (!infile.is_open()) {return;}

		auto helper = [&infile]() {
			uint32_t length;
			infile.read(reinterpret_cast<char*> (&length), sizeof(uint32_t));

			std::string buffer(length, '\0');
			infile.read(buffer.data(), length);

			return buffer;
		};

		uint8_t tombstone;
		while (infile.read(reinterpret_cast<char*> (&tombstone), sizeof(uint8_t))) {
			std::string key = helper();
			if (tombstone == 0) {
				std::string val = helper();
				memtable.insert(key, val);
			}
			else {
				memtable.remove(key);
			}
		}

		infile.close();
	}

}