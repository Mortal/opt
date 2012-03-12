#ifndef __SPEEDTEST_H__
#define __SPEEDTEST_H__

#include <boost/chrono.hpp>
#include <boost/math/distributions/students_t.hpp>

namespace C = boost::chrono;
typedef C::system_clock::time_point time_point;
inline time_point now() { return C::system_clock::now(); }

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

struct normal_samples {
	inline normal_samples()
		: m_n(0)
		, m_mean(0.0)
		, m_m2(0.0)
	{
	}

	template <typename T>
	inline void operator()(const T & time) {
		++m_n;
		double x = static_cast<double>(time);
		double delta = x - m_mean;
		m_mean += delta/n();
		m_m2 += delta*(x - m_mean);
	}

	size_t n() const { return m_n; }
	double mean() const { return m_mean; }
	double m2() const { return m_m2; }
	double stddev() const { return sqrt(m2()/(n()-1)); }

	ci_t ci(double alpha = 0.05) const {
		if (n() < 2) return std::make_pair(-1.0/0, 1.0/0);
		boost::math::students_t dist(n() - 1);
		double T = quantile(complement(dist, alpha / 2));
		double w = T * stddev() / sqrt(n());
		return std::make_pair(mean() - w, mean() + w);
	}

	operator double() const { return mean(); }

private:
	size_t m_n;
	double m_mean;
	double m_m2;
};

std::ostream & operator<<(std::ostream & os, const normal_samples & ns) {
	return os << "[Normal samples with n = " << ns.n() << ", mean = " << ns.ci() << ", stddev = " << ns.stddev() << ']';
}

template <typename F, typename N>
struct speedtester {
	inline speedtester(F f)
		: f(f)
	{
	}

	inline double time(N n) {
		time_point start = now();
		f(n);
		C::duration<double> sec = now()-start;
		return sec.count();
	}

	template <typename Logger>
	inline N calibrate(double target_duration, Logger & logger) {
		N n = 1;
		double t = 0.0;
		while (true) {
			logger << "\rCalibrating with n = " << n << std::flush;
			t = time(n);
			if (t > target_duration) break;
			if (2*t > target_duration) {
				n *= target_duration*1.1/t;
			} else {
				n *= 2;
			}
		}
		logger << std::endl;
		return target_duration*n/t;
	}

	template <typename Logger>
	inline normal_samples sample(N n, size_t samples, Logger & logger) {
		logger << "Sampling with n = " << n << std::endl;
		normal_samples xs;
		for (size_t i = 0; i < samples; ++i) {
			xs(time(n));
			logger << '\r' << xs << std::flush;
		}
		logger << std::endl;
		return xs;
	}

	F f;
};

#endif // __SPEEDTEST_H__
