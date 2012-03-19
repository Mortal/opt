#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"
#include "solve.h"
#include "io.h"
#include "objective.h"
#include "parallel.h"
#include "csv.h"

template <typename Reporter>
void go(input_t & input, bool parallel, bool delayed, Reporter reporter = Reporter()) {
    obj_goodness objective;
    if (parallel) {
	if (delayed) {
	    parallel_solve<buffer_report_delayed>(input, objective, reporter);
	} else {
	    parallel_solve<buffer_report_immediate>(input, objective, reporter);
	}
    } else {
	solve(input, objective, reporter);
    }
}

int main(int argc, char ** argv) {
    bool parallel = true;
    bool use_debug_reporter = false;
    bool csv = false;
    bool delayed = true;
    for (int i = 1; i < argc; ++i) {
	std::string arg(argv[i]);
	if (arg == "-s" || arg == "--sequential") {
	    parallel = false;
	} else if (arg == "-d" || arg == "--debug") {
	    use_debug_reporter = true;
	} else if (arg == "-c" || arg == "--csv") {
	    csv = true;
	} else if (arg == "--immediate") {
	    delayed = false;
	}
    }
    std::auto_ptr<input_t> input;
    std::auto_ptr<csv_parser> p;
    if (csv) {
	p.reset(new csv_parser());
	input.reset(new input_t(p->parse_csv_input()));
    } else {
	input.reset(new input_t(get_input()));
    }

    if (use_debug_reporter)
	go<debug_reporter>(*input, parallel, delayed);
    else if (csv)
	go<csv_reporter>(*input, parallel, delayed, csv_reporter(*p));
    else
	go<cout_reporter>(*input, parallel, delayed);

    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
