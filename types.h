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
static const person_t roomie_capacity = 9;
static const dest_t dest_capacity = 25;
static const person_t destpeople_capacity = 15;

// Input types

template <typename T, size_t N>
struct static_vector {
    typedef T item_type;
    typedef boost::array<T, N> container_t;
    container_t container;
    typedef typename container_t::iterator iterator;
    typedef typename container_t::const_iterator const_iterator;

    size_t n;

    inline static_vector() : n(0) {
	std::fill(container.begin(), container.end(), T());
    }

    inline static_vector(size_t n) : n(n) {
	std::fill(container.begin(), container.begin()+n, T());
    }

    inline T & operator[](size_t i) { return container[i]; }
    inline const T & operator[](size_t i) const { return container[i]; }

    inline void push_back(const T & el) {
	container[n++] = el;
    }

    inline void insert(const T & el) {
	push_back(el);
    }

    inline size_t size() const {
	return n;
    }

    inline void resize(size_t m) {
	n = m;
	std::fill(container.begin(), (m == N) ? container.end() : container.begin()+m, T());
    }

    inline void resize(size_t m, const T & el) {
	n = m;
	std::fill(container.begin(), (m == N) ? container.end() : container.begin()+m, el);
    }

    inline size_t count(const T & p) const {
	for (size_t i = 0; i < N; ++i) {
	    if (container[i] == p) return 1;
	}
	return 0;
    }

    inline iterator begin() { return container.begin(); }
    inline iterator end() { return container.begin()+n; }
    inline const_iterator begin() const { return container.begin(); }
    inline const_iterator end() const { return container.begin()+n; }
};

// people you want to live with
typedef static_vector<person_t, roomie_capacity> roomies_t;

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
typedef static_vector<priorities_t, person_capacity> people_t;

// destination -> capacity.
typedef static_vector<size_t, dest_capacity> capacity_t;

// condition number -> room -> condition value
typedef static_vector<static_vector<bool, dest_capacity>, 2> condition_t;

// destination -> person list
typedef static_vector<static_vector<person_t, destpeople_capacity>, dest_capacity> destassignment_t;

struct input_t {
    capacity_t capacity;
    people_t people;
    condition_t condition;
};

#endif // __TYPES_H__
// vim: set sw=4 sts=4 ts=8 noet:
