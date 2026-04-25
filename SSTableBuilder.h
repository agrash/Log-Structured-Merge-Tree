#include "helper.h"

namespace lsm {

	class SSTableBuilder {
	private:
		std::ofstream file;
		const std::string path;

	public:
		SSTableBuilder(const std::string& filepath);
		~SSTableBuilder();
		void flush(const SkipList& memtable);
	};

}