#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "stat.h"

using namespace std;

bool read_cumulative_sample_data(normal_sample & tgt, size_t i = 1, bool prompt = false) {
    size_t n;
    double sum, uss;
    if (prompt) cout << "n" << sub(i) << " = " << flush;
    if (!(cin >> n)) return false;
    if (prompt) cout << "S" << sub(i) << " = " << flush;
    if (!(cin >> sum)) return false;
    if (prompt) cout << "USS" << sub(i) << " = " << flush;
    if (!(cin >> uss)) return false;
    tgt = normal_sample(n, sum, uss);
    return true;
}

void derive(const normal_sample & xs, string index = "") {
    string mu = MU + index;
    string sum = "S" + index;
    string n = "n" + index;
    string ssd = "SSD" + index;
    string uss = "USS" + index;
    string var = "s" + index + SQ;
    cout << "Mean:         " << mu << " = " << sum << " / " << n << " = " << xs.sum() << " / " << xs.n() << " = " << xs.mean() <<
    endl << "             " << ssd << " = " << uss << " - " << sum << SQ" / " << n << " = " << xs.uss() << " - " << xs.sum() << SQ" / " << xs.n() << " = " << xs.ssd() <<
    endl << "Variance:    " << var << " = 1 / (" << n << " - 1) * " << ssd << " = " << xs.variance() << endl;
}

void more_than_two_samples(vector<normal_sample> & samples) {
    cout << "Testing the hypothesis of a common variance in more than two samples." <<
    endl << "H_0"SIGMASQ" : "SIGMA << sub("(1)") << SQ" = ... = "SIGMA << char_subscript('(') << sub(samples.size()) << char_subscript(')') << SQ" = "SIGMASQ << endl;

    pair<double,double> v = common_variance(samples);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
    if (v.second >= 0.05) {
	cout << SIGMASQ" <- s"SQ" = " << setprecision(5) << v.first << "\n\n";
	pair<double, double> m = common_mean(samples);
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
	if (m.second >= 0.05) {
	    cout << MU" <- m = " << m.first << "\n\n";
	    normal_sample zs = normal_sample::sum(samples.begin(), samples.end());
	    cout << "Model M" << sub(2) << ": X_ij ~ N("MU", "SIGMASQ")" << endl;
	    cout << "Single sample data: " << zs << endl;
	    cout << "Confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	}
    }
}

void two_samples(normal_sample & xs, normal_sample & ys) {
    cout << "Testing the hypothesis of a common variance in two samples." <<
    endl << "H_0"SIGMASQ" : "SIGMA << sub("(1)") << SQ" = "SIGMA << sub("(2)") << SQ" = "SIGMASQ << endl;

    pair<double,double> v = common_variance(xs, ys);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
    if (v.second >= 0.05) {
	cout << "Model M" << sub(1) << ": X_ij = N("MU"_i, "SIGMASQ") for i = 1, ..., n, j = 1, ..., n_i" << endl;
	cout << SIGMASQ" <- s"SQ" = " << setprecision(5) << v.first << "\n\n";
	normal_samples zs = xs+ys;
	cout << "Confidence interval for the variance (biogeostat p. 61): " << setprecision(5) << zs.ci_variance() << '\n';
	pair<double,double> m = common_mean(xs, ys);
	cout << "Test of common mean:\n";
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "NOT REJECTED") << '\n';
	if (m.second >= 0.05) {
	    cout << MU" <- m = " << m.first << "\n\n";
	    cout << "Single sample data: " << zs << endl;
	    cout << "Confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	}
    }
}

void go() {
    vector<normal_sample> samples;
    normal_sample xs;
    size_t i = 1;
    bool prompt = isatty(0);
    while (read_cumulative_sample_data(xs, i, prompt)) {
	stringstream ss;
	ss << char_subscript('(') << sub(i) << char_subscript(')');
	samples.push_back(xs);
	derive(xs, ss.str());
	cout << endl;
	++i;
    }
    size_t k = samples.size();
    cout << "Model M" << sub(0) << ": X_ij = N("MU"_i, "SIGMA"_i"SQ") for i = 1, ..., " << k << ", j = 1, ..., n_i" << endl;
    if (samples.size() < 2) return;
    if (samples.size() == 2) return two_samples(samples[0], samples[1]);
    return more_than_two_samples(samples);
}

int main() {
    go();
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
