#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/chi_squared.hpp>

struct ci_t {
    inline ci_t(std::pair<double, double> ci)
	: first(ci.first)
	, second(ci.second)
    {
    }
    double first;
    double second;
};

std::ostream & operator<<(std::ostream & os, const ci_t & ci) {
    return os << '[' << ci.first << ',' << ci.second << ']';
}

struct normal_sample {
    inline normal_sample()
	: m_n(0)
	, m_sum(0.0)
	, m_uss(0.0)
    {
    }

    inline normal_sample(size_t n, double sum, double uss)
	: m_n(n)
	, m_sum(sum)
	, m_uss(uss)
    {
    }

    template <typename T>
    inline void operator()(const T & time) {
	++m_n;
	double x = static_cast<double>(time);
	m_uss += x*x;
	m_sum += x;
    }

    // Number of observations
    inline size_t n() const { return m_n; }
    // Degrees of freedom
    inline size_t freedom() const { return n()-1; }
    // Mean of observations
    inline double mean() const { return sum()/n(); }
    // Standard deviation
    inline double stddev() const { return sqrt(m2()/freedom()); }
    // Sum
    inline double sum() const { return m_sum; }
    // Sum of squares
    inline double uss() const { return m_uss; }
    // SSD = sum of (x_i - mean)^2
    inline double ssd() const { return uss()-sum()*sum()/n(); }
    // Estimation of variance
    inline double variance() const { return ssd()/freedom(); }

    // Confidence interval for the mean. biogeostat p. 61
    inline ci_t ci(double alpha = 0.05) const {
	if (freedom() < 2) return std::make_pair(-1.0/0.0, 1.0/0.0);
	boost::math::students_t dist(freedom());
	double T = quantile(complement(dist, alpha / 2));
	double w = T * stddev() / sqrt(n());
	return std::make_pair(mean() - w, mean() + w);
    }

    // Confidence interval for the variance. biogeostat p. 62
    inline ci_t ci_variance(double alpha = 0.05) const {
	boost::math::chi_squared_distribution<double> dist(freedom());
	double lhs = freedom()*variance()/quantile(dist, 1-alpha/2);
	double rhs = freedom()*variance()/quantile(dist, alpha/2);
	return std::make_pair(lhs, rhs);
    }

    inline normal_sample operator+(const normal_sample & ys) {
	return normal_sample(n()+ys.n(), sum()+ys.sum(), uss()+ys.uss());
    }

    // Deprecated
    inline double m2() const { return ssd(); }

private:
    size_t m_n;
    double m_sum;
    double m_uss;
};

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

// typo
typedef normal_sample normal_samples;

std::ostream & operator<<(std::ostream & os, const normal_sample & ns) {
    return os << "[Normal samples with n = " << ns.n() << ", mean = " << ns.mean() << ", m2 = " << ns.m2() << ", stddev = " << ns.stddev() << ", var = " << ns.variance() << ']';
}
// vim: set sw=4 sts=4 ts=8 noet:
