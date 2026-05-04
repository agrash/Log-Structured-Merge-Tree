#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include "MurmurHash3.h"

namespace lsm {

	std::string encode(bool is_tombstone, const std::string& key, const std::string& val);
	bool decode(std::ifstream& file, bool& is_tombstone, std::string& key, std::string& val);

	struct Bookmark {
		std::string key;
		uint64_t offset;

		Bookmark(const std::string& key, uint64_t offset) : key(key), offset(offset) {}

		bool operator<(const Bookmark& other) const;
		bool operator<(const std::string& other) const;
		bool operator==(const std::string& other) const;
	};

	struct returnStruct {
		bool key_found;
		bool is_tombstone;
		std::string val;

		returnStruct(bool key_found, bool is_tombstone, std::string val) : key_found(key_found), is_tombstone(is_tombstone), val(std::move(val)) {}
	};

	class BloomFilter {
	private:
		std::vector<bool> hash_table;
		int num_hashes;
	public:
		BloomFilter(int size, int num_hashes) : hash_table(size), num_hashes(num_hashes) {}

		void add(const std::string& key);
		bool contains(const std::string& key);
	};

}