#ifndef __SPEEDTEST_H__
#define __SPEEDTEST_H__

#include <boost/chrono.hpp>
#include "stat.h"

namespace C = boost::chrono;
typedef C::system_clock::time_point time_point;
inline time_point now() { return C::system_clock::now(); }

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
// vim: set sw=4 sts=4 ts=4 noet:
