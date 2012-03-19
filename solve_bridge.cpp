#include "io.h"
#include "csv.h"
#include "solve.h"
#include "parallel.h"

#define fn(Objective, Reporter) \
    void solve ## Objective ## Reporter(const input_t & input, Objective & objective, Reporter & reporter) { \
	solve(input, objective, reporter); \
    }

#define pfn(BufferReporter, Objective, Reporter) \
    void parallel_solve ## BufferReporter ## Objective ## Reporter(const input_t & input, Objective & objective, Reporter & reporter) { \
	parallel_solve<BufferReporter>(input, objective, reporter); \
    }

#include "solve_bridge.inl"

#undef fn
#undef pfn

// vim: set sw=4 sts=4 ts=8 noet:
