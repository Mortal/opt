#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <boost/timer.hpp>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>
#include <ctime>

#include "types.h"
#include "random.h"
#include "objective.h"
#include "tourney.h"

typedef static_vector<dest_t, dest_capacity> destorder_t;
typedef static_vector<person_t, person_capacity> personorder_t;
typedef static_vector<dest_t, capacitysum_capacity> slots_t;

struct first_greater {
    template <typename First, typename Second>
    inline bool operator()(const std::pair<First, Second> & lhs, const std::pair<First, Second> & rhs) {
	return lhs.first > rhs.first;
    }
};

template <size_t M>
struct assignment_tournament {
    typedef std::pair<weight_t, std::shared_ptr<assignment_t> > item_type;
    tournament_tree<item_type, M, first_greater> tree;
    size_t remaining;

    inline assignment_tournament()
	: tree(std::make_pair(static_cast<weight_t>(0), std::shared_ptr<assignment_t>()))
	, remaining(M)
    {
    }

    inline void insert(weight_t w, const assignment_t & assignment) {
	assert(remaining <= M);
	if (remaining) {
	    tree.push(std::make_pair(w, new assignment_t(assignment)));
	    --remaining;
	    return;
	}
	if (tree.top().first > w) {
	    return;
	}
	tree.replace_top(std::make_pair(w, new assignment_t(assignment)));
    }

    inline std::shared_ptr<assignment_t> pop() {
	std::shared_ptr<assignment_t> best = tree.top().second;
	tree.pop();
	return best;
    }

    template <typename OutputIterator>
    OutputIterator uninitialized_copy_to(OutputIterator i) {
	for (auto j = tree.begin(); j != tree.end(); ++j) {
	    new (i++) assignment_t(*(j->second));
	}
	return i;
    }

    inline weight_t top_weight() {
	return tree.top().first;
    }
};

struct permuter_t {
    inline permuter_t(const input_t & input) : input(input), _exhausted(false) {
	dest_count = input.capacity.size();
	person_count = input.people.size();
	assert(dest_count > 0);
	assert(person_count > 0);
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
		d = 0;
		if (++p >= person_count) return inc();
	    }
	} else if (p1 < person_count) {
	    if (p2 < person_count-1) {
		++p2;
	    } else {
		p2 = 0;
		if (++p1 >= person_count) return inc();
	    }
	} else if (d1 < dest_count) {
	    if (d2 < dest_count-1) {
		++d2;
	    } else {
		d2 = 0;
		if (++d1 >= dest_count) return inc();
	    }
	} else {
	    _exhausted = true;
	}
	//std::cout << p << ' ' << d << ' ' << p1 << ' ' << p2 << ' ' << d1 << ' ' << d2 << std::endl;
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
	assert(p >= person_count && "Not enough destinations");
    }

inline weight_t optimize(assignment_t & base) {
    static const size_t k = 16;
    char cbest[k*sizeof(assignment_t)];
    assignment_t * best = reinterpret_cast<assignment_t *>(cbest);

    permuter_t p(input);
    assignment_tournament<k> t;
    while (!p.exhausted()) {
	assignment_t solution = base;
	p(solution);
	weight_t solv = obj(input, solution);
	++rep;
	t.insert(solv, solution);
    }
    t.uninitialized_copy_to(best);

    weight_t wb = 0;
    size_t ib = 0;
    for (size_t i = 0; i < k; ++i) {
	locally_optimize(best[i]);
	weight_t w = obj(input, best[i]);
	++rep;
	if (w > wb) {
	    wb = w;
	    ib = i;
	}
    }
    base = best[ib];
    return wb;
}

inline void locally_optimize(assignment_t & best) {
    weight_t best_value = obj(input, best);
    ++rep;
    bool improvement = true;
    while (improvement) {
	improvement = false;
	permuter_t p(input);
	while (!p.exhausted()) {
	    assignment_t solution = best;
	    p(solution);
	    weight_t goodness = obj(input, solution);
	    ++rep;
	    if (goodness > best_value) {
		best_value = goodness;
		best = solution;
		improvement = true;
	    }
	}
    }
}

inline void find_good(assignment_t & target) {
    const size_t iterations = 50000;
    assignment_t solution = target;
    weight_t best = 0;
    for (size_t i = 0; i < iterations; ++i) {
	shuffle(solution);
	++rep;
	weight_t w = obj(input, solution);
	if (w > best) {
	    target = solution;
	    best = w;
	}
    }
}

inline weight_t find_better(assignment_t & target) {
    weight_t best_value = 0;
    for (size_t i = 0; i < 20; ++i) {
	assignment_t solution = target;
	find_good(solution);
	weight_t goodness = optimize(solution);
	if (goodness > best_value) {
	    best_value = goodness;
	    rep(input, solution, goodness);
	    target = solution;
	}
    }
    return best_value;
}

void go() {
    rep.start();

    while (true) {
	find_better(solution);
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
