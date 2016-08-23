#ifndef SIZE_CHECKER_H
#define SIZE_CHECKER_H

#define COUNTER #__COUNTER__
#define CHECKER_NAME checker ## COUNTER

template <size_t size, size_t required>
struct size_printer;

template <typename T, size_t required>
struct size_checker {
	static_assert(sizeof(T) == required);
};


#endif//SIZE_CHECKER_H
