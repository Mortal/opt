#include <iostream>
#include <vector>

const int floors = 4;
const int people[] = {7, 12, 13, 14};
const int total_capacity = 80;
const int destinations = 24;

void print_destinations() {
    std::cout << "2 2 3 3 4 4 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9" << std::endl;
}

void print_priorities() {
    int offset = 0;
    for (int f = 0; f < floors; ++f) {
	for (int p = 0; p < people[f]; ++p) {
	    for (int q = 0; q < people[f]; ++q) {
		if (q != p) std::cout << offset+q << ' ';
	    }
	    std::cout << '\n';
	}
	offset += people[f];
    }
    std::cout << std::flush;
}

int main() {
    print_destinations();
    print_priorities();
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
