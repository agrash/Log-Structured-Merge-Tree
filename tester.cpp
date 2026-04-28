#include <iostream>
#include <string>
#include "DB.h"
using namespace std;
using namespace lsm;

int main() {
	DB database;

	const int limit = 1000000;
	for (int i=0; i<limit; ++i) {
		database.put(to_string(i), to_string(i));
	}

	for (int i=0; i<limit; ++i) {
		returnStruct res = database.get(to_string(i));
		if (!res.key_found || res.val != to_string(i)) {
			cout<<"Failed "<<i<<endl;
		}
		else {
			cout<<"Passed "<<i<<endl;
		}
	}
}