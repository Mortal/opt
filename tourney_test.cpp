#include <iostream>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <queue>
#include <ctime>
#include "tourney.h"

template <typename RNG>
int pq_test(size_t n, RNG & rng) {
    boost::timer::auto_cpu_timer t;
    std::priority_queue<size_t> q;
    for (size_t i = 0; i < n; ++i) {
	q.push(rng());
    }
    return q.top() % 128;
}

int main(int argc, char ** argv) {
    boost::mt19937 rng(time(NULL));
    const static size_t n = 1 << 25;
    if (std::string(argv[0]) == "./pq_test") return pq_test(n, rng);
    boost::timer::auto_cpu_timer t;
    auto * tree = new tournament_tree<size_t, size_t, n>();
    for (size_t i = 0; i < n; ++i) {
	size_t k = rng();
	tree->insert(k, k);
    }
    return tree->winner() % 128;
}
// vim: set sw=4 sts=4 ts=8 noet:
