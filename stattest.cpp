#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "stat.h"

using namespace std;

bool read_cumulative_sample_data(normal_sample & tgt) {
    size_t n;
    double sum, uss;
    if (!(std::cin >> n >> sum >> uss)) return false;
    tgt = normal_sample(n, sum, uss);
    return true;
}

void derive(const normal_sample & xs, std::string index = "") {
    std::string mu = "mu" + index;
    std::string sum = "S" + index;
    std::string n = "n" + index;
    std::string ssd = "SSD" + index;
    std::string uss = "USS" + index;
    std::string var = "s" + index + "^2";
    cout << "Mean:         " << mu << " = " << sum << " / " << n << " = " << xs.sum() << " / " << xs.n() << " = " << xs.mean() <<
    endl << "             " << ssd << " = " << uss << " - " << sum << "^2 / " << n << " = " << xs.uss() << " - " << xs.sum() << "^2 / " << xs.n() << " = " << xs.ssd() <<
    endl << "Variance:    " << var << " = 1 / (" << n << " - 1) * " << ssd << " = " << xs.variance() << endl;
}

void more_than_two_samples(std::vector<normal_sample> & samples) {
    cout << "Testing the hypothesis of a common variance in more than two samples." <<
    endl << "H_0sigma^2 : sigma_(1)^2 = ... = sigma_(n)^2 = sigma^2" << endl;

    pair<double,double> v = common_variance(samples);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
    if (v.second >= 0.05) {
	cout << "sigma^2 <- s^2 = " << setprecision(5) << v.first << "\n\n";
	pair<double, double> m = common_mean(samples);
	cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
	if (m.second >= 0.05) {
	    cout << "mu <- m = " << m.first << "\n\n";
	    normal_sample zs = normal_sample::sum(samples.begin(), samples.end());
	    cout << "Single sample data: " << zs << endl;
	    cout << "Confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	}
    }
}

void two_samples(normal_sample & xs, normal_sample & ys) {
    cout << "Testing the hypothesis of a common variance in two samples." <<
    endl << "H_0sigma^2 : sigma_(1)^2 = sigma_(2)^2 = sigma^2" << endl;

    pair<double,double> v = common_variance(xs, ys);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
    if (v.second >= 0.05) {
	cout << "Model M_1: X_ij = N(mu_i, sigma^2) for i = 1, ..., n, j = 1, ..., n_i" << endl;
	cout << "sigma^2 <- s^2 = " << setprecision(5) << v.first << "\n\n";
	normal_samples zs = xs+ys;
	cout << "Confidence interval for the variance (biogeostat p. 61): " << setprecision(5) << zs.ci_variance() << '\n';
	pair<double,double> m = common_mean(xs, ys);
	cout << "Test of common mean:\n";
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "NOT REJECTED") << '\n';
	if (m.second >= 0.05) {
	    cout << "mu <- m = " << m.first << "\n\n";
	    cout << "Single sample data: " << zs << endl;
	    cout << "Confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	}
    }
}

void go() {
    std::vector<normal_sample> samples;
    normal_sample xs;
    size_t i = 1;
    while (read_cumulative_sample_data(xs)) {
	std::stringstream ss;
	ss << "_(" << i << ")";
	samples.push_back(xs);
	derive(xs, ss.str());
	cout << endl;
	++i;
    }
    cout << "Model M_0: X_ij = N(mu_i, sigma_i^2) for i = 1, ..., " << i << ", j = 1, ..., n_i" << endl;
    if (samples.size() < 2) return;
    if (samples.size() == 2) return two_samples(samples[0], samples[1]);
    return more_than_two_samples(samples);
}

int main() {
    go();
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
