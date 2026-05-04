#include <iostream>
#include <string>
#include <random>
#include "DB.h"
using namespace std;
using namespace lsm;

class stringGenerator {
public:
	mt19937 gen;
	uniform_int_distribution<int> dist{0, 127};

	stringGenerator() : gen(random_device{}()) {}

	string generate(size_t length) {
		string res;
		for (int i=0; i<length; ++i) {
			res += (char)dist(gen);
		}
		return res;
	}
};

/*int main() {
	DB database;

	const int limit = 10000;

	for (int i=0; i<limit; ++i) {
		database.put(to_string(i), to_string(i));
	}
}*/

int main() {
	DB database;

	const int limit = 1000000;

	mt19937 length_gen(random_device{}());
	uniform_int_distribution<int> dist(1, 100);

	unordered_map<string, string> m;

	stringGenerator s;


	for (int i=0; i<limit; ++i) {

		if (i % 10000 == 0) {cout<<i<<endl;}

		string key = s.generate(dist(length_gen));
		string val = s.generate(dist(length_gen));

		
		database.put(key, val);
		m[key] = val;
	}

	for (auto& [key, val] : m) {
		bool d = dist(length_gen) <= 5;

		if (d) {
			database.remove(key);
			val = "";
		}
	}

	cout<<"Phase 1 done"<<endl;
	int counter = 0;

	for (auto& [key, val] : m) {
		if (counter % 10000 == 0) {cout<<counter<<endl;}
		counter++;
		auto res = database.get(key);

		if (!res.key_found || res.val != val || (val == "" && !res.is_tombstone)) {
			cout<<"Failed"<<endl;
			return 0;
		}

	}
	cout<<"Phase 2 done"<<endl;

	for (int i=0; i<limit / 100; ++i) {
		string key = s.generate(dist(length_gen));

		auto res = database.get(key);
		auto it = m.find(key);

		if ((it != m.end()) ^ (res.key_found)) {
			cout<<"Failed"<<endl;
			return 0;
		}
	}

	cout<<"All Clear!!!"<<endl;

	return 0;
}