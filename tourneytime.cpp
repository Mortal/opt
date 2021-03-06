#include <boost/random.hpp>
#include <iostream>
#include <queue>
#include <sstream>
#include "tourney.h"
#include "speedtest.h"
#include "stream_max.h"
#include "static_heap.h"
#include "replacer.h"

typedef int32_t item_type;
typedef uint64_t N;

template <typename Tree, size_t Capacity>
struct tourney_timer {
	boost::mt19937 rng;
	Tree tree;

	inline tourney_timer() {
		for (N i = 0; i < Capacity; ++i) {
			tree.push(0);
		}
	}

	inline void operator()(N n) {
		for (N i = 0; i < n; ++i) {
			replacer<Tree>::replace_top(&tree, rng());
		}
	}
};

template <typename Tree, size_t Capacity, typename Logger>
double sample(Logger & logger, size_t samples = 4, double calibrate = 0.5, double duration = 0.5) {
	typedef tourney_timer<Tree, Capacity> Timer;
	Timer f;
	speedtester<Timer &, N> tester(f);
	N n = tester.calibrate(calibrate, logger) * duration / calibrate;
	return n/tester.sample(n, samples, logger).mean();
}

static const size_t colwidth = 20;
static const size_t headerwidth = 20;

template <size_t Capacity, typename Results, typename Logger>
void test(Results & results, Logger & logger) {
	results << std::setw(4) << Capacity << std::flush;
	{
	double speed = sample<tournament_tree<item_type, Capacity>, Capacity>(logger);
	logger << std::setw(headerwidth) << "Tournament tree: " << std::fixed << speed << " elements per second" << std::endl;
	results << std::setw(colwidth) << static_cast<int64_t>(speed) << std::flush;
	}
	{
	double speed = sample<static_heap<item_type, Capacity>, Capacity>(logger);
	logger << std::setw(headerwidth) << "Static heap: " << std::fixed << speed << " elements per second" << std::endl;
	results << std::setw(colwidth) << static_cast<int64_t>(speed) << std::flush;
	}
	{
	double speed = sample<std::priority_queue<item_type>, Capacity>(logger);
	logger << std::setw(headerwidth) << "STL priority_queue: " << std::fixed << speed << " elements per second" << std::endl;
	results << std::setw(colwidth) << static_cast<int64_t>(speed) << std::flush;
	}
	{
	double speed = sample<streaming_max<item_type, Capacity>, Capacity>(logger);
	logger << std::setw(headerwidth) << "Streaming max: " << std::fixed << speed << " elements per second" << std::endl;
	results << std::setw(colwidth) << static_cast<int64_t>(speed) << std::flush;
	}
	results << std::endl;
}

template <typename Results, typename Logger>
void all_tests(Results & results, Logger & logger) {
	results << "   k"
		<< std::setw(colwidth) << "Tournament tree"
		<< std::setw(colwidth) << "Static heap"
		<< std::setw(colwidth) << "STL priority_queue"
		<< std::setw(colwidth) << "Unsorted array" << std::endl;
	test<2>    (results, logger);
	test<4>    (results, logger);
	test<8>    (results, logger);
	test<16>   (results, logger);
	test<32>   (results, logger);
	test<64>   (results, logger);
	test<128>  (results, logger);
	test<256>  (results, logger);
	test<512>  (results, logger);
	test<1024> (results, logger);
	test<2048> (results, logger);
	test<4096> (results, logger);
	test<8192> (results, logger);
	test<16384>(results, logger);
}

int main(int argc, char ** argv) {
	if (argc > 1 && std::string(argv[1]) == "-q") {
		std::stringstream devnull;
		all_tests(std::cout, devnull);
	} else {
		std::stringstream results;
		all_tests(results, std::cerr);
		std::cout << results.str() << std::endl;
	}
	return 0;
}
