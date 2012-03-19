#ifndef __SOLVE_BRIDGE_H__
#define __SOLVE_BRIDGE_H__

#include "io.h"
#include "csv.h"

#define fn(Objective, Reporter) \
    void solve ## Objective ## Reporter(const input_t & input, Objective & objective, Reporter & reporter);
#define pfn(BufferReporter, Objective, Reporter) \
    void parallel_solve ## BufferReporter ## Objective ## Reporter(const input_t & input, Objective & objective, Reporter & reporter);

#include "solve_bridge.inl"

#undef fn
#undef pfn

template <typename Objective, typename Reporter>
inline void solve(const input_t & input, Objective & obj, Reporter & reporter);

struct buffer_report_immediate;
struct buffer_report_delayed;

template <typename BufferReporter, typename Reporter, typename Objective>
inline void parallel_solve(const input_t & input, Objective & obj, Reporter & reporter);

#define fn(Objective, Reporter) \
    template <> \
    inline void solve<Objective, Reporter>(const input_t & input, Objective & objective, Reporter & reporter) { \
	solve ## Objective ## Reporter(input, objective, reporter); \
    }

#define pfn(BufferReporter, Objective, Reporter) \
    template <> \
    inline void parallel_solve<BufferReporter, Reporter, Objective> (const input_t & input, Objective & objective, Reporter & reporter) { \
	parallel_solve ## BufferReporter ## Objective ## Reporter(input, objective, reporter); \
    }

#include "solve_bridge.inl"

#undef fn
#undef pfn

#endif // __SOLVE_BRIDGE_H__
// vim: set sw=4 sts=4 ts=8 noet:
