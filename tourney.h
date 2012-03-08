#ifndef __TOURNEY_H__
#define __TOURNEY_H__

#include <algorithm>
#include <boost/integer/static_log2.hpp>
#include <limits>
#include <iostream>
#include <iomanip>

typedef size_t index_t;

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct tournament_tree_round;

template <typename T, typename Compare, size_t Capacity>
struct tournament_tree_round<T, 0, Compare, Capacity> {
    typedef std::pair<index_t, T> winner_type;
    static const size_t contestants = 1;

    inline winner_type winner() {
	return std::make_pair(m_entry, keys[m_entry]);
    }

    inline void set(size_t, index_t entry) {
	m_entry = entry;
    }

    inline void set_key(index_t index, const T & entry) {
	keys[index] = entry;
    }

    inline const T & key(index_t index) const { return keys[index]; }

    inline bool compare(index_t index1, index_t index2) const {
	return Compare()(keys[index1], keys[index2]);
    }

    inline index_t max(index_t lhs, index_t rhs) const {
	return compare(lhs, rhs) ? rhs : lhs;
    }

    inline const T * begin() const { return keys+0; }
    inline const T * end()   const { return keys+Capacity; }

    inline void printline(size_t) {
	std::cout << std::setw(16) << key(m_entry);
    }

    inline void print() {
	printline();
	std::cout << std::endl;
    }

private:
    T keys[Capacity];
    index_t m_entry;
};

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct tournament_tree_round {
    typedef std::pair<index_t, T> winner_type;
    static const size_t contestants = 1 << Round;

    inline tournament_tree_round()
	: next_contestant(0)
    {
    }

    inline winner_type winner() {
	return inner.winner();
    }

    inline void set(size_t index, index_t entry) {
	assert(index < contestants);
	m_entries[index] = entry;
	size_t other = index ^ 1;
	inner.set(index >> 1, max(m_entries[index], m_entries[other]));
    }

    inline void set_key(index_t key, const T & entry) {
	return inner.set_key(key, entry);
    }

    inline bool compare(index_t index1, index_t index2) const {
	return inner.compare(index1, index2);
    }

    inline index_t max(index_t lhs, index_t rhs) const {
	return inner.max(lhs, rhs);
    }

    inline void printline(size_t index) {
	std::cout << std::setw(16) << key(m_entries[index]);
	if (index & 1) return;
	inner.printline(index >> 1);
    }

    inline void print() {
	for (size_t i = 0; i < contestants; ++i) {
	    printline(i);
	    std::cout << std::endl;
	}
    }

    inline const T & key(index_t index) const { return inner.key(index); }

    inline const T * begin() const { return inner.begin(); }
    inline const T * end()   const { return inner.end(); }

private:
    size_t next_contestant;
    index_t m_entries[contestants];
    tournament_tree_round<T, Round-1, Compare, Capacity> inner;
};

template <typename T, size_t Capacity, typename Compare = std::less<T> >
struct tournament_tree {
    static const size_t rounds = boost::static_log2<Capacity-1>::value+1;
    typedef size_t index_t;

    inline T worst() {
	return m_worst;
    }

    inline tournament_tree(const T & worst = std::numeric_limits<T>::min())
	: next_contestant(0)
	, m_worst(worst)
    {
    }

    inline void push(const T & k) {
	inner.set_key(next_contestant, k);
	inner.set(next_contestant, next_contestant);
	++next_contestant;
    }

    inline T top() {
	return inner.winner().second;
    }

    inline void pop() {
	size_t index = winner_index();
	inner.set_key(index, worst());
	inner.set(index, index);
    }

    inline void print() {
	inner.print();
    }

    inline void replace_top(const T & k) {
	index_t index = winner_index();
	inner.set_key(index, k);
	inner.set(index, index);
    }

    inline const T * begin() const { return inner.begin(); }
    inline const T * end()   const { return inner.end(); }

private:
    size_t next_contestant;
    tournament_tree_round<T, rounds, Compare, Capacity> inner;
    T m_worst;

    inline index_t winner_index() {
	return inner.winner().first;
    }
};

#endif // __TOURNEY_H__
// vim: set sw=4 sts=4 ts=8 noet:
