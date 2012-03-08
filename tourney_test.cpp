#include <iostream>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <queue>
#include <ctime>
#include "tourney.h"

template <typename RNG>
int pq_test(size_t n, size_t k, RNG & rng) {
    boost::timer::auto_cpu_timer t;
    std::priority_queue<size_t> q;
    for (size_t i = 0; i < n; ++i) {
	q.push(rng());
    }
    size_t res = 0;
    for (size_t i = 0; i < k; ++i) {
	res ^= q.top();
	q.pop();
    }
    return res % 128;
}

int main(int, char ** argv) {
    boost::mt19937 rng(time(NULL));
    const static size_t n = 1 << 25;
    const static size_t k = n >> 7;
    // For n = 2^25, k = 2^19, tournament tree fares better.
    // For n = 2^25, k = 2^18, priority queue fares better.
    if (std::string(argv[0]) == "./pq_test") return pq_test(n, k, rng);
    boost::timer::auto_cpu_timer t;
    auto * tree = new tournament_tree<size_t, size_t, n>();
    for (size_t i = 0; i < n; ++i) {
	size_t k = rng();
	tree->insert(k, k);
    }
    size_t res = 0;
    for (size_t i = 0; i < k; ++i) {
	res ^= tree->winner();
	tree->pop_winner();
    }
    return res % 128;
}
// vim: set sw=4 sts=4 ts=8 noet:
