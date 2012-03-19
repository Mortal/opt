#ifndef __IO_H__
#define __IO_H__

#include <unordered_map>
#include <boost/timer.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include "types.h"

inline input_t get_input() {
    input_t result;
    capacity_t & capacity = result.capacity;
    people_t & people = result.people;
    condition_t & condition = result.condition;

    // Read capacities
    {
	std::string s;
	if (!getline(std::cin, s)) abort();
	std::stringstream tokens(s);
	size_t p;
	while (tokens >> p) {
	    capacity.push_back(p);
	}
    }
    {
	condition.resize(2);
	condition[0].resize(capacity.size(), false);
	condition[1] = condition[0];
	for (dest_t d = 0; d < capacity.size(); ++d) {
	    if (d < 12) condition[0][d] = true;
	    if (d % 4) condition[1][d] = true;
	}
    }
    // Read priorities
    while (true) {
	std::string s;
	if (!getline(std::cin, s)) break;
	std::stringstream tokens(s);
	priorities_t prio;

	tokens >> prio.wp >> prio.w1 >> prio.w2 >> prio.c1 >> prio.c2;

	roomies_t & roomies = prio.roomies;
	person_t p;
	while (tokens >> p) {
	    roomies.insert(p);
	}

	people.push_back(prio);
    }

    return result;
}

struct csv_parser {
private:
    template <typename T>
    inline void _ioerror(const char * file, int line, size_t inputline, T msg) {
	std::cerr << "While parsing input on line " << inputline << ": " << file << ':' << line << "  " << msg << std::endl;
	abort();
    }
#define ioerror(m) _ioerror(__FILE__, __LINE__, inputline, m)

    std::vector<std::string> destination_names;
    std::vector<std::string> person_names;
    std::unordered_map<std::string, size_t> person_ids;

public:

inline input_t parse_csv_input() {
    input_t result;
    std::string line;
    size_t inputline = 0;
    size_t capacity_count = 0;
    size_t person_count = 0;
    std::vector<std::string> roomies;
    size_t capacity_sum = 0;
    while (getline(std::cin, line)) {
	std::cout << line << std::endl;
	++inputline;
	static const boost::regex r_capacity("(\\d+-(St|\\d+)),(\\d+).*");
	boost::smatch m_capacity;
	if (boost::regex_match(line, m_capacity, r_capacity)) {
	    std::cout << "Got destination" << std::endl;
	    if (capacity_count == dest_capacity) ioerror("Too many destinations");
	    destination_names.push_back(m_capacity.str(1));
	    result.capacity.push_back(boost::lexical_cast<size_t>(m_capacity.str(3)));
	    capacity_sum += result.capacity[capacity_count];
	    result.condition[0][capacity_count] = (line.at(0) <= '6');
	    result.condition[1][capacity_count] = (m_capacity.str(2) == "St");
	    ++capacity_count;
	    continue;
	}
	static const boost::regex r_person("\"?([^,]*)\"?,(\\d+),(\\d+),(\\d+),\"?([^,]+)\"?,\"?([^,]+)\"?,\"(.*)\"");
	boost::smatch m_person;
	if (boost::regex_match(line, m_person, r_person)) {
	    std::cout << "Got person" << std::endl;
	    if (person_count == person_capacity) ioerror("Too many people");
	    person_names.push_back(m_person.str(1));
	    person_ids.insert(std::make_pair(m_person.str(1), person_count));
	    priorities_t p;
	    p.wp = boost::lexical_cast<weight_t>(m_person.str(2));
	    p.w1 = boost::lexical_cast<weight_t>(m_person.str(3));
	    p.w2 = boost::lexical_cast<weight_t>(m_person.str(4));
	    p.c1 = (m_person.str(5) == "Langelandsgade");
	    p.c2 = (m_person.str(6) == "Stue");
	    result.people.push_back(p);
	    roomies.push_back(m_person.str(7));
	    ++person_count;
	    continue;
	}
	std::cerr << line << std::endl;
	ioerror("Unrecognized line");
    }
    if (capacity_sum < person_count) {
	std::cerr << "Not enough destinations (have " << capacity_sum << " rooms for " << person_count << " people)" << std::endl;
    }
    for (size_t i = 0; i < person_count; ++i) {
	static const boost::regex r_person("[^, ][^,]*");
	boost::sregex_iterator begin = boost::make_regex_iterator(roomies[i], r_person);
	boost::sregex_iterator end;
	size_t j = 0;
	while (begin != end) {
	    std::string roomie = (*begin).str(0);
	    if (!person_ids.count(roomie)) {
		std::cerr << roomie << std::endl;
		ioerror("Unknown roomie");
	    }
	    result.people[i].roomies.push_back(person_ids[roomie]);
	    ++j;
	    ++begin;
	}
    }
    print(result);
    return result;
}

void print(const input_t & result) {
    for (size_t i = 0; i < destination_names.size(); ++i) {
	std::cout << destination_names[i] << ',' << result.capacity[i] << std::endl;
    }
    for (size_t i = 0; i < person_names.size(); ++i) {
	std::cout << person_names[i] << ','
	    << result.people[i].wp << ','
	    << result.people[i].w1 << ','
	    << result.people[i].w2 << ','
	    << (result.people[i].c1 ? "Langelandsgade" : "Norrebrogade") << ','
	    << (result.people[i].c2 ? "Stue" : "Ikke stue") << ",\"";
	for (size_t j = 0; j < result.people[i].roomies.size(); ++j) {
	    if (j) std::cout << ", ";
	    std::cout << person_names[result.people[i].roomies[j]];
	}
	std::cout << '"' << std::endl;
    }
}
};

struct sorted_solution {
    inline sorted_solution(const input_t & input, const assignment_t & solution)
	: people(input.people)
	, condition(input.condition)
	, solution(solution)
	, by_dest(solution.by_dest())
	, person_count(people.size())
	, gs(reinterpret_cast<goodness_calculation *>(new char[person_count*sizeof(goodness_calculation)]))
	, pgs(new goodness_calculation*[person_count])
    {
	for (person_t p = 0; p < person_count; ++p) {
	    const priorities_t & prio = people[p];
	    dest_t dest = solution[p];
	    destassignment_t::item_type & actual_roomies = by_dest[dest];
	    pgs[p] = new (gs+p) goodness_calculation(prio, dest, condition, actual_roomies);
	}
	std::sort(pgs+0, pgs+person_count, ptrless());
    }

    inline size_t size() {
	return person_count;
    }

    inline std::pair<size_t, const goodness_calculation &> operator[](size_t i) {
	return std::make_pair(pgs[i]-gs, *(pgs[i]));
    }
private:
    const people_t & people;
    const condition_t & condition;
    const assignment_t & solution;
    destassignment_t by_dest;
    size_t person_count;
    goodness_calculation * gs;
    goodness_calculation ** pgs;

    struct ptrless {
	template <typename T>
	inline bool operator()(const T * a, const T * b) {
	    return *a < *b;
	}
    };
};

struct cout_reporter {
    inline cout_reporter() : attempts(0) {
    }

    inline void start() {
	t.restart();
    }

    inline size_t operator++() { return ++attempts; }
    inline size_t operator++(int) { return attempts++; }
    inline void operator+=(size_t s) { attempts += s; }

    inline void operator()(const input_t & input, const assignment_t & solution, const weight_t & goodness) {
	std::cout << "\n\n";
	std::cout << t.elapsed() << " s\n";
	std::cout << "After " << attempts << " attempts in total." << std::endl;
	std::cout << "Beboer & Gang & g_p & g_s & g_e & v_p & v_s & v_e &  G & Ønskede roomies \\\\" << std::endl;

	sorted_solution sol(input, solution);
	for (person_t pp = 0; pp < sol.size(); ++pp) {
	    person_t p = sol[pp].first;
	    const goodness_calculation & c = sol[pp].second;
	    dest_t dest = solution[p];
	    std::cout << std::setw(6) << p << " &";
	    if (dest%4)
		std::cout << std::setw(2) << (4+dest/4) << '-' << dest%4 << ' ';
	    else
		std::cout << std::setw(2) << (4+dest/4) << '-' << "St";
	    std::cout << " &"
	              << std::setw(4) << c.g_p << " &"
	              << std::setw(4) << c.g_s << " &"
	              << std::setw(4) << c.g_e << " &"
	              << std::setw(4) << c.v_p << " &"
	              << std::setw(4) << c.v_s << " &"
	              << std::setw(4) << c.v_e << " &"
	              << std::setw(3) << c.G << " & ";
	    for (roomies_t::const_iterator i = c.wished_roomies.begin(); i != c.wished_roomies.end(); ++i) {
		std::cout << std::setw(3) << *i << ',';
	    }
	    std::cout << " \\\\\n";
	}
	std::cout << "Sum &&&&&&&& " << goodness << " & \\\\" << std::endl;
	std::cout << std::flush;
    }

private:
    boost::timer t;
    size_t attempts;
};

struct debug_reporter {
    inline debug_reporter()
	: lastattempts(0)
	, attempts(0)
	, nextreport(10000)
    {
    }

    inline void start() {
	t.restart();
    }

    inline void operator++() { inc(); }
    inline void operator++(int) { inc(); }
    inline void operator+=(size_t s) { inc(s); }

    inline void operator()(const input_t & /*input*/, const assignment_t & /*solution*/, const weight_t & goodness) {
	best = goodness;
    }

private:
    boost::timer t;
    double lasttime;
    size_t lastattempts;
    size_t attempts;
    size_t nextreport;
    weight_t best;

    inline void inc(size_t by = 1) {
	attempts += by;
	if (attempts > nextreport) {
	    double now = t.elapsed();
	    double speed = (attempts-lastattempts)/(now-lasttime);
	    nextreport += 0.1*speed;
	    std::cout << '\r' << now << "s\tbest = " << best << "\tattempts = " << attempts << "\tspeed = " << speed << std::flush;
	    lastattempts = attempts;
	    std::swap(now, lasttime);
	}
    }
};

#endif // __IO_H__
// vim: set sw=4 sts=4 ts=8 noet:
