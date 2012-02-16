#ifndef __IO_H__
#define __IO_H__

#include <cstdlib>
#include "types.h"

inline input_t get_input(size_t & rooms, size_t & person_count) {
    input_t result;
    capacity_t & capacity = result.capacity;
    people_t & people = result.people;

    std::cout << "Hit me with capacities!" << std::endl;
    {
	std::string s;
	if (!getline(std::cin, s)) exit(0);
	std::stringstream tokens(s);
	size_t p;
	while (tokens >> p) {
	    capacity.push_back(p);
	    ++rooms;
	}
    }
    std::cout << "We have " << rooms << " rooms.\n"
	<< "Hit me with priorities!\n" << std::endl;
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

    return result;
}
#endif
// vim: set sw=4 sts=4 ts=8 noet:
