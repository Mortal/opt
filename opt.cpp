#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"
#include "rate.h"
#include "solve.h"

int main(int argc, char ** argv) {
    size_t rooms = 0;
    std::cout << "Hit me with capacities!" << std::endl;
    capacity_t capacity;
    {
	std::string s;
	if (!getline(std::cin, s)) return 0;
	std::stringstream tokens(s);
	size_t p;
	while (tokens >> p) {
	    capacity.push_back(p);
	    ++rooms;
	}
    }
    std::cout << "We have " << rooms << " rooms.\n"
	<< "Hit me with priorities!\n" << std::endl;
    size_t person_count = 0;
    people_t people;
    while (true) {
	std::string s;
	if (!getline(std::cin, s)) break;
	std::stringstream tokens(s);
	priorities_t prio;
	person_t p;
	while (tokens >> p) {
	    prio.push_back(p);
	}
	people.push_back(prio);
	++person_count;
    }
    std::cout << "We have " << person_count << " people." << std::endl;

    assignment_enumeration solutions = solve(capacity, people);

    double best = 0.0;
    bool first = true;
    int since_last = 0;
    while (solutions) {
	assignment_t solution = solutions();
	double badness = rate(capacity, people, solution);
	if (first || badness < best) {
	    best = badness;
	    first = false;
	    std::cout << "\n\nThe badness is:" << std::endl;
	    std::cout << badness << std::endl;
	    std::cout << "The result is:" << std::endl;
	    for (person_t p = 0; p < person_count; ++p) {
		std::cout << p << " -> " << solution[p] << ":";
		for (person_t q = 0; q < people[p].size(); ++q) {
		    if (solution[p] == solution[people[p][q]]) {
			std::cout << ' ' << q;
		    }
		}
		std::cout << std::endl;
	    }
	    std::cout << std::endl;
	    since_last = 0;
	} else {
	    ++since_last;
	    if (since_last > 500000) {
		std::cout << '.' << std::flush;
		solutions.shuffle();
		since_last = 0;
	    }
	}
    }
    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
