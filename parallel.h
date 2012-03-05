#ifndef __PARALLEL_H__
#define __PARALLEL_H__

#include <boost/thread.hpp>
#include <memory>

#include "random.h"
#include "solve.h"

struct parallel_result {
    assignment_t solution;
    weight_t goodness;
    size_t times;

    inline parallel_result(const assignment_t & solution, const weight_t & goodness, size_t times)
	: solution(solution)
	, goodness(goodness)
	, times(times)
    {
    }
};

struct parallel_buffer {
    boost::mutex m;
    boost::condition_variable cond;
    std::unique_ptr<parallel_result> held_result;

    inline const parallel_result & fetch() {
	return *held_result;
    }

    inline void send_result(const parallel_result & source) {
	boost::unique_lock<boost::mutex> lock(m);
	if (held_result.get() && source.goodness <= held_result->goodness) return;
	held_result.reset(new parallel_result(source));
	cond.notify_one();
	lock.unlock();
    }
};

struct parallel_reporter {
    inline parallel_reporter(parallel_buffer & b)
	: b(b), times(0)
    {
    }

    inline void start() {
    }

    inline void operator++() { ++times; }
    inline void operator++(int) { ++times; }
    inline void operator+=(size_t s) { times += s; }

    inline void operator()(const input_t & /*input*/, const assignment_t & solution, const weight_t & goodness) {
	parallel_result res(solution, goodness, times);
	b.send_result(res);
    }

private:
    parallel_buffer & b;
    size_t times;
};

struct parallel_solver {
    template <typename Objective>
    inline void operator()(const input_t & input, Objective & obj, parallel_reporter & rep, uint32_t seed) {
	solver_t<Objective, parallel_reporter> solver(input, obj, rep, seed);
	solver.go();
    }
};

template <typename Objective, typename Reporter>
inline void parallel_solve(const input_t & input, Objective & obj, Reporter & reporter) {
    parallel_buffer buf;
    parallel_reporter rep(buf);
    reporter.start();

    unsigned int t = boost::thread::hardware_concurrency();
    boost::thread threads[t];
    randutil rand;
    for (unsigned int i = 0; i < t; ++i) {
	parallel_solver p;
	boost::thread th(p, input, obj, rep, rand.random_source());
	boost::swap(th, threads[i]);
    }

    size_t prev_times = 0;
    while (true) {
	boost::unique_lock<boost::mutex> lock(buf.m);
	buf.cond.wait(lock);
	parallel_result res = buf.fetch();
	reporter += t*(res.times-prev_times);
	prev_times = res.times;
	reporter(input, res.solution, res.goodness);
    }
}

#endif // __PARALLEL_H__
// vim: set sw=4 sts=4 ts=8 noet:
