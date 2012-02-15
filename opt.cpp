#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef size_t person_t;
typedef size_t dest_t;

// Input types
typedef std::vector<person_t> priorities_t;
typedef std::vector<priorities_t> people_t;
typedef std::vector<size_t> capacity_t;

// Output types
typedef std::vector<dest_t> assignment_t;

assignment_t solve(const capacity_t & capacity, const people_t & people) {
    capacity_t remaining = capacity;
    assignment_t result(people.size());
    dest_t next = 0;
    for (person_t p = 0; p < people.size(); ++p) {
	while (!remaining[next]) {
	    ++next;
	}
	--remaining[next];
	result[p] = next;
    }
    return result;
}

int main(int argc, char ** argv) {
    size_t rooms = 0;
    std::cout << "Hit me with capacities!" << std::endl;
    capacity_t capacity;
    {
	std::string s;
	if (!getline(std::cin, s)) return 0;
	std::stringstream tokens(s);
	size_t p;
	while (tokens >> p) {
	    capacity.push_back(p);
	    ++rooms;
	}
    }
    std::cout << "We have " << rooms << " rooms.\n"
	<< "Hit me with priorities!\n" << std::endl;
    size_t person_count = 0;
    people_t people;
    while (true) {
	std::string s;
	if (!getline(std::cin, s)) break;
	std::stringstream tokens(s);
	priorities_t prio;
	person_t p;
	while (tokens >> p) {
	    prio.push_back(p);
	}
	people.push_back(prio);
	++person_count;
    }
    std::cout << "We have " << person_count << " people." << std::endl;

    assignment_t solution = solve(capacity, people);

    std::cout << "The result is:" << std::endl;
    for (person_t p = 0; p < person_count; ++p) {
	std::cout << p << " -> " << solution[p] << std::endl;
    }
    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
