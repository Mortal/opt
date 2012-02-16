#ifndef __OBJECTIVE_H__
#define __OBJECTIVE_H__

#include "types.h"
#include "assignment.h"

// person list.
typedef std::vector<person_t> personlist_t;
// room -> person list.
typedef std::vector<personlist_t> roomcontents_t;

struct obj_highestpriority {
double operator()(const capacity_t & /*capacity*/, const people_t & people, const assignment_t & assignment) {
    size_t person_count = people.size();

    double val = 0.0;
    for (person_t p = 0; p < person_count; ++p) {
	person_t prio_count = people[p].size();
	person_t q;
	for (q = 0; q < prio_count && q < 5; ++q) {
	    if (assignment[people[p][q]] == assignment[p]) break;
	}
	val += q*q*q;
    }
    return val;
}
};

struct obj_geometric {
double operator()(const capacity_t & capacity, const people_t & people, const assignment_t & assignment) {
    size_t person_count = people.size();
    roomcontents_t rooms(capacity.size());
    for (person_t p = 0; p < person_count; ++p) {
	rooms[assignment[p]].push_back(p);
    }

    double val = 0.0;
    for (person_t p = 0; p < person_count; ++p) {
	size_t prio_count = people[p].size();
	// we wish to add some amount of badness if the 1st priority isn't met.
	// we wish to add half that amount if the 2nd priority isn't met.
	// we wish to add half that if the 3rd priority isn't met. etc.
	// also, the maximum amount of punishment should be equal for all.
	// sum x*2^-i from 0 to (n-1) = y
	// 2^(1-n) (-1+2^n) x = y
	// x = y 2^(n-1) (-1+2^n)^-1
	double badness = static_cast<double>(1 << (prio_count - 1))/((1 << prio_count) - 1);
	for (person_t q = 0; q < prio_count; ++q) {
	    if (assignment[people[p][q]] != assignment[p]) {
		val += badness;
	    }
	    badness *= 0.5;
	}
    }
    return val;
}
};

#endif // __OBJECTIVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
