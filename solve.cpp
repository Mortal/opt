#include "solve.h"

assignment_enumeration solve(const capacity_t & capacity, const people_t & people) {
    return assignment_enumeration(capacity, people);
}

assignment_enumeration::assignment_enumeration(const capacity_t & capacity, const people_t & people) 
    : capacity(capacity)
    , people(people) {

    capacity_t remaining = capacity;
    result.resize(people.size());
    dest_t next = 0;
    for (person_t p = 0; p < people.size(); ++p) {
	while (!remaining[next]) {
	    ++next;
	}
	--remaining[next];
	result[p] = next;
    }
}

assignment_enumeration::operator bool() {
    return true;
}

assignment_t assignment_enumeration::operator()() {
    return result;
}
// vim: set sw=4 sts=4 ts=8 noet:
