#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include "stat.h"

using namespace std;

normal_sample read_cumulative_sample_data() {
    size_t n;
    double sum, uss;
    std::cin >> n >> sum >> uss;
    return normal_sample(n, sum, uss);
}

void derive(const normal_sample & xs, std::string index = "") {
    std::string mu = "mu" + index;
    std::string sum = "S" + index;
    std::string n = "n" + index;
    std::string ssd = "SSD" + index;
    std::string uss = "USS" + index;
    std::string var = "s" + index + "^2";
    cout << "Estimate of " << mu << " = " << sum << " / " << n << " = " << xs.sum() << " / " << xs.n() << " = " << xs.mean() << endl;
    cout << ssd << " = " << uss << " - " << sum << "^2 / " << n << " = " << xs.uss() << " - " << xs.sum() << "^2 / " << xs.n() << " = " << xs.ssd() << endl;
    cout << "Variance: " << var << " = 1 / (" << n << " - 1) * " << ssd << " = " << xs.variance() << endl;
}

int main() {
    normal_samples xs = read_cumulative_sample_data();
    normal_samples ys = read_cumulative_sample_data();
    derive(xs, "_(1)");
    cout << endl;
    derive(ys, "_(2)");
    cout << endl;
    pair<double,double> v = common_variance(xs, ys);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Accepted") << '\n';
    if (v.second >= 0.05) {
	cout << "sigma^2 <- s^2 = " << setprecision(5) << v.first << "\n\n";
	normal_samples zs = xs+ys;
	cout << "Confidence interval for the variance (biogeostat p. 61): " << setprecision(5) << zs.ci_variance() << '\n';
	pair<double,double> m = common_mean(xs, ys);
	cout << "Test of common mean:\n";
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "ACCEPTED") << '\n';
	if (m.second >= 0.05) {
	    cout << "mu <- m = " << m.first << "\n\n";
	    cout << "Single sample data: " << zs << endl;
	    cout << "Confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	}
    }
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
