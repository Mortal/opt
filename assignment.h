#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "types.h"

// person -> destination
struct assignment_t {
    assignment_t(const input_t & input)
	: input(input)
	, _remaining(input.capacity)
	, dest_count(input.capacity.size())
	, person_count(input.people.size())
	, by_person(person_count, person_count)
    {

    }

    inline void reset() {
	std::fill(by_person.begin(), by_person.end(), person_count);
	_remaining = input.capacity;
    }

    inline dest_t operator[](person_t i) const {
	return by_person[i];
    }

    inline size_t remaining(dest_t d) const {
	return _remaining[d];
    }

    inline destassignment_t by_dest() const {
	destassignment_t result(dest_count);
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
    const input_t & input;
    capacity_t _remaining;
    dest_t dest_count;
    person_t person_count;

    // person -> destination.
    std::vector<dest_t> by_person;
};

#endif // __ASSIGNMENT_H__
// vim: set sw=4 sts=4 ts=8 noet:
