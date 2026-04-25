#include "helper.h"

using namespace lsm {

	bool Bookmark::operator<(const Bookmark& other) const {
		return key < other.key;
	}

	bool Bookmark::operator<(const std::string& other) const {
		return key < other;
	}

	bool Bookmark::operator==(const std::string& other) const {
		return key == other;
	}

	std::string encode(bool is_tombstone, const std::string& key, const std::string& val) const {
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

	bool decode(std::ifstream& infile, bool& is_tombstone, std::string& key, std::string& val) {

		auto helper = [&infile]() {
			uint32_t length;
			infile.read(reinterpret_cast<char*> (&length), sizeof(uint32_t));

			std::string buffer(length, '\0');
			infile.read(buffer.data(), length);

			return buffer;
		};

		uint8_t tombstone;
		bool success = infile.read(reinterpret_cast<char*> (&tombstone), sizeof(uint8_t));
		if (!success) {return false;}

		key = helper();
		if (tombstone == 0) {
			is_tombstone = false;
			val = helper();
		}
		else {
			is_tombstone = true;
		}

		return true;
	}
}