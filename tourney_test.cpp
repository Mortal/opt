#include <iostream>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <queue>
#include <ctime>
#include "tourney.h"
#include "stream_max.h"
#include "static_heap.h"
#include "replacer.h"

template <size_t n, size_t k>
int test_correctness() {
    auto * pq = new std::priority_queue<size_t>();
    auto * tt = new tournament_tree<size_t, n>();
    boost::mt19937 rng(time(NULL));
    for (size_t i = 0; i < n; ++i) {
	size_t v = rng();
	pq->push(v);
	tt->push(v);
    }
    size_t bad = 0;
    for (size_t i = 0; i < k; ++i) {
	if (pq->top() != tt->top()) ++bad;
	pq->pop();
	tt->pop();
    }
    if (bad) {
	std::cout << bad << " bad tops out of " << k << std::endl;
	return 1;
    }
    return 0;
}

template <size_t n>
int print_test() {
    auto * tt = new tournament_tree<size_t, n>();
    boost::mt19937 rng(time(NULL));
    for (size_t i = 0; i < n; ++i) {
	size_t v = rng();
	tt->push(v);
    }
    std::cout << *tt << std::flush;
    return 0;
}

int main(int argc, char ** argv) {
    const static size_t n = 1 << 25;
    const static size_t k = n >> 7;
    // For n = 2^25, k = 2^19, tournament tree fares better.
    // For n = 2^25, k = 2^18, priority queue fares better.

    std::string arg;
    if (argc > 1) arg = argv[1];

    if (arg == "correctness")
	return test_correctness<n, k>();

    else if (arg == "print")
	return print_test<32>();

    std::cout << "Usage: " << argv[0] << " <correctness|print>" << std::endl;
}
// vim: set sw=4 sts=4 ts=8 noet:
