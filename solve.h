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
	shuffle();
    }

    inline operator bool() { return has_next; }

    void shuffle() {
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

void optimize(assignment_t & best) {
    weight_t best_value = obj(input, best);
    solution = best;
    permuter_t p(input);
    while (!p.exhausted()) {
	p(solution);
	weight_t goodness = obj(input, solution);
	if (goodness > best_value) {
	    best_value = goodness;
	    best = solution;
	    p.reset();
	}
    }
    solution = best;
}

void go() {
    weight_t best_value = 0;
    std::unique_ptr<assignment_t> best;
    std::unique_ptr<assignment_t> next(new assignment_t(solution));
    rep.start();
    size_t since_last = 0;
    bool maximized = false;
    while (true) {
	weight_t goodness;
#ifdef DEBUG
	const size_t threshold = 10000;
#else
	const size_t threshold = 1000000;
#endif
	if (!maximized && since_last >= threshold) {
	    optimize(*best);
	    since_last = 0;
	    goodness = obj(input, solution);
	    maximized = true;
	} else {
	    shuffle();
	    ++since_last;
	    goodness = obj(input, solution);
	    if (goodness > best_value) maximized = false;
	}
	++rep;
	if (!best.get() || goodness > best_value) {
	    best_value = goodness;
	    best.reset(new assignment_t(solution));
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

struct cout_reporter {
    inline cout_reporter() : attempts(0) {
    }

    inline void start() {
	t.start();
    }

    inline size_t operator++() { return ++attempts; }
    inline size_t operator++(int) { return attempts++; }
    inline void operator+=(size_t s) { attempts += s; }

    struct ptrless {
	template <typename T>
	inline bool operator()(const T * a, const T * b) {
	    return *a < *b;
	}
    };

    inline void operator()(const input_t & input, const assignment_t & solution, const weight_t & goodness) {
	    const people_t & people = input.people;
	    const condition_t & condition = input.condition;
	    destassignment_t by_dest = solution.by_dest();
	    size_t person_count = people.size();

	    std::cout << "\n\n";
	    std::cout << t.format();
	    std::cout << "After " << attempts << " attempts in total." << std::endl;
	    std::cout << "Beboer & Gang & g_p & g_s & g_e & v_p & v_s & v_e &  G & Ønskede roomies \\\\" << std::endl;

	    char buf[person_count*sizeof(goodness_calculation)];
	    goodness_calculation * gs = reinterpret_cast<goodness_calculation *>(buf);
	    goodness_calculation * pgs[person_count];

	    for (person_t p = 0; p < person_count; ++p) {
		const priorities_t & prio = people[p];
		dest_t dest = solution[p];
		destassignment_t::item_type & actual_roomies = by_dest[dest];
		pgs[p] = new (gs+p) goodness_calculation(prio, dest, condition, actual_roomies);
	    }
	    std::sort(pgs+0, pgs+person_count, ptrless());
	    for (person_t p = 0; p < person_count; ++p) {
		const goodness_calculation & c = *pgs[p];
		dest_t dest = solution[p];
		std::cout << std::setw(6) << pgs[p]-gs << " &";
		if (dest%4)
		    std::cout << std::setw(2) << (4+dest/4) << '-' << dest%4 << ' ';
		else
		    std::cout << std::setw(2) << (4+dest/4) << '-' << "St";
		std::cout << " &"
		          << std::setw(4) << c.g_p << " &"
		          << std::setw(4) << c.g_s << " &"
		          << std::setw(4) << c.g_e << " &"
			  << std::setw(4) << c.v_p << " &"
			  << std::setw(4) << c.v_s << " &"
			  << std::setw(4) << c.v_e << " &"
			  << std::setw(3) << c.G << " & ";
		for (roomies_t::const_iterator i = c.wished_roomies.begin(); i != c.wished_roomies.end(); ++i) {
		    std::cout << std::setw(3) << *i << ',';
		}
		std::cout << " \\\\\n";
	    }
	    std::cout << "Sum &&&&&&&& " << goodness << " & \\\\" << std::endl;
	    std::cout << std::flush;
    }

private:
    boost::timer::cpu_timer t;
    size_t attempts;
};

template <typename Objective, typename Reporter>
inline void solve(const input_t & input, Objective & obj, Reporter & reporter) {
    solver_t<Objective, Reporter> solver(input, obj, reporter, time(NULL));
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
