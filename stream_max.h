#ifndef __STREAM_MAX_H__
#define __STREAM_MAX_H__

#include <algorithm>

template <typename T, int m>
struct streaming_max {
    inline streaming_max(T worst = std::numeric_limits<T>::min())
	: worst(worst)
	, maxel(0)
    {
    }

    inline void push(const T & el) {
	top_ref() = el;
    }

    inline T top() {
	return top_ref();
    }

    inline void pop() {
	elements[maxel] = worst;
    }
private:
    T worst;
    T elements[m];
    size_t maxel;

    inline T & top_ref() {
	maxel = std::max_element(elements+0, elements+m)-elements;
	return elements[maxel];
    }
};

#endif // __STREAM_MAX_H__
// vim: set sw=4 sts=4 ts=8 noet:
