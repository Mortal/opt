#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "types.h"

// person -> destination
struct assignment_t {
    assignment_t(const capacity_t & capacity, person_t person_count)
	: capacity(capacity)
	, _remaining(capacity)
	, person_count(person_count)
	, by_person(person_count, person_count)
    {

    }

    inline dest_t operator[](person_t i) const {
	return by_person[i];
    }

    inline size_t remaining(dest_t d) const {
	return _remaining[d];
    }

    inline destassignment_t by_dest() const {
	destassignment_t result(capacity.size());
	for (person_t p = 0; p < person_count; ++p) {
	    result[by_person[p]].push_back(p);
	}
	return result;
    }

    inline void set_person(person_t p, dest_t d) {
	if (by_person[p] < person_count) {
	    ++_remaining[by_person[p]];
	}
	--_remaining[d];
	by_person[p] = d;
    }

    inline void swap_dests(dest_t d1, dest_t d2) {
	destassignment_t dests = by_dest();
	if (dests[d1].size() > dests[d2].size()) {
	    std::swap(d1, d2);
	} else if (dests[d1].size() == dests[d2].size()) {
	    for (person_t p = 0; p < dests[d1].size(); ++p) {
		set_person(dests[d1][p], d2);
	    }
	    for (person_t p = 0; p < dests[d2].size(); ++p) {
		set_person(dests[d2][p], d1);
	    }
	    return;
	}
	// dests[d1].size() < dests[d2].size().
	for (person_t p = 0; p < dests[d1].size(); ++p) {
	    set_person(dests[d1][p], d2);
	}
	std::vector<char> mask = random_mask(dests[d1].size(), dests[d2].size());
	for (person_t p = 0; p < dests[d2].size(); ++p) {
	    if (mask[p]) {
		set_person(dests[d2][p], d1);
	    }
	}
    }

    inline void swap_people(person_t p1, person_t p2) {
	dest_t d2 = by_person[p1];
	set_person(p1, by_person[p2]);
	set_person(p2, d2);
    }

private:
    const capacity_t & capacity;
    capacity_t _remaining;

    size_t person_count;

    // person -> destination.
    std::vector<dest_t> by_person;
};

#endif // __ASSIGNMENT_H__
