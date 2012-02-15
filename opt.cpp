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

    while (solutions) {
	assignment_t solution = solutions();
	/*
	std::cout << "The result is:" << std::endl;
	for (person_t p = 0; p < person_count; ++p) {
	    std::cout << p << " -> " << solution[p] << std::endl;
	}
	*/
	std::cout << "The badness is:" << std::endl;
	std::cout << rate(capacity, people, solution) << std::endl;
    }
    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
