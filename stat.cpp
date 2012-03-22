#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/chi_squared.hpp>

#include "stat.h"

// Confidence interval for the mean. biogeostat p. 61
ci_t normal_sample::ci(double alpha) const {
    if (freedom() < 2) return std::make_pair(-1.0/0.0, 1.0/0.0);
    boost::math::students_t dist(freedom());
    double T = quantile(complement(dist, alpha / 2));
    double w = T * stddev() / sqrt(n());
    return std::make_pair(mean() - w, mean() + w);
}

// Confidence interval for the variance. biogeostat p. 62
ci_t normal_sample::ci_variance(double alpha) const {
    boost::math::chi_squared_distribution<double> dist(freedom());
    double lhs = freedom()*variance()/quantile(dist, 1-alpha/2);
    double rhs = freedom()*variance()/quantile(dist, alpha/2);
    return std::make_pair(lhs, rhs);
}

// returns <estimation of variance, significance probability>
std::pair<double,double> common_variance(const normal_sample & first, const normal_sample & second) {
    if (first.variance() < second.variance()) return common_variance(second, first);
    double F = first.variance() / second.variance();
    double cdf = 1-boost::math::cdf(boost::math::fisher_f(first.n()-1, second.n()-1), F);
    double p_obs = 2*cdf;
    double var = (first.ssd() + second.ssd())/(first.n()+second.n()-2);
    return std::make_pair(var, p_obs);
}

std::pair<double, double> common_mean(const normal_sample & first, const normal_sample & second) {
    if (first.mean() < second.mean()) return common_mean(second, first);
    double variance = common_variance(first, second).first;
    double t = (first.mean() - second.mean())/sqrt(variance*(1.0/first.n() + 1.0/second.n()));
    double p_obs = 2*(1-boost::math::cdf(boost::math::students_t(first.n()+second.n()-2), t));
    double mean = (first.mean()*first.n()+second.mean()*second.n())/(first.n()+second.n());
    return std::make_pair(mean, p_obs);
}
