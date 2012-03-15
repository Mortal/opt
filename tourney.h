#ifndef __TOURNEY_H__
#define __TOURNEY_H__

#include <algorithm>
#include <boost/integer/static_log2.hpp>
#include <limits>
#include <iostream>
#include <iomanip>

typedef size_t index_t;

template <typename T, typename Compare>
struct tournament_tree_printer;

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct lineprinter;

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct tournament_tree_round;

template <typename T, typename Compare, size_t Capacity>
struct tournament_tree_round<T, 0, Compare, Capacity> {
    static const size_t contestants = 1;

    inline tournament_tree_round(Compare comp, T initial)
	: comp(comp)
	, m_entry(initial)
    {
    }

    inline T winner() {
	return m_entry;
    }

    inline void set(size_t, const T & entry) {
	m_entry = entry;
    }

    inline bool compare(const T & index1, const T & index2) const {
	return comp(index1, index2);
    }

    inline T max(const T & lhs, const T & rhs) const {
	return compare(lhs, rhs) ? rhs : lhs;
    }

private:
    Compare comp;
    T m_entry;
    friend struct lineprinter<T, 0, Compare, Capacity>;
};

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct tournament_tree_round {
    static const size_t contestants = 1 << Round;

    inline tournament_tree_round(Compare comp, T initial)
	: next_contestant(0)
	, inner(comp, initial)
    {
	std::fill(m_entries+0, m_entries+contestants, initial);
    }

    inline T winner() {
	return inner.winner();
    }

    inline void set(size_t index, const T & entry) {
	assert(index < contestants);
	m_entries[index] = entry;
	size_t other = index ^ 1;
	inner.set(index >> 1, max(m_entries[index], m_entries[other]));
    }

    inline T max(const T & lhs, const T & rhs) const {
	return inner.max(lhs, rhs);
    }

private:
    size_t next_contestant;
    T m_entries[contestants];
    tournament_tree_round<T, Round-1, Compare, Capacity> inner;

    friend struct lineprinter<T, Round, Compare, Capacity>;
};

template <typename T, size_t Capacity, typename Compare = std::less<T> >
struct tournament_tree {
    static const size_t rounds = boost::static_log2<Capacity-1>::value+1;
    typedef size_t index_t;

    struct key_comp {
	inline bool operator()(const T * lhs, const T * rhs) const {
	    return Compare()(*lhs, *rhs);
	}
    };

    typedef const T * iterator_type;

    inline T worst() {
	return m_worst;
    }

    inline tournament_tree(const T & worst = std::numeric_limits<T>::min())
	: next_contestant(0)
	, inner(key_comp(), keys+0)
	, m_worst(worst)
    {
    }

    inline void push(const T & k) {
	keys[next_contestant] = k;
	inner.set(next_contestant, keys+next_contestant);
	++next_contestant;
    }

    inline T top() {
	return *inner.winner();
    }

    inline void pop() {
	size_t index = winner_index();
	keys[index] = worst();
	inner.set(index, keys+index);
    }

    inline void replace_top(const T & k) {
	index_t index = winner_index();
	keys[index] = k;
	inner.set(index, keys+index);
    }

    inline iterator_type begin() const { return keys+0; }
    inline iterator_type end()   const { return keys+Capacity; }

    void print() {
	/*
	tournament_tree_printer<T, Compare> p;
	p.print(*this);
	*/
    }

private:
    size_t next_contestant;
    T keys[Capacity];
    tournament_tree_round<const T *, rounds, key_comp, Capacity> inner;
    T m_worst;

    inline index_t winner_index() {
	return inner.winner()-keys;
    }

    friend struct tournament_tree_printer<T, Compare>;
};

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct lineprinter {

    static inline void printline(std::ostream & os, tournament_tree_round<T, Round, Compare, Capacity> & t, size_t index) {
	os << std::setw(16) << *(t.m_entries[index]);
	if (index & 1) return;
	lineprinter<T, Round-1, Compare, Capacity>::printline(os, t.inner, index >> 1);
    }

};

template <typename T, typename Compare, size_t Capacity>
struct lineprinter<T, 0, Compare, Capacity> {

    static inline void printline(std::ostream & os, tournament_tree_round<T, 0, Compare, Capacity> & t, size_t /*index*/) {
	os << std::setw(16) << *(t.m_entry);
    }

};

template <typename T, typename Compare>
struct tournament_tree_printer {
    inline tournament_tree_printer(std::ostream & os)
	: os(os)
    {
    }

    template <size_t Capacity>
    inline void print(tournament_tree<T, Capacity, Compare> & t) {
	print(t.inner);
    }

    template <size_t Round, size_t Capacity, typename RoundCompare>
    inline void print(tournament_tree_round<const T *, Round, RoundCompare, Capacity> & t) {
	for (size_t i = 0; i < t.contestants; ++i) {
	    lineprinter<const T *, Round, RoundCompare, Capacity>::printline(os, t, i);
	    os << '\n';
	}
    }

private:
    std::ostream & os;
};

template <typename T, size_t Capacity, typename Compare>
inline std::ostream & operator<<(std::ostream & os, tournament_tree<T, Capacity, Compare> & t) {
    tournament_tree_printer<T, Compare> printer(os);
    printer.print(t);
    return os;
}

#endif // __TOURNEY_H__
// vim: set sw=4 sts=4 ts=8 noet:
