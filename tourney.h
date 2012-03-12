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
    friend struct lineprinter<T, 0, Compare, Capacity>;
};

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct tournament_tree_round {
    typedef std::pair<index_t, T> winner_type;
    static const size_t contestants = 1 << Round;

    inline tournament_tree_round()
	: next_contestant(0)
    {
	std::fill(m_entries+0, m_entries+contestants, 0);
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

    inline const T & key(index_t index) const { return inner.key(index); }

    inline const T * begin() const { return inner.begin(); }
    inline const T * end()   const { return inner.end(); }

private:
    size_t next_contestant;
    index_t m_entries[contestants];
    tournament_tree_round<T, Round-1, Compare, Capacity> inner;

    friend struct lineprinter<T, Round, Compare, Capacity>;
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

    inline void replace_top(const T & k) {
	index_t index = winner_index();
	inner.set_key(index, k);
	inner.set(index, index);
    }

    inline const T * begin() const { return inner.begin(); }
    inline const T * end()   const { return inner.end(); }

    void print() {
	tournament_tree_printer<T, Compare> p;
	p.print(*this);
    }

private:
    size_t next_contestant;
    tournament_tree_round<T, rounds, Compare, Capacity> inner;
    T m_worst;

    inline index_t winner_index() {
	return inner.winner().first;
    }

    friend struct tournament_tree_printer<T, Compare>;
};

template <typename T, size_t Round, typename Compare, size_t Capacity>
struct lineprinter {

    static inline void printline(std::ostream & os, tournament_tree_round<T, Round, Compare, Capacity> & t, size_t index) {
	os << std::setw(16) << t.key(t.m_entries[index]);
	if (index & 1) return;
	lineprinter<T, Round-1, Compare, Capacity>::printline(os, t.inner, index >> 1);
    }

};

template <typename T, typename Compare, size_t Capacity>
struct lineprinter<T, 0, Compare, Capacity> {

    static inline void printline(std::ostream & os, tournament_tree_round<T, 0, Compare, Capacity> & t, size_t /*index*/) {
	os << std::setw(16) << t.key(t.m_entry);
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

    template <size_t Round, size_t Capacity>
    inline void print(tournament_tree_round<T, Round, Compare, Capacity> & t) {
	for (size_t i = 0; i < t.contestants; ++i) {
	    lineprinter<T, Round, Compare, Capacity>::printline(os, t, i);
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
