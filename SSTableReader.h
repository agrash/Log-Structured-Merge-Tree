#include <iostream>
#include <string>
#include <stdexcept>
#include "helper.h"

namespace lsm {

	class SSTableReader {
	private:
		std::ifstream file;

		uint64_t index_offset;
		std::streampos index_start;

		std::vector<Bookmark> index;

	public:
		SSTableReader(const std::string& filepath);
		~SSTableReader();
		returnStruct findKey(const std::string& key);
	};

}