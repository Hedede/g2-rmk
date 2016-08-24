#ifndef SIZE_CHECKER_H
#define SIZE_CHECKER_H

#define CONCAT1(a,b) a ## b
#define CONCAT(a,b) CONCAT1(a,b)
#define COUNTER __COUNTER__
#define CHECKER_NAME CONCAT(checker_,COUNTER)

#define CHECK_SIZE(T,S) namespace { size_checker<T,S> CHECKER_NAME; }

template <size_t size>
struct size_printer;

template <typename T, size_t required>
struct size_checker {
	static_assert(sizeof(T) == required);
};


#endif//SIZE_CHECKER_H
