#include <iostream>
#include <sstream>
#include <cmath>
#include "stat.h"

int main() {
    double x;
    normal_samples xs;
    normal_samples ys;
    normal_samples *cur = &xs;
    std::string line;
    while (getline(std::cin, line)) {
	std::stringstream numbers(line);
	if (!(numbers >> x)) {
	    if (cur == &ys) break;
	    cur = &ys;
	} else {
	    (*cur)(log(x));
	    while (numbers >> x) {
		(*cur)(log(x));
	    }
	}
	std::cout << *cur << std::endl;
    }
    std::pair<double,double> v = common_variance(xs, ys);
    std::pair<double,double> m = common_mean(xs, ys);
    std::cout << v.first << std::endl << v.second << std::endl;
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
