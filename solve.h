#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <vector>
#include <memory>

#include "types.h"
#include "random.h"
#include "objective.h"

struct permuter_t {
    inline permuter_t(const input_t & input) : input(input) {
	dest_count = input.capacity.size();
	person_count = input.people.size();
    }
    inline void operator()(assignment_t & assignment) {
	if (flip_coin())
	    assignment.swap_dests(rand_less_than(dest_count), rand_less_than(dest_count));
	else
	    assignment.swap_people(rand_less_than(person_count), rand_less_than(person_count));
    }
private:
    const input_t & input;
    size_t dest_count;
    size_t person_count;
};

template <typename Rater>
struct solver_t {
    inline solver_t(const input_t & input, Rater & rater)
	: input(input)
	, capacity(input.capacity)
	, people(input.people)
	, solution(input)
	, capacity_sum(0)
	, rater(rater)
    {
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    capacity_sum += capacity[d];
	}
	random_assignment();
	//shuffle();
    }

    inline operator bool() { return has_next; }

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
    double best_value = 0.0;
    std::unique_ptr<assignment_t> best;
    std::unique_ptr<assignment_t> next(new assignment_t(solution));
    int since_last = 0;
    const size_t person_count = people.size();
    permuter_t permuter(input);
    while (true) {
	permuter(*next);
	double badness = rater(capacity, people, solution);
	if (!best.get() || badness < best_value) {
	    best_value = badness;
	    best.swap(next);
	    if (!next.get()) next.reset(new assignment_t(*best));

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
		*next = solution;
		since_last = 0;
	    } else {
		*next = *best;
	    }
	}
    }
}

private:
    const input_t & input;
    const capacity_t & capacity;
    const people_t & people;
    assignment_t solution;
    bool has_next;
    size_t capacity_sum;
    Rater & rater;
};

template <typename Rater>
inline void solve(const input_t & input, Rater & rater) {
    solver_t<Rater> solver(input, rater);
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
