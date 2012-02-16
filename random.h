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

template <typename C>
struct shuffled_iterator {
    shuffled_iterator(const std::vector<C> & source)
	: items(source)
	, n(items.size())
	, indexes(n)
	, index(0)
    {
	for (size_t i = 0; i < n; ++i) {
	    indexes[i] = i;
	}
	std::iter_swap(items.begin(), items.begin()+rand_less_than(n));
    }
    const C & operator*() {
	return items[index];
    }
    void operator++(int) { ++*this; }
    void operator++() {
	++index;
	std::iter_swap(items.begin()+index, items.begin()+index+rand_less_than(n)-1);
    }
private:
    std::vector<C> items;
    size_t n;
    std::vector<int> indexes;
    size_t index;
};

#endif // __RANDOM_H__
// vim: set sw=4 sts=4 ts=8 noet:
