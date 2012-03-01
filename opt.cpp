#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"
#include "solve.h"
#include "io.h"
#include "objective.h"

int main() {
    size_t rooms = 0;
    size_t person_count = 0;
    input_t input = get_input(rooms, person_count);

    obj_goodness objective;
    solve(input, objective);
    return 0;
}

// vim: set sw=4 sts=4 ts=8 noet:
