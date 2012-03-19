#ifndef __PARALLEL_H__
#define __PARALLEL_H__

#include <boost/thread.hpp>
#include <memory>
#include <ctime>

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

/** Passing solutions between threads, filtering to make sure reported
 * solutions are monotonically increasing in goodness. */
struct parallel_buffer {
    boost::mutex m;
    /** Signalled when there is a new result. */
    boost::condition_variable cond;
    std::unique_ptr<parallel_result> held_result;
    bool has_result;

    inline parallel_buffer()
	: has_result(false)
    {
    }

    /** Caller must hold the mutex. */
    inline const parallel_result & fetch() {
	has_result = false;
	return *held_result;
    }

    /** Caller must not hold the mutex. */
    inline void send_result(const parallel_result & source) {
	boost::unique_lock<boost::mutex> lock(m);
	if (held_result.get() && source.goodness <= held_result->goodness) return;
	has_result = true;
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

template <template <typename Reporter> class BufferReporter, typename Reporter, typename Objective>
inline void parallel_solve(const input_t & input, Objective & obj, Reporter & reporter) {
    parallel_buffer buf;
    parallel_reporter rep(buf);
    BufferReporter<Reporter> brep(reporter, input, buf);

    unsigned int t = boost::thread::hardware_concurrency();
    boost::thread * threads = new boost::thread[t];
    randutil rand(time(NULL));
    uint32_t seed = rand.random_source();
    for (unsigned int i = 0; i < t; ++i) {
	parallel_solver p;
	boost::thread th(p, input, obj, rep, seed+i);
	boost::swap(th, threads[i]);
    }

    brep();
    delete[] threads;
}

template <typename Reporter>
struct buffer_report_immediate {

    inline buffer_report_immediate(const Reporter & reporter, const input_t & input, parallel_buffer & buf)
	: prev_times(0)
	, buf(buf)
	, input(input)
	, reporter(reporter)
    {
    }

    inline void operator()() {
	reporter.start();
	while (true) {
	    boost::unique_lock<boost::mutex> lock(buf.m);
	    while (!buf.has_result) buf.cond.wait(lock);
	    parallel_result res = buf.fetch();
	    reporter += res.times-prev_times;
	    prev_times = res.times;
	    reporter(input, res.solution, res.goodness);
	}
    }

private:
    size_t prev_times;
    parallel_buffer & buf;
    input_t input;
    Reporter reporter;
};

template <typename Reporter>
struct buffer_report_delayed {

    inline buffer_report_delayed(const Reporter & reporter, const input_t & input, parallel_buffer & buf)
	: prev_times(0)
	, buf(buf)
	, input(input)
	, reporter(reporter)
    {
    }

    inline void operator()() {
	std::unique_ptr<parallel_result> held_result;
	boost::unique_lock<boost::mutex> lock(buf.m);
	while (true) {
	    while (!buf.has_result) {
		if (held_result.get()) {
		    if (!buf.cond.timed_wait(lock, boost::get_system_time() + boost::posix_time::milliseconds(1000))) {
			// We timed out.
			lock.unlock();
			reporter += held_result->times - prev_times;
			prev_times = held_result->times;
			reporter(input, held_result->solution, held_result->goodness);
			lock.lock();
			held_result.reset(0);
		    }
		} else {
		    buf.cond.wait(lock);
		}
	    }
	    held_result.reset(new parallel_result(buf.fetch()));
	}
    }

private:
    size_t prev_times;
    parallel_buffer & buf;
    input_t input;
    Reporter reporter;
};

#endif // __PARALLEL_H__
// vim: set sw=4 sts=4 ts=8 noet:
