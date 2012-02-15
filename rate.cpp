#include "rate.h"

// person list.
typedef std::vector<person_t> personlist_t;
// room -> person list.
typedef std::vector<personlist_t> roomcontents_t;

double rate(const capacity_t & capacity, const people_t & people, const assignment_t & assignment) {
    size_t person_count = people.size();
    roomcontents_t rooms(capacity.size());
    for (person_t p = 0; p < person_count; ++p) {
	rooms[assignment[p]].push_back(p);
    }

    double val = 0.0;
    for (person_t p = 0; p < person_count; ++p) {
	double pval = 0.0;
	personlist_t & roomies = rooms[assignment[p]];
	size_t roomie_count = roomies.size();
	for (person_t q = 0; q < roomie_count; ++q) {
	    person_t roomie = roomies[q];
	    if (roomie == p) continue;
	    size_t priority = 0;
	    size_t max = people[p].size();
	    while (priority < max && people[p][priority] != q) {
		++priority;
	    }
	    pval += priority*priority;
	}
	val += pval/(1+roomie_count);
    }
    return val;
}

// vim: set sw=4 sts=4 ts=8 noet:
