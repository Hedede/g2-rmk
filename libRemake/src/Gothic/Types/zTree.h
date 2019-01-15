#pragma once
#include <cstddef>

template <class T>
struct zCTree {
	zCTree *parent;
	zCTree *firstChild;
	zCTree *next;
	zCTree *prev;
	T *data;
};
