#ifndef __SOLVE_H__
#define __SOLVE_H__

#include "types.h"

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

    inline void set_person(person_t p, dest_t d) {
	if (by_person[p] < person_count) {
	    ++_remaining[by_person[p]];
	}
	--_remaining[d];
	by_person[p] = d;
    }

private:
    const capacity_t & capacity;
    capacity_t _remaining;

    size_t person_count;

    // person -> destination.
    std::vector<dest_t> by_person;
};

struct assignment_enumeration {
    inline assignment_enumeration(const capacity_t & capacity, const people_t & people) 
	: capacity(capacity)
	, people(people) {

    }

    inline operator bool() { return has_next; }

    assignment_t operator()() {
	return simple_assignment();
    }

    assignment_t simple_assignment() {
	assignment_t result(capacity, people.size());
	dest_t next = 0;
	for (person_t p = 0; p < people.size(); ++p) {
	    while (!result.remaining(next)) {
		++next;
	    }
	    result.set_person(p, next);
	}
	return result;
    }

private:
    const capacity_t & capacity;
    const people_t & people;
    bool has_next;
};

assignment_enumeration solve(const capacity_t & capacity, const people_t & people);

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
