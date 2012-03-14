#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"
#include "solve.h"
#include "io.h"
#include "objective.h"
#include "parallel.h"

template <typename Reporter>
void go(input_t & input, bool parallel) {
    Reporter reporter;
    obj_goodness objective;
    if (parallel)
	parallel_solve(input, objective, reporter);
    else
	solve(input, objective, reporter);
}

int main(int argc, char ** argv) {
    bool parallel = true;
    bool use_debug_reporter = false;
    for (int i = 1; i < argc; ++i) {
	std::string arg(argv[i]);
	if (arg == "-s" || arg == "--sequential") {
	    parallel = false;
	} else if (arg == "-d" || arg == "--debug") {
	    use_debug_reporter = true;
	}
    }
    input_t input = csv_parser().parse_csv_input();

    if (use_debug_reporter)
	go<debug_reporter>(input, parallel);
    else
	go<cout_reporter>(input, parallel);

    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
