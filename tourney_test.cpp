#include <iostream>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <queue>
#include <ctime>
#include "tourney.h"

template <typename Q, size_t n, size_t k>
int speed_test() {
    boost::mt19937 rng(time(NULL));
    boost::timer::auto_cpu_timer t;
    Q * q = new Q();
    for (size_t i = 0; i < n; ++i) {
	q->push(rng());
    }
    size_t res = 0;
    for (size_t i = 0; i < k; ++i) {
	res ^= q->top();
	q->pop();
    }
    return res % 128;
}

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

int main(int argc, char ** argv) {
    const static size_t n = 1 << 25;
    const static size_t k = n >> 7;
    // For n = 2^25, k = 2^19, tournament tree fares better.
    // For n = 2^25, k = 2^18, priority queue fares better.

    std::string arg;
    if (argc > 1) arg = argv[1];

    if (arg == "pq_speed")
	return speed_test<std::priority_queue<size_t>, n, k>();

    else if (arg == "tourney_speed")
	return speed_test<tournament_tree<size_t, n>, n, k>();

    else if (arg == "correctness")
	return test_correctness<n, k>();

    std::cout << "Usage: " << argv[0] << " <pq_speed|tourney_speed|correctness>" << std::endl;
}
// vim: set sw=4 sts=4 ts=8 noet:
