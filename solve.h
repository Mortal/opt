#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <boost/timer/timer.hpp>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>
#include <ctime>

#include "types.h"
#include "random.h"
#include "objective.h"

typedef static_vector<dest_t, dest_capacity> destorder_t;
typedef static_vector<person_t, person_capacity> personorder_t;
typedef static_vector<dest_t, capacitysum_capacity> slots_t;

struct permuter_t {
    inline permuter_t(const input_t & input) : input(input), _exhausted(false) {
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
	while (p < person_count) {
	    if (assignment.remaining(d) > 0) {
		assignment.set_person(p, d);
		inc();
		return;
	    } else {
		inc();
	    }
	}
	if (p1 < person_count) {
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
	    if (d < dest_count-1) {
		++d;
	    } else {
		++p;
		d = 0;
	    }
	} if (p1 < person_count) {
	    if (p2 < person_count-1) {
		++p2;
	    } else {
		++p1;
		p2 = 0;
	    }
	} else if (d1 < dest_count) {
	    if (d2 < dest_count-1) {
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

template <typename Objective, typename Reporter>
struct solver_t {
    inline solver_t(const input_t & input, Objective & obj, Reporter & rep, uint32_t seed)
	: input(input)
	, capacity(input.capacity)
	, dest_count(capacity.size())
	, people(input.people)
	, person_count(people.size())
	, destorder(dest_count)
	, personorder(person_count)
	, rand(seed)
	, solution(rand, input)
	, capacity_sum(0)
	, obj(obj)
	, rep(rep)
    {
	for (dest_t d = 0; d < dest_count; ++d) {
	    capacity_sum += capacity[d];
	    destorder[d] = d;
	}
	for (person_t i = 0; i < person_count; ++i) {
	    personorder[i] = i;
	}
	shuffle(solution);
    }

    inline operator bool() { return has_next; }

    void shuffle(assignment_t & solution) {
	solution.reset();
	rand.shuffle(destorder.begin(), destorder.end());
	rand.shuffle(personorder.begin(), personorder.end());
	dest_t d;
	person_t p = 0;
	for (d = 0; d < dest_count; ++d) {
	    dest_t next = destorder[d];
	    while (solution.remaining(next)) {
		solution.set_person(personorder[p++], next);
		if (p >= person_count) break;
	    }
	    if (p >= person_count) break;
	}
    }

void optimize(assignment_t & base) {
    //std::cout << '\n';
    weight_t w = obj(input, base);
    //std::cout << "\nLocally optimizing " << base.hash() << '/' << w << " -> " << std::flush;
    //locally_optimize(base);
    //w = obj(input, base);
    //std::cout << base.hash() << '/' << w << std::endl;
    assignment_t first = base;
    assignment_t second = base;
    permuter_t p(input);
    find_best(base, p, first, second, w, w);
    //{
    //weight_t w1 = obj(input, first);
    //weight_t w2 = obj(input, second);
    //std::cout << "Optimizing yields " << first.hash() << '/' << w1 << " and " << second.hash() << '/' << w2 << " -> " << std::flush;
    //}
    locally_optimize(first);
    locally_optimize(second);
    weight_t w1 = obj(input, first);
    weight_t w2 = obj(input, second);
    //std::cout << first.hash() << '/' << w1 << " and " << second.hash() << '/' << w2 << std::endl;
    if (w1 > w2) {
	base = first;
    } else {
	base = second;
    }
}

void locally_optimize(assignment_t & best) {
    weight_t best_value = obj(input, best);
    bool improvement = true;
    while (improvement) {
	improvement = false;
	permuter_t p(input);
	while (!p.exhausted()) {
	    assignment_t solution = best;
	    p(solution);
	    weight_t goodness = obj(input, solution);
	    if (goodness > best_value) {
		best_value = goodness;
		best = solution;
		improvement = true;
	    }
	}
    }
}

void find_best(assignment_t & base, permuter_t & p, assignment_t & first, assignment_t & second, weight_t firstv, weight_t secondv, bool swap = false) {
    while (!p.exhausted()) {
	assignment_t solution = base;
	p(solution);
	weight_t solv = obj(input, solution);
	if (solv > firstv) {
	    second = solution;
	    return find_best(base, p, second, first, solv, firstv, !swap);
	} else if (solv > secondv) {
	    second = solution;
	    return find_best(base, p, first, second, firstv, solv, swap);
	}
    }
    if (swap) std::swap(first, second);
}

void go() {
    weight_t best_value = 0;
    assignment_t best_unoptimized = solution;
    weight_t best_unoptimized_value = 0;
    size_t since_last = 0;
    rep.start();

    while (true) {
	weight_t goodness;
#ifdef DEBUG
	const size_t threshold = 10000;
#else
	const size_t threshold = 1000000;
#endif
	if (since_last >= threshold) {
	    solution = best_unoptimized;
	    optimize(solution);
	    since_last = 0;
	    goodness = obj(input, solution);
	    best_unoptimized_value = 0;
	} else {
	    shuffle(solution);
	    ++since_last;
	    goodness = obj(input, solution);
	    if (goodness > best_unoptimized_value) {
		best_unoptimized = solution;
		best_unoptimized_value = goodness;
	    }
	}
	++rep;
	if (goodness > best_value) {
	    best_value = goodness;
	    rep(input, solution, goodness);
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
    destorder_t destorder;
    personorder_t personorder;
    randutil rand;
    assignment_t solution;
    bool has_next;
    size_t capacity_sum;
    Objective & obj;
    Reporter & rep;
};

template <typename Objective, typename Reporter>
inline void solve(const input_t & input, Objective & obj, Reporter & reporter) {
    solver_t<Objective, Reporter> solver(input, obj, reporter, time(NULL));
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
