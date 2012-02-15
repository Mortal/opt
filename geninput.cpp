#include <iostream>
#include <vector>
#include "solve.h"

int main(int argc, char ** argv) {
    const int n = 20;
    for (int i = 0; i < n; ++i) {
	std::vector<int> priorities(n);
	for (int j = 0; j < n; ++j) {
	    priorities[j] = j;
	}
	shuffle(priorities.begin(), priorities.end());
	for (int j = 0; j < n; ++j) {
	    std::cout << priorities[j] << ' ';
	}
	std::cout << std::endl;
    }
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
