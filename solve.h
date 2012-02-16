#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <vector>

#include "types.h"
#include "random.h"
#include "rate.h"

struct solver_t {
    inline solver_t(const input_t & input)
	: capacity(input.capacity)
	, people(input.people)
	, solution(capacity, people.size())
	, capacity_sum(0)
    {
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    capacity_sum += capacity[d];
	}
	random_assignment();
	//shuffle();
    }

    inline operator bool() { return has_next; }

    assignment_t get_solution() {
	return solution;
    }

    void simple_assignment(const std::vector<person_t> & order) {
	dest_t next = 0;
	for (person_t p = 0; p < order.size(); ++p) {
	    while (!solution.remaining(next)) {
		++next;
	    }
	    solution.set_person(order[p], next);
	}
    }

    void shuffle() {
	std::vector<person_t> order(people.size());
	for (person_t i = 0; i < people.size(); ++i) {
	    order[i] = i;
	}
	::shuffle(order.begin(), order.end());
	simple_assignment(order);
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
    double best = 0.0;
    bool first = true;
    int since_last = 0;
    const size_t person_count = people.size();
    while (*this) {
	assignment_t solution = get_solution();
	if (flip_coin())
	    solution.swap_dests(rand_less_than(capacity.size()), rand_less_than(capacity.size()));
	else
	    solution.swap_people(rand_less_than(people.size()), rand_less_than(people.size()));
	double badness = rate_geometric()(capacity, people, solution);
	if (first || badness < best) {
	    best = badness;
	    first = false;
	    std::cout << "\n\nThe badness is:" << std::endl;
	    std::cout << badness << std::endl;
	    std::cout << "The result is:" << std::endl;
	    for (person_t p = 0; p < person_count; ++p) {
		std::cout << p << " -> " << solution[p] << ":";
		for (person_t q = 0; q < people[p].size(); ++q) {
		    if (solution[p] == solution[people[p][q]]) {
			std::cout << ' ' << q;
		    }
		}
		std::cout << std::endl;
	    }
	    std::cout << std::endl;
	    since_last = 0;
	} else {
	    ++since_last;
	    if (since_last > 500000) {
		std::cout << '.' << std::flush;
		shuffle();
		since_last = 0;
	    }
	}
    }
}

private:
    const capacity_t & capacity;
    const people_t & people;
    assignment_t solution;
    bool has_next;
    size_t capacity_sum;
};

inline void solve(const input_t & input) {
    solver_t solver(input);
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
