#ifndef __OBJECTIVE_H__
#define __OBJECTIVE_H__

#include "types.h"
#include "assignment.h"

// person list.
typedef std::vector<person_t> personlist_t;
// room -> person list.
typedef std::vector<personlist_t> roomcontents_t;

struct obj_goodness {
weight_t operator()(const input_t & input, const assignment_t & assignment) {
    //const capacity_t & capacity = input.capacity;
    const people_t & people = input.people;
    const condition_t & condition = input.condition;
    destassignment_t by_dest = assignment.by_dest();
    size_t person_count = people.size();

    weight_t val = 0;
    for (person_t p = 0; p < person_count; ++p) {
	const priorities_t & prio = people[p];
	dest_t dest = assignment[p];
	if (prio.w1 && condition[0][dest]) val += prio.w1;
	if (prio.w2 && condition[1][dest]) val += prio.w2;
	if (prio.wp) {
	    const roomies_t & wished_roomies = prio.roomies;
	    std::vector<person_t> & actual_roomies = by_dest[assignment[p]];
	    for (person_t q = 0; q < actual_roomies.size(); ++q) {
		if (wished_roomies.count(actual_roomies[q])) {
		    val += prio.wp;
		    break;
		}
	    }
	}
    }
    return val;
}
};

#endif // __OBJECTIVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
