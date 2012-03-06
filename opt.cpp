#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"
#include "solve.h"
#include "io.h"
#include "objective.h"
#include "parallel.h"

int main(int argc, char ** argv) {
    size_t rooms = 0;
    size_t person_count = 0;
    input_t input = get_input(rooms, person_count);

    obj_goodness objective;
    cout_reporter reporter;
    bool parallel = !(argc > 1 && std::string(argv[1]) == "--sequential");
    if (parallel)
	parallel_solve(input, objective, reporter);
    else
	solve(input, objective, reporter);
    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
