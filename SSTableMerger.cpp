#include "SSTableMerger.h"

namespace lsm {

	/*
	bool remove_tombstones

	std::ifstream old_file;
	std::streampos old_file_index_start;

	std::ifstream new_file;
	std::streampos new_file_index_start;

	SSTableBuilder merged_file;
	*/
	SSTableMerger::SSTableMerger(bool remove_tombstones, const std::string& old_file_path, const std::string& new_file_path, const std::string& merged_file_path) : remove_tombstones(remove_tombstones), merged_file(merged_file_path) {

		old_file.open(old_file_path, std::ios::in | std::ios::binary);
		if (!old_file.is_open()) {throw std::runtime_error("Unable to open " + old_file_path);}

		new_file.open(new_file_path, std::ios::in | std::ios::binary);
		if (!new_file.is_open()) {throw std::runtime_error("Unable to open " + new_file_path);}

		auto helper = [](std::streampos& index_start, std::ifstream& file) {
			file.seekg(-(int)(sizeof(uint64_t)), std::ios::end);
			uint64_t file_offset;
			file.read(reinterpret_cast<char*> (&file_offset), sizeof(uint64_t));
			index_start = static_cast<std::streamoff> (file_offset);
		};

		helper(old_file_index_start, old_file);
		helper(new_file_index_start, new_file);

		merge();
	}

	extern bool decode(std::ifstream& infile, bool& is_tombstone, std::string& key, std::string& val);

	void SSTableMerger::merge() {
		old_file.seekg(0);
		new_file.seekg(0);

		std::streampos old_file_pos = old_file.tellg();
		std::streampos new_file_pos = new_file.tellg();

		dataContainer old_file_data;
		dataContainer new_file_data;

		bool read_old_file = true;
		bool read_new_file = true;

		auto helper = [](std::ifstream& infile, std::streampos& pos, dataContainer& container) {
			decode(infile, container.is_tombstone, container.key, container.val);
			pos = infile.tellg();
		};

		while (old_file_pos < old_file_index_start && new_file_pos < new_file_index_start) {
			if (read_old_file) {
				helper(old_file, old_file_pos, old_file_data);
			}
			if (read_new_file) {
				helper(new_file, new_file_pos, new_file_data);
			}

			auto cmp_keys = new_file_data.key <=> old_file_data.key;
			if (cmp_keys == 0) {
				writeEntry(new_file_data);
				read_old_file = true;
				read_new_file = true;
			}
			else if (cmp_keys < 0) {
				writeEntry(new_file_data);
				read_old_file = false;
				read_new_file = true;
			}
			else {
				writeEntry(old_file_data);
				read_old_file = true;
				read_new_file = false;
			}
		}

		auto addRemaining = [&helper, this](std::ifstream& file, std::streampos& pos, std::streampos& end, dataContainer& data, bool read_pending) {
			if (read_pending) {writeEntry(data);}
			while (pos < end) {
				helper(file, pos, data);
				writeEntry(data);
			}
		};

		addRemaining(old_file, old_file_pos, old_file_index_start, old_file_data, !read_old_file);
		addRemaining(new_file, new_file_pos, new_file_index_start, new_file_data, !read_new_file);

		writeIndex();
	}

	void SSTableMerger::writeEntry(dataContainer& data) {
		if (remove_tombstones && data.is_tombstone) {return;}
		merged_file.writeEntry(data.is_tombstone, data.key, data.val);
	}

	void SSTableMerger::writeIndex() {
		merged_file.writeIndex();
	}

}