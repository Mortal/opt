#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include "stat.h"

using namespace std;

int main() {
    double x;
    normal_samples xs;
    normal_samples ys;
    normal_samples *cur = &xs;
    string line;
    while (getline(cin, line)) {
	stringstream numbers(line);
	if (!(numbers >> x)) {
	    if (cur == &ys) break;
	    cur = &ys;
	} else {
	    (*cur)(x);
	    while (numbers >> x) {
		(*cur)(x);
	    }
	}
	cout << *cur << endl;
    }
    pair<double,double> v = common_variance(xs, ys);
    pair<double,double> m = common_mean(xs, ys);
    cout << "\n\nTEST RESULTS\n\nTest of common variance:\n";
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Accepted") << '\n';
    if (v.second >= 0.05) {
	cout << "s^2 = " << v.first << '\n';
	cout << "Test of common mean:\n";
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "ACCEPTED") << '\n';
	if (m.second >= 0.05) {
	    cout << "m = " << m.first << '\n';
	}
    }
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
