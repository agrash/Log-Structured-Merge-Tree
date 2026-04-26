#include "helper.h"
#include "SSTableBuilder.h"

namespace lsm {

	class SSTableMerger {
	private:
		bool remove_tombstones;

		std::ifstream old_file;
		std::streampos old_file_index_start;

		std::ifstream new_file;
		std::streampos new_file_index_start;

		SSTableBuilder merged_file;

		struct dataContainer {
			bool is_tombstone;
			std::string key;
			std::string val;

			dataContainer() {}
		};

		void merge();
		void writeEntry(dataContainer& data);
		void writeIndex();

	public:
		SSTableMerger(bool remove_tombstones, const std::string& old_file_path, const std::string& new_file_path, const std::string& merged_file_path);
	};

}