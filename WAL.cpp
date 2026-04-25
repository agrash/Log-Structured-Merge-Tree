#include "WAL.h"

namespace lsm {

	/*
	std::ofstream file;
	const std::string filepath;
	*/

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

	extern std::string encode(bool is_tombstone, const std::string& key, const std::string& val) const;

	void WAL::append(bool is_tombstone, const std::string& key, const std::string& val) {
		auto buffer = encode(is_tombstone, key, val);
		file.write(buffer.data(), buffer.size());
	}

	void WAL::flush() {
		file.flush();
	}

	extern bool decode(std::ifstream& infile, bool& is_tombstone, std::string& key, std::string& val);

	void WAL::recover(lsm::SkipList& memtable) const {
		std::ifstream infile(filepath, std::ios::in | std::ios::binary);
		if (!infile.is_open()) {return;}


		bool is_tombstone;
		std::string key, val;

		while (decode(infile, is_tombstone, key, val)) {
			if (is_tombstone) {memtable.insert(false, key, val);}
			else {memtable.insert(true, key, {});}
		}

		infile.close();
	}

}