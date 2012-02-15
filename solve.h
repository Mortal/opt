#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <vector>

#include "types.h"

int rand_less_than(int bound);

std::vector<char> random_mask(int m, int n);

bool flip_coin();

template <typename IT>
void shuffle(IT begin, IT end) {
    const int n = end-begin;
    for (int i = n; i--;) {
	std::iter_swap(begin+i, begin+rand_less_than(i+1));
    }
}

struct assignment_t {
    assignment_t(const capacity_t & capacity, person_t person_count)
	: capacity(capacity)
	, _remaining(capacity)
	, person_count(person_count)
	, by_person(person_count, person_count)
    {

    }

    inline dest_t operator[](person_t i) const {
	return by_person[i];
    }

    inline size_t remaining(dest_t d) const {
	return _remaining[d];
    }

    inline destassignment_t by_dest() const {
	destassignment_t result(capacity.size());
	for (person_t p = 0; p < person_count; ++p) {
	    result[by_person[p]].push_back(p);
	}
	return result;
    }

    inline void set_person(person_t p, dest_t d) {
	if (by_person[p] < person_count) {
	    ++_remaining[by_person[p]];
	}
	--_remaining[d];
	by_person[p] = d;
    }

    inline void swap_dests(dest_t d1, dest_t d2) {
	destassignment_t dests = by_dest();
	if (dests[d1].size() > dests[d2].size()) {
	    std::swap(d1, d2);
	} else if (dests[d1].size() == dests[d2].size()) {
	    for (person_t p = 0; p < dests[d1].size(); ++p) {
		set_person(dests[d1][p], d2);
	    }
	    for (person_t p = 0; p < dests[d2].size(); ++p) {
		set_person(dests[d2][p], d1);
	    }
	    return;
	}
	// dests[d1].size() < dests[d2].size().
	for (person_t p = 0; p < dests[d1].size(); ++p) {
	    set_person(dests[d1][p], d2);
	}
	std::vector<char> mask = random_mask(dests[d1].size(), dests[d2].size());
	for (person_t p = 0; p < dests[d2].size(); ++p) {
	    if (mask[p]) {
		set_person(dests[d2][p], d1);
	    }
	}
    }

    inline void swap_people(person_t p1, person_t p2) {
	dest_t d2 = by_person[p1];
	set_person(p1, by_person[p2]);
	set_person(p2, d2);
    }

private:
    const capacity_t & capacity;
    capacity_t _remaining;

    size_t person_count;

    // person -> destination.
    std::vector<dest_t> by_person;
};

struct assignment_enumeration {
    inline assignment_enumeration(const capacity_t & capacity, const people_t & people) 
	: capacity(capacity)
	, people(people)
	, result(capacity, people.size())
	, capacity_sum(0)
    {
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    capacity_sum += capacity[d];
	}
	random_assignment();
	//shuffle();
    }

    inline operator bool() { return has_next; }

    assignment_t operator()() {
	if (flip_coin())
	    result.swap_dests(rand_less_than(capacity.size()), rand_less_than(capacity.size()));
	else
	    result.swap_people(rand_less_than(people.size()), rand_less_than(people.size()));
	return result;
    }

    void simple_assignment(const std::vector<person_t> & order) {
	dest_t next = 0;
	for (person_t p = 0; p < order.size(); ++p) {
	    while (!result.remaining(next)) {
		++next;
	    }
	    result.set_person(order[p], next);
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
	    result.set_person(p, slots[p]);
	}
    }

private:
    const capacity_t & capacity;
    const people_t & people;
    assignment_t result;
    bool has_next;
    size_t capacity_sum;
};

assignment_enumeration solve(const capacity_t & capacity, const people_t & people);

#endif // __SOLVE_H__
// vim: set sw=4 sts=4 ts=8 noet:
