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

    std::cout << "Usage: " << argv[0] << " <pq_speed|tourney_speed>" << std::endl;
}
// vim: set sw=4 sts=4 ts=8 noet:
