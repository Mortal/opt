#ifndef __TYPES_H__
#define __TYPES_H__

#include <vector>
#include <set>
#include <cstddef>

typedef size_t person_t;
typedef size_t dest_t;

typedef size_t weight_t;

// Input types

// people you want to live with
typedef std::set<person_t> roomies_t;

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
