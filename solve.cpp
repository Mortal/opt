#include "solve.h"

assignment_t solve(const capacity_t & capacity, const people_t & people) {
    capacity_t remaining = capacity;
    assignment_t result(people.size());
    dest_t next = 0;
    for (person_t p = 0; p < people.size(); ++p) {
	while (!remaining[next]) {
	    ++next;
	}
	--remaining[next];
	result[p] = next;
    }
    return result;
}
// vim: set sw=4 sts=4 ts=8 noet:
