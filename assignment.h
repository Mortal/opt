#ifndef __ASSIGNMENT_H__
#define __ASSIGNMENT_H__

#include "random.h"
#include "types.h"

// person -> destination
struct assignment_t {
    typedef uint32_t hash_type;

    inline hash_type hash() {
	hash_type h = 0;
	destassignment_t a = by_dest();
	for (dest_t d = 0; d < dest_count; ++d) {
	    h = h*13;
	    for (person_t p = 0; p < a[d].size(); ++p) {
		h += a[d][p];
	    }
	}
	return h;
    }

    inline assignment_t(randutil & rand, const input_t & input)
	: input(input)
	, dest_count(input.capacity.size())
	, person_count(input.people.size())
	, rand(rand)
    {
	reset();
    }

    inline void reset() {
	std::fill(by_person.begin(), by_person.end(), person_count);
	_remaining = input.capacity;
    }

    inline dest_t operator[](person_t i) const {
	return by_person[i];
    }

    inline size_t remaining(dest_t d) const {
	return _remaining[d];
    }

    inline destassignment_t by_dest() const {
	destassignment_t result(dest_count);
	for (person_t p = 0; p < person_count; ++p) {
	    assert(by_person[p] < dest_count);
	    result[by_person[p]].push_back(p);
	}
	return result;
    }

    inline void set_person(person_t p, dest_t d) {
	BOOST_ASSERT(p < person_count);
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
		swap_people(dests[d1][p], dests[d2][p]);
	    }
	    return;
	}
	// dests[d1].size() < dests[d2].size().
	rand.shuffle(dests[d2].begin(), dests[d2].end());
	for (person_t p = 0; p < dests[d1].size(); ++p) {
	    swap_people(dests[d1][p], dests[d2][p]);
	}
    }

    inline void swap_people(person_t p1, person_t p2) {
	BOOST_ASSERT(p1 < person_count);
	BOOST_ASSERT(p2 < person_count);
	std::swap(by_person[p1], by_person[p2]);
    }

    inline assignment_t & operator=(const assignment_t & other) {
	dest_count = other.dest_count;
	person_count = other.person_count;
	std::copy(other._remaining.begin(), other._remaining.begin()+dest_count, _remaining.begin());
	std::copy(other.by_person.begin(), other.by_person.begin()+person_count, by_person.begin());
	return *this;
    }

private:
    const input_t & input;
    capacity_t _remaining;
    dest_t dest_count;
    person_t person_count;

    // person -> destination.
    boost::array<dest_t, person_capacity> by_person;
    
    randutil & rand;
};

#endif // __ASSIGNMENT_H__
// vim: set sw=4 sts=4 ts=8 noet:
