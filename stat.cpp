#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include "stat.h"

using namespace std;

int main() {
    size_t n1, n2;
    double sum1, uss1, sum2, uss2;
    std::cin >> n1 >> sum1 >> uss1 >> n2 >> sum2 >> uss2;
    normal_samples xs(n1, sum1, uss1);
    normal_samples ys(n2, sum2, uss2);
    cout << xs << endl << ys << endl;
    pair<double,double> v = common_variance(xs, ys);
    pair<double,double> m = common_mean(xs, ys);
    cout << "\n\nTEST RESULTS\n\nTest of common variance:\n";
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Accepted") << '\n';
    if (v.second >= 0.05) {
	cout << "sigma^2 <- s^2 = " << v.first << '\n';
	cout << "Test of common mean:\n";
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "ACCEPTED") << '\n';
	if (m.second >= 0.05) {
	    cout << "mu <- m = " << m.first << '\n';
	    normal_samples zs(n1+n2, sum1+sum2, uss1+uss2);
	    cout << zs << endl;
	    cout << "Confidence interval: " << zs.ci() << '\n';
	}
    }
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
