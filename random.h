#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <vector>

int rand_less_than(int bound);

std::vector<char> random_mask(int m, int n);

bool flip_coin();

template <typename IT>
void shuffle(IT begin, IT end) {
    const int n = end-begin;
    for (int i = n; i--;) {
	std::iter_swap(begin+i, begin+rand_less_than(i+1));
    }
}

#endif // __RANDOM_H__
// vim: set sw=4 sts=4 ts=8 noet:
