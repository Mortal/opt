#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <vector>
#include <memory>
#include <fstream>

#include "types.h"
#include "random.h"
#include "objective.h"

std::ofstream logfile("logfile");

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
		//logfile << "set_person(" << p << ", " << d << ")\n";
		assignment.set_person(p, d);
	    }
	} else if (p1 < person_count) {
	    //logfile << "swap_people(" << p1 << ", " << p2 << ")\n";
	    assignment.swap_people(p1, p2);
	} else {
	    //logfile << "swap_dests(" << d1 << ", " << d2 << ")\n";
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
	, people(input.people)
	, solution(input)
	, capacity_sum(0)
	, obj(obj)
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
	logfile << "shuffle()\n";
	std::vector<person_t> order(people.size());
	for (person_t i = 0; i < people.size(); ++i) {
	    order[i] = i;
	}
	::shuffle(order.begin(), order.end());
	simple_assignment(order);
    }

    void random_assignment() {
	logfile << "random_assignment()\n";
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
	double badness = obj(capacity, people, solution);
	if (!best.get() || badness < best_value) {
	    logfile << badness << "\n";
	    best_value = badness;
	    best.swap(next);
	    if (!next.get()) {
		next.reset(new assignment_t(*best));
		permuter.reset();
	    }

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
	    if (permuter.exhausted() || since_last > 500000) {
		shuffle();
		*next = solution;
		permuter.reset();
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
    Objective & obj;
};

template <typename Objective>
inline void solve(const input_t & input, Objective & obj) {
    solver_t<Objective> solver(input, obj);
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
