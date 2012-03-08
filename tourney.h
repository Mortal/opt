#ifndef __TOURNEY_H__
#define __TOURNEY_H__

#include <algorithm>
#include <boost/integer/static_log2.hpp>

template <typename Key, typename Value, size_t Round>
struct tournament_tree_round;

template <typename Key, typename Value>
struct tournament_tree_round<Key, Value, 0> {
    typedef std::pair<Key, Value> entry_type;
    static const size_t contestants = 1;

    inline void insert(const entry_type & entry) {
	m_entry = entry;
    }

    inline const entry_type & winner() {
	return m_entry;
    }

private:
    entry_type m_entry;
};

template <typename Key, typename Value, size_t Round>
struct tournament_tree_round {
    typedef std::pair<Key, Value> entry_type;
    static const size_t contestants = 1 << Round;

    inline tournament_tree_round()
	: next_contestant(0)
    {
    }

    inline void insert(const entry_type & entry) {
	m_entries[next_contestant++] = entry;
	if (next_contestant & 1) return;
	inner.insert(std::max(m_entries[next_contestant-2], entry));
    }

    inline const entry_type & winner() {
	return inner.winner();
    }

private:
    size_t next_contestant;
    entry_type m_entries[contestants];
    tournament_tree_round<Key, Value, Round-1> inner;
};

template <typename Key, typename Value, size_t Capacity>
struct tournament_tree {
    static const size_t rounds = boost::static_log2<Capacity-1>::value+1;

    inline void insert(const Key & k, const Value & v) {
	inner.insert(std::make_pair(k,v));
    }

    inline const Value & winner() {
	return inner.winner().second;
    }

private:
    tournament_tree_round<Key, Value, rounds> inner;
};

#endif // __TOURNEY_H__
// vim: set sw=4 sts=4 ts=8 noet:
