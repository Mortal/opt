#ifndef __SOLVE_H__
#define __SOLVE_H__

#include "types.h"

struct assignment_enumeration {
    assignment_enumeration(const capacity_t & capacity, const people_t & people);
    operator bool();
    assignment_t operator()();

private:
    const capacity_t & capacity;
    const people_t & people;
    assignment_t result;
};

assignment_enumeration solve(const capacity_t & capacity, const people_t & people);

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
