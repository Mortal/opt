#include <boost/random.hpp>
#include "random.h"

boost::rand48 random_source;

int rand_less_than(int bound) {
    boost::uniform_int<> dist(0,bound-1);
    boost::variate_generator<boost::rand48&, boost::uniform_int<> > rng(random_source, dist);
    return rng();
}

bool flip_coin() {
    boost::uniform_smallint<> dist(0,1);
    return dist(random_source);
}

std::vector<char> random_mask(int m, int n) {
    std::vector<char> mask(n, 0);
    std::fill(mask.begin(), mask.begin()+m, 1);
    shuffle(mask.begin(), mask.end());
    return mask;
}

// vim: set sw=4 sts=4 ts=8 noet:
