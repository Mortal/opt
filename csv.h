#ifndef __CSV_H__
#define __CSV_H__

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "types.h"
#include "io.h"

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
	    ++inputline;
	    static const boost::regex r_capacity("(\\d+-(St|\\d+)),(\\d+).*");
	    boost::smatch m_capacity;
	    if (boost::regex_match(line, m_capacity, r_capacity)) {
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

    void print(std::ofstream & out, const sorted_solution & sol, double elapsed, uint64_t attempts) {
	weight_t goodness = 0;
	out << "Beboer,Godhed:,Gang,g_p,g_s,g_e,v_p,v_s,v_e,G,Ønsker:,Personer,Side,Etage," << elapsed << " s," << attempts << "\n";
	for (person_t pp = 0; pp < sol.size(); ++pp) {
	    person_t p = sol[pp].first;
	    const goodness_calculation & c = sol[pp].second;
	    dest_t dest = c.dest;
	    out << person_names[p] << ",,"
		<< destination_names[dest] << ','
		<< c.g_p << ','
		<< c.g_s << ','
		<< c.g_e << ','
		<< c.v_p << ','
		<< c.v_s << ','
		<< c.v_e << ','
		<< c.G << ",,\"";
	    for (size_t j = 0; j < sol.person(p).roomies.size(); ++j) {
		if (j) out << ", ";
		out << person_names[sol.person(p).roomies[j]];
	    }
	    out << "\","
		<< (sol.person(p).c1 ? "Langelandsgade" : "Norrebrogade") << ','
		<< (sol.person(p).c2 ? "Stue" : "Ikke stue")
		<< '\n';
	    goodness += c.G;
	}
	out << "Sum,,,,,,,,," << goodness << '\n';
    }
};

struct csv_reporter {
    inline csv_reporter(csv_parser & parser) : attempts(0), parser(parser) {
    }

    inline void start() {
	t.restart();
    }

    inline size_t operator++() { return ++attempts; }
    inline size_t operator++(int) { return attempts++; }
    inline void operator+=(size_t s) { attempts += s; }

    inline std::string name(weight_t goodness) {
	std::stringstream ss;
	ss << "opt-" << goodness << ".csv";
	return ss.str();
    }

    inline void operator()(const input_t & input, const assignment_t & solution, const weight_t & goodness) {
	std::ofstream out(name(goodness));

	sorted_solution sol(input, solution);
	parser.print(out, sol, t.elapsed(), attempts);
	std::cout << "Printed to " << name(goodness) << std::endl;
    }

private:
    boost::timer t;
    size_t attempts;
    csv_parser & parser;
};

#endif // __CSV_H__
// vim: set sw=4 sts=4 ts=8 noet:
