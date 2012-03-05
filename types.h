#ifndef __TYPES_H__
#define __TYPES_H__

#include <vector>
#include <set>
#include <cstddef>
#include <boost/array.hpp>

typedef size_t person_t;
typedef size_t dest_t;

typedef size_t weight_t;

static const person_t person_capacity = 47;
static const person_t roomie_capacity = 5;

// Input types

// people you want to live with
struct roomies_t {
    typedef boost::array<person_t, roomie_capacity> container_t;
    typedef container_t::const_iterator const_iterator;
    container_t container;
    int next;

    inline roomies_t() : next(0) {
    }

    inline void insert(person_t p) {
	container[next++] = p;
    }

    inline const_iterator begin() const {
	return container.begin();
    }

    inline const_iterator end() const {
	return container.end();
    }

    inline size_t count(person_t p) const {
	for (person_t i = 0; i < roomie_capacity; ++i) {
	    if (container[i] == p) return 1;
	}
	return 0;
    }
};

struct priorities_t {
    // Weight of satisfying roommate constraint
    weight_t wp;
    // Weight of satisfying room constraint 1
    weight_t w1;
    // Weight of satisfying room constraint 2
    weight_t w2;
    // Roomies
    roomies_t roomies;
    // Whether room condition 1 is wanted
    bool c1;
    // Whether room condition 2 is wanted
    bool c2;
};

// person -> priorities
typedef std::vector<priorities_t> people_t;

// destination -> capacity.
typedef std::vector<size_t> capacity_t;

// condition number -> room -> condition value
typedef std::vector<std::vector<bool> > condition_t;

// destination -> person list
typedef std::vector<std::vector<person_t> > destassignment_t;

struct input_t {
    capacity_t capacity;
    people_t people;
    condition_t condition;
};

#endif // __TYPES_H__
// vim: set sw=4 sts=4 ts=8 noet:
