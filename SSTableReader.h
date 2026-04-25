#include <iostream>
#include <string>
#include <stdexcept>
#include "helper.h"

namespace lsm {

	class SSTableReader {
	private:
		std::ifstream file;
		std::string path;

		uint64_t index_offset;
		streampos index_start;

		std::vector<Bookmark> index;

	public:
		SSTableReader(const std::string& filepath);
		std::string findKey(const std::string& key);
	};

}