#include <iostream>
#include <vector>
#include <string>
#include <boost/random.hpp>
#include <iomanip>

using namespace std;

int main() {
    string line;
    getline(cin, line);
    cout << line << endl;
    boost::mt19937 rng;
    while (getline(cin, line)) {
	size_t v1 = rng() % 13;
	size_t v2 = rng() % (13-v1);
	size_t v3 = 12-v1-v2;
	if (v1+v2+v3 != 12) {
	    cerr << "Something went wrong" << endl;
	}
	size_t c = rng() % 4;
	cout << v1 << ' ' << v2 << ' ' << v3 << ' ' << (c&1) << ' ' << (c>>1) << ' ' << line << endl;
    }
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
