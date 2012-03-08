#ifndef __TOURNEY_H__
#define __TOURNEY_H__

#include <algorithm>
#include <boost/integer/static_log2.hpp>
#include <limits>
#include <iostream>
#include <iomanip>

template <typename T, size_t Round>
struct tournament_tree_round;

template <typename T>
struct tournament_tree_round<T, 0> {
    static const size_t contestants = 1;

    inline void insert(const T & entry) {
	m_entry = entry;
    }

    inline const T & winner() {
	return m_entry;
    }

    inline void set(size_t /*index*/, const T & entry) {
	m_entry = entry;
    }

    inline void printline(size_t) {
	std::cout << std::setw(16) << m_entry.first;
    }

    inline void print() {
	printline();
	std::cout << std::endl;
    }

private:
    T m_entry;
};

template <typename T, size_t Round>
struct tournament_tree_round {
    static const size_t contestants = 1 << Round;

    inline tournament_tree_round()
	: next_contestant(0)
    {
    }

    inline void insert(const T & entry) {
	m_entries[next_contestant++] = entry;
	if (next_contestant & 1) return;
	inner.insert(std::max(m_entries[next_contestant-2], entry));
    }

    inline const T & winner() {
	return inner.winner();
    }

    inline void set(size_t index, const T & entry) {
	assert(index < contestants);
	m_entries[index] = entry;
	size_t other = index ^ 1;
	inner.set(index >> 1, std::max(m_entries[index], m_entries[other]));
    }

    inline void printline(size_t index) {
	std::cout << std::setw(16) << m_entries[index].first;
	if (index & 1) return;
	inner.printline(index >> 1);
    }

    inline void print() {
	for (size_t i = 0; i < contestants; ++i) {
	    printline(i);
	    std::cout << std::endl;
	}
    }

private:
    size_t next_contestant;
    T m_entries[contestants];
    tournament_tree_round<T, Round-1> inner;
};

template <typename T, size_t Capacity>
struct tournament_tree {
    static const size_t rounds = boost::static_log2<Capacity-1>::value+1;
    typedef size_t index_t;

    inline T worst() {
	return std::numeric_limits<T>::min();
    }

    inline tournament_tree(const T & worst = std::numeric_limits<T>::min())
	: next_contestant(0)
	, m_worst(worst)
    {
    }

    inline void push(const T & k) {
	inner.insert(std::make_pair(k,next_contestant));
	values[next_contestant++] = k;
    }

    inline const T & top() {
	size_t idx = winner_index();
	return values[idx];
    }

    inline void pop() {
	size_t index = winner_index();
	inner.set(index, std::make_pair(worst(), index));
    }

    inline void print() {
	inner.print();
    }

private:
    size_t next_contestant;
    tournament_tree_round<std::pair<T, index_t>, rounds> inner;
    T values[Capacity];
    T m_worst;

    inline size_t winner_index() {
	return inner.winner().second;
    }
};

#endif // __TOURNEY_H__
// vim: set sw=4 sts=4 ts=8 noet:
