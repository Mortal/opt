#ifndef __IO_H__
#define __IO_H__

#include <cstdlib>
#include "types.h"

inline input_t get_input(size_t & rooms, size_t & person_count) {
    input_t result;
    capacity_t & capacity = result.capacity;
    people_t & people = result.people;
    condition_t & condition = result.condition;

    // Read capacities
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
    {
	condition.resize(2);
	condition[0].resize(capacity.size(), false);
	condition[1] = condition[0];
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    if (d < 12) condition[0][d] = true;
	    if (d % 4) condition[1][d] = true;
	}
    }
    // Read priorities
    while (true) {
	std::string s;
	if (!getline(std::cin, s)) break;
	std::stringstream tokens(s);
	priorities_t prio;

	tokens >> prio.wp >> prio.w1 >> prio.w2 >> prio.c1 >> prio.c2;

	roomies_t & roomies = prio.roomies;
	person_t p;
	while (tokens >> p) {
	    roomies.insert(p);
	}

	people.push_back(prio);
	++person_count;
    }

    return result;
}
#endif
// vim: set sw=4 sts=4 ts=8 noet:
