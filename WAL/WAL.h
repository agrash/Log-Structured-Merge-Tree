#include <iostream>
#include <vector>
#include <string>
#include "SkipList.hpp"

namespace lsm {

	class WAL {
	private:
		std::ofstream file;
		std::string filepath;
		std::string encode(bool is_tombstone, const std::string& key, const std::string& val) const;
	public:
		WAL(const std::string& path);
		~WAL();

		void append(bool is_tombstone, const std::string& key, const std::string& val);
		void flush();

		void recover(lsm::SkipList<std::string, std::string>& memtable) const;
	};

}