#ifndef __TOURNEY_H__
#define __TOURNEY_H__

#include <algorithm>
#include <boost/integer/static_log2.hpp>
#include <limits>

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

    inline void set(size_t /*index*/, const entry_type & entry) {
	m_entry = entry;
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

    inline void set(size_t index, const entry_type & entry) {
	if (index >= contestants) {
	    std::cout << "Level " << Round << ": " << index << " >= " << contestants << std::endl;
	}
	assert(index < contestants);
	m_entries[index] = entry;
	size_t other = index ^ (index & 1);
	inner.set(index >> 1, std::max(m_entries[index], m_entries[other]));
    }

private:
    size_t next_contestant;
    entry_type m_entries[contestants];
    tournament_tree_round<Key, Value, Round-1> inner;
};

template <typename Key, typename Value, size_t Capacity>
struct tournament_tree {
    static const size_t rounds = boost::static_log2<Capacity-1>::value+1;
    typedef size_t inner_value;

    inline Key worst() {
	return std::numeric_limits<Key>::min();
    }

    inline tournament_tree()
	: next_contestant(0)
	, values(Capacity)
    {
    }

    inline void insert(const Key & k, const Value & v) {
	inner.insert(std::make_pair(k,next_contestant));
	values[next_contestant++] = v;
    }

    inline const Value & winner() {
	size_t idx = winner_index();
	return values[idx];
    }

    inline void pop_winner() {
	size_t index = winner_index();
	inner.set(index, std::make_pair(worst(), index));
    }

private:
    size_t next_contestant;
    tournament_tree_round<Key, inner_value, rounds> inner;
    std::vector<Value> values;

    inline size_t winner_index() {
	return inner.winner().second;
    }
};

#endif // __TOURNEY_H__
// vim: set sw=4 sts=4 ts=8 noet:
