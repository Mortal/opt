#ifndef __OBJECTIVE_H__
#define __OBJECTIVE_H__

#include "types.h"
#include "assignment.h"

struct goodness_calculation {
    dest_t dest;
    bool g_p, g_s, g_e;
    weight_t v_p, v_s, v_e, G;
    const roomies_t & wished_roomies;

    inline goodness_calculation(const priorities_t & prio,
				dest_t dest,
				const condition_t & condition,
				const destassignment_t::item_type & actual_roomies)
	: wished_roomies(prio.roomies)
    {
	this->dest = dest;

	G = 0;

	v_s = prio.w1;
	if (condition[0][dest] == prio.c1) {
	    g_s = true;
	    G += v_s;
	} else {
	    g_s = false;
	}

	v_e = prio.w2;
	if (condition[1][dest] == prio.c2) {
	    g_e = true;
	    G += v_e;
	} else {
	    g_e = false;
	}

	v_p = prio.wp;
	g_p = false;
	if (prio.wp) {
	    for (person_t q = 0; q < actual_roomies.size(); ++q) {
		if (wished_roomies.count(actual_roomies[q]) > 0) {
		    g_p = true;
		    G += v_p;
		    break;
		}
	    }
	}
    }

    inline bool operator<(const goodness_calculation & other) const {
	return G < other.G;
    }
};

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
	destassignment_t::item_type & actual_roomies = by_dest[assignment[p]];
	goodness_calculation c(prio, dest, condition, actual_roomies);
	val += c.G;
    }
    return val;
}
};

#endif // __OBJECTIVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
