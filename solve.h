#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <boost/timer/timer.hpp>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>

#include "types.h"
#include "random.h"
#include "objective.h"

typedef static_vector<dest_t, dest_capacity> destorder_t;
typedef static_vector<person_t, person_capacity> personorder_t;
typedef static_vector<dest_t, capacitysum_capacity> slots_t;

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

template <typename Objective, typename Reporter>
struct solver_t {
    inline solver_t(const input_t & input, Objective & obj, Reporter & rep, uint32_t seed)
	: input(input)
	, capacity(input.capacity)
	, dest_count(capacity.size())
	, people(input.people)
	, person_count(people.size())
	, dests_in_order(dest_count)
	, people_in_order(person_count)
	, rand(seed)
	, solution(rand, input)
	, capacity_sum(0)
	, obj(obj)
	, rep(rep)
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
	destorder_t destorder(dests_in_order);
	rand.shuffle(destorder.begin(), destorder.end());
	personorder_t order(people_in_order);
	rand.shuffle(order.begin(), order.end());
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
	slots_t slots(capacity_sum);
	size_t i = 0;
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    for (size_t j = 0; j < capacity[d]; ++j, ++i) {
		slots[i] = d;
	    }
	}
	rand.shuffle(slots.begin(), slots.end());
	for (person_t p = 0; p < people.size(); ++p) {
	    solution.set_person(p, slots[p]);
	}
    }

void go() {
    weight_t best_value = 0;
    std::unique_ptr<assignment_t> best;
    std::unique_ptr<assignment_t> next(new assignment_t(solution));
    rep.start();
    while (true) {
	shuffle();
	++rep;
	weight_t goodness = obj(input, solution);
	if (!best.get() || goodness > best_value) {
	    best_value = goodness;
	    best.swap(next);
	    if (!next.get()) {
		next.reset(new assignment_t(*best));
	    }

	    rep(input, solution, goodness);
	}
    }
}

private:
    const input_t & input;
    const capacity_t & capacity;
    dest_t dest_count;
    const people_t & people;
    person_t person_count;
    destorder_t dests_in_order;
    personorder_t people_in_order;
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

    inline void operator()(const input_t & input, const assignment_t & solution, const weight_t & goodness) {
	    const people_t & people = input.people;
	    const condition_t & condition = input.condition;
	    destassignment_t by_dest = solution.by_dest();
	    size_t person_count = people.size();

	    std::cout << "\n\n";
	    std::cout << t.format();
	    std::cout << "After " << attempts << " attempts in total." << std::endl;
	    std::cout << "Beboer & Gang & g_p & g_s & g_e & v_p & v_s & v_e & G & Ønskede roomies \\\\" << std::endl;
	    for (person_t p = 0; p < person_count; ++p) {
		const priorities_t & prio = people[p];
		dest_t dest = solution[p];
		const roomies_t & wished_roomies = prio.roomies;
		std::cout << std::setw(6) << p << " &" << std::setw(5) << dest << " &";
		weight_t val = 0;
		if (prio.w1 && condition[0][dest]) val += prio.w1;
		if (prio.w2 && condition[1][dest]) val += prio.w2;
		bool gp = false;
		if (prio.wp) {
		    destassignment_t::item_type & actual_roomies = by_dest[dest];
		    for (person_t q = 0; q < actual_roomies.size(); ++q) {
			if (wished_roomies.count(actual_roomies[q])) {
			    gp = true;
			    val += prio.wp;
			    break;
			}
		    }
		}
		std::cout << std::setw(4) << gp << " &"
		          << std::setw(4) << (prio.w1 && condition[0][dest]) << " &"
		          << std::setw(4) << (prio.w2 && condition[1][dest]) << " &"
			  << std::setw(4) << prio.wp << " &"
			  << std::setw(4) << prio.w1 << " &"
			  << std::setw(4) << prio.w2 << " &"
			  << std::setw(3) << val << " & ";
		for (roomies_t::const_iterator i = wished_roomies.begin(); i != wished_roomies.end(); ++i) {
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
    solver_t<Objective, Reporter> solver(input, obj, reporter);
    solver.go();
}

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
