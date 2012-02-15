#ifndef __TYPES_H__
#define __TYPES_H__

#include <vector>
#include <cstddef>

typedef size_t person_t;
typedef size_t dest_t;

// Input types

// priority -> person.
typedef std::vector<person_t> priorities_t;

// person -> priority -> person.
typedef std::vector<priorities_t> people_t;

// destination -> capacity.
typedef std::vector<size_t> capacity_t;

// destination -> person list
typedef std::vector<std::vector<person_t> > destassignment_t;

// person -> destination
struct assignment_t;

#endif // __TYPES_H__
// vim: set sw=4 sts=4 ts=8 noet:
