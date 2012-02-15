#ifndef __TYPES_H__
#define __TYPES_H__

#include <vector>
#include <cstddef>

typedef size_t person_t;
typedef size_t dest_t;

// Input types
typedef std::vector<person_t> priorities_t;
typedef std::vector<priorities_t> people_t;
typedef std::vector<size_t> capacity_t;

// Output types
typedef std::vector<dest_t> assignment_t;

#endif // __TYPES_H__
// vim: set sw=4 sts=4 ts=8 noet:
