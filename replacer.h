template <typename Q>
struct replacer;

template <typename T, size_t n>
struct replacer<tournament_tree<T, n> > {
    static void replace_top(tournament_tree<T, n> * q, const T & el) {
	q->replace_top(el);
    }
};

template <typename T>
struct replacer<std::priority_queue<T> > {
    static void replace_top(std::priority_queue<T> * q, const T & el) {
	q->pop();
	q->push(el);
    }
};

template <typename Q>
struct replacer {
    template <typename T>
    static void replace_top(Q * q, const T & el) {
	q->push(el);
    }
};
