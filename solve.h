#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <vector>
#include <memory>
#include <fstream>

#include "types.h"
#include "random.h"
#include "objective.h"


struct permuter_t {
    inline permuter_t(const input_t & input) : input(input) {
	dest_count = input.capacity.size();
	person_count = input.people.size();
	reset();
    }
    inline bool exhausted() const {
	return _exhausted;
    }
    inline void reset() {
	seed = p1 = p2 = d1 = d2 = p = d = 0;
    }
    inline void operator()(assignment_t & assignment) {
	if (p < person_count) {
	    if (assignment.remaining(d) > 0) {
		assignment.set_person(p, d);
	    }
	} else if (p1 < person_count) {
	    assignment.swap_people(p1, p2);
	} else {
	    assignment.swap_dests(d1, d2);
	}

	inc();
    }

private:
    const input_t & input;
    dest_t dest_count;
    person_t person_count;
    size_t seed;
    person_t p1, p2;
    dest_t d1, d2;
    person_t p;
    dest_t d;
    bool _exhausted;

    inline void inc() {
	if (p < person_count) {
	    if (d < dest_count) {
		++d;
	    } else {
		++p;
		d = 0;
	    }
	} if (p1 < person_count) {
	    if (p2 < person_count) {
		++p2;
	    } else {
		++p1;
		p2 = 0;
	    }
	} else if (d1 < dest_count) {
	    if (d2 < dest_count) {
		++d2;
	    } else {
		++d1;
		d2 = 0;
	    }
	} else {
	    _exhausted = true;
	}
    }
};

template <typename Objective>
struct solver_t {
    inline solver_t(const input_t & input, Objective & obj)
	: input(input)
	, capacity(input.capacity)
	, dest_count(capacity.size())
	, people(input.people)
	, person_count(people.size())
	, dests_in_order(dest_count)
	, people_in_order(person_count)
	, solution(input)
	, capacity_sum(0)
	, obj(obj)
    {
	for (dest_t d = 0; d < dest_count; ++d) {
	    capacity_sum += capacity[d];
	    dests_in_order[d] = d;
	}
	for (person_t i = 0; i < person_count; ++i) {
	    people_in_order[i] = i;
	}
	random_assignment();
	//shuffle();
    }

    inline operator bool() { return has_next; }

    void shuffle() {
	solution.reset();
	std::vector<dest_t> destorder(dests_in_order);
	::shuffle(destorder.begin(), destorder.end());
	std::vector<person_t> order(people_in_order);
	::shuffle(order.begin(), order.end());
	dest_t d;
	person_t p = 0;
	for (d = 0; d < dest_count; ++d) {
	    dest_t next = destorder[d];
	    while (solution.remaining(next)) {
		solution.set_person(order[p++], next);
		if (p >= person_count) break;
	    }
	    if (p >= person_count) break;
	}
    }

    void random_assignment() {
	std::vector<dest_t> slots(capacity_sum);
	size_t i = 0;
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    for (size_t j = 0; j < capacity[d]; ++j, ++i) {
		slots[i] = d;
	    }
	}
	::shuffle(slots.begin(), slots.end());
	for (person_t p = 0; p < people.size(); ++p) {
	    solution.set_person(p, slots[p]);
	}
    }

void go() {
    weight_t best_value = 0;
    std::unique_ptr<assignment_t> best;
    std::unique_ptr<assignment_t> next(new assignment_t(solution));
    const size_t person_count = people.size();
    size_t since_last = 0;
    while (true) {
	shuffle();
	++since_last;
	weight_t goodness = obj(input, solution);
	if (!best.get() || goodness > best_value) {
	    best_value = goodness;
	    best.swap(next);
	    if (!next.get()) {
		next.reset(new assignment_t(*best));
	    }

	    std::cout << "\n\nAfter " << since_last << " attempts.\nThe goodness is:" << std::endl;
	    std::cout << goodness << std::endl;
	    std::cout << "The result is:" << std::endl;
	    for (person_t p = 0; p < person_count; ++p) {
		std::cout << p << " -> " << solution[p] << std::endl;
	    }
	    std::cout << std::endl;
	    since_last = 0;
	}
    }
}

private:
    const input_t & input;
    const capacity_t & capacity;
    dest_t dest_count;
    const people_t & people;
    person_t person_count;
    std::vector<dest_t> dests_in_order;
    std::vector<person_t> people_in_order;
    assignment_t solution;
    bool has_next;
    size_t capacity_sum;
    Objective & obj;
};

template <typename Objective>
inline void solve(const input_t & input, Objective & obj) {
    solver_t<Objective> solver(input, obj);
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
