#ifndef __TOURNEY_H__
#define __TOURNEY_H__

#include <algorithm>
#include <boost/integer/static_log2.hpp>
#include <limits>
#include <iostream>
#include <iomanip>

template <typename T, size_t Round, typename Compare>
struct tournament_tree_round;

template <typename T, typename Compare>
struct tournament_tree_round<T, 0, Compare> {
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

template <typename T, size_t Round, typename Compare>
struct tournament_tree_round {
    static const size_t contestants = 1 << Round;

    inline tournament_tree_round()
	: next_contestant(0)
    {
    }

    inline void insert(const T & entry) {
	m_entries[next_contestant++] = entry;
	if (next_contestant & 1) return;
	inner.insert(max(m_entries[next_contestant-2], entry));
    }

    inline const T & winner() {
	return inner.winner();
    }

    inline void set(size_t index, const T & entry) {
	assert(index < contestants);
	m_entries[index] = entry;
	size_t other = index ^ 1;
	inner.set(index >> 1, max(m_entries[index], m_entries[other]));
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

    inline const T * begin() const { return m_entries + 0; }
    inline const T * end()   const { return m_entries + contestants; }

private:
    size_t next_contestant;
    T m_entries[contestants];
    tournament_tree_round<T, Round-1, Compare> inner;

    inline const T & max(const T & lhs, const T & rhs) {
	return Compare()(lhs, rhs) ? rhs : lhs;
    }
};

template <typename Compare>
struct compare_first {
    template <typename First, typename Second>
    inline bool operator()(const std::pair<First, Second> & lhs, const std::pair<First, Second> & rhs) {
	return Compare()(lhs.first, rhs.first);
    }
};

template <typename T, size_t Capacity, typename Compare>
struct tournament_tree_iter_base {
    tournament_tree_iter_base(const std::pair<T, size_t> * from)
	: it(from)
    {
    }

    const T & operator*()  const { return it->first; }
    const T * operator->() const { return &(it->first); }
    void operator++() { ++it; }
    void operator++(int) { it++; }
    bool operator==(const tournament_tree_iter_base & other) const {
	return it == other.it;
    }
    bool operator!=(const tournament_tree_iter_base & other) const {
	return !(*this == other);
    }
private:
    const std::pair<T, size_t> * it;
};

template <typename T, size_t Capacity, typename Compare = std::less<T> >
struct tournament_tree {
    static const size_t rounds = boost::static_log2<Capacity-1>::value+1;
    typedef size_t index_t;
    typedef tournament_tree_iter_base<T, Capacity, Compare> iter_base;

    inline T worst() {
	return m_worst;
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

    inline void replace_top(const T & k) {
	size_t index = winner_index();
	inner.set(index, std::make_pair(k, index));
    }

    inline iter_base begin() const { return inner.begin(); }
    inline iter_base end()   const { return inner.end(); }

private:
    size_t next_contestant;
    tournament_tree_round<std::pair<T, index_t>, rounds, compare_first<Compare> > inner;
    T values[Capacity];
    T m_worst;

    inline size_t winner_index() {
	return inner.winner().second;
    }
};

namespace std {
    template <typename T, size_t Capacity, typename Compare>
    struct iterator_traits<tournament_tree_iter_base<T, Capacity, Compare> > {
	typedef T value_type;
    };
}

#endif // __TOURNEY_H__
// vim: set sw=4 sts=4 ts=8 noet:
