#include <iostream>
#include <vector>
#include <cstdlib>
#include "solve.h"

void test_mask() {
    const int n = 200000;
    for (int m = 0; m < n; m += n/10) {
	std::vector<char> mask = random_mask(m, n);
	int count = 0;
	for (int i = 0; i < n; ++i) {
	    if (mask[i]) ++count;
	}
	if (count != m) {
	    std::cout << "Got " << count << ", expected " << m << std::endl;
	    exit(1);
	}
    }
}

int main() {
    test_mask();
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
