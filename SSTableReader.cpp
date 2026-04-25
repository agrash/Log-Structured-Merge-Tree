#include "SSTableReader.h"

namespace lsm {


	/*
	std::ifstream file;
	std::string path;

	uint64_t index_offset;
	streampos index_start;

	std::vector<Bookmark> index;
	*/
	SSTableReader::SSTableReader(const std::string& filepath) : path(filepath) {
		file.open(filepath, std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			throw runtime_error("Unable to open file " + path);
		}

		file.seekg(-(int)sizeof(uint64_t), std::ios::end);

		streampos offset_start = file.tellg();

		file.read(reinterpret_cast<char*> (&index_offset), sizeof(uint64_t));

		file.seekg(index_offset);
		index_start = file.tellg();

		streampos curr = index_start;

		while (curr < offset_start) {
			uint32_t key_length;
			file.read(reinterpret_cast<char*> (&key_length), sizeof(uint32_t));

			std::string key(key_length, '\0');
			file.read(key.data(), key_length);

			uint64_t offset;
			file.read(reinterpret_cast<char*> (&offset), sizeof(uint64_t));

			curr += sizeof(uint32_t) + key_length + sizeof(uint64_t);
			index.emplace_back(std::move(key), offset);
		}
	}

	extern bool decode(std::ifstream& infile, bool& is_tombstone, std::string& key, std::string& val);

	std::string SSTableReader::findKey(const std::string& key) {
		if (!file.is_open()) {
			throw runtime_error("Unable to open file " + path);
		}

		Bookmark dummy(key, 0);
		int idx = upper_bound(index.begin(), index.end(), dummy) - index.begin() - 1;
		if (idx < 0) {
			throw runtime_error("Key not found.");
		}

		file.seekg(index[idx].offset);

		streampos curr = file.tellg();
		streampos end  = idx != index.size() - 1 ? static_cast<std::streamoff>(index[idx + 1].offset) : index_start;

		while (curr < end) {
			bool is_tombstone;
			std::string saved_key, saved_val;

			if (!decode(file, is_tombstone, saved_key, saved_val)) {throw runtime_error("Error occured while reading file.");}

			if (key == saved_key) {
				if (is_tombstone) {return "";}
				else {return val;}
			}

			curr = file.tellg();
		}

		throw std::runtime_error("Key not found.");
	}

}