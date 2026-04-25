#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

namespace lsm {

	std::string encode(bool is_tombstone, const std::string& key, const std::string& val);
	std::string decode(std::ifstream& file, bool& is_tombstone, std::string& key, std::string& val);

	struct Bookmark {
		std::string key;
		uint64_t offset;

		Bookmark(const std::string& key, uint64_t offset) : key(key), offset(offset) {}

		bool operator<(const Bookmark& other) const;
		bool operator<(const std::string& other) const;
		bool operator==(const std::string& other) const;
	};

}