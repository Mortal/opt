template <typename T, int m>
struct static_heap {
    inline static_heap(T worst = std::numeric_limits<T>::min())
	: worst(worst)
	, sz(0)
    {
    }

    inline void push(const T & el) {
	elements[sz++] = el;
	std::push_heap(elements+0, elements+sz);
    }

    inline T top() {
	return elements[0];
    }

    inline void pop() {
	std::pop_heap(elements+0, elements+m);
    }

    inline void replace_top(const T & el) {
	std::pop_heap(elements+0, elements+m);
	elements[m-1] = el;
	std::push_heap(elements+0, elements+m);
    }
private:
    T worst;
    T elements[m];
    size_t sz;
};
// vim: set sw=4 sts=4 ts=8 noet:
