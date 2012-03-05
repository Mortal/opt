#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <vector>
#include <boost/random.hpp>

struct randutil {

boost::mt19937 random_source;

template <typename IT>
void shuffle(IT begin, IT end) {
    const int n = end-begin;
    for (int i = n; i--;) {
	std::iter_swap(begin+i, begin+rand_less_than(i+1));
    }
}

int rand_less_than(int bound) {
    boost::uniform_smallint<> dist(0,bound-1);
    boost::variate_generator<boost::mt19937&, boost::uniform_smallint<> > rng(random_source, dist);
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

};

#endif // __RANDOM_H__
// vim: set sw=4 sts=4 ts=8 noet:
