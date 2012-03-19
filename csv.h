#ifndef __CSV_H__
#define __CSV_H__

#include <sstream>
#include "types.h"
#include "io.h"

struct csv_reporter {
    inline csv_reporter(csv_parser &) : attempts(0) {
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
	out << t.elapsed() << " s\n";
	out << "After " << attempts << " attempts in total.\n";
	out << "Beboer,Gang,g_p,g_s,g_e,v_p,v_s,v_e,G,Ønskede roomies\n";

	sorted_solution sol(input, solution);
	for (person_t pp = 0; pp < sol.size(); ++pp) {
	    person_t p = sol[pp].first;
	    const goodness_calculation & c = sol[pp].second;
	    dest_t dest = solution[p];
	    out << p << ',';
	    if (dest%4)
		out << (4+dest/4) << '-' << dest%4 << ' ';
	    else
		out << (4+dest/4) << '-' << "St";
	    out << ','
		<< c.g_p << ','
		<< c.g_s << ','
		<< c.g_e << ','
		<< c.v_p << ','
		<< c.v_s << ','
		<< c.v_e << ','
		<< c.G << ',';
	    for (roomies_t::const_iterator i = c.wished_roomies.begin(); i != c.wished_roomies.end(); ++i) {
		out << *i << ' ';
	    }
	    out << '\n';
	}
	out << "Sum,,,,,,,," << goodness << '\n';
    }

private:
    boost::timer t;
    size_t attempts;
};

#endif // __CSV_H__
// vim: set sw=4 sts=4 ts=8 noet:
